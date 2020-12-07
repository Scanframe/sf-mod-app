/*
General functions, defines, type definitions and templates to
make C++ programming easier.
*/

#ifndef MISC_GENUTILS_H
#define MISC_GENUTILS_H

// Import of shared library export defines.
#include "global.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <string>
#include <iostream>
#include <memory>
#if !IS_WIN
#include <pwd.h>
#include <grp.h>
#include <openssl/md5.h>
#include <sys/stat.h>
#endif

// Import of template class mcvector.
#include "mcvector.h"

namespace sf
{
//using namespace std;

/**
* Calculates the offset for a given range and setpoint.
*/
template <class T, class S>
inline
S calc_offset(T value, T min, T max, S len, bool clip)
{
	max -= min;
	value -= min;
	S temp = ((max) && (value)) ? S((value * len) / max) : S(0);
	return (clip) ? ((temp > len) ? len : (temp < S(0)) ? S(0) : temp) : temp;
}

/**
* Return clipped value of v between a and b where a < b.
*/
template <class T>
T clip(const T v, const T a, const T b)
{
	return (v < a) ? a : ((v > b) ? b : v);
}

/**
* Deletes object and clears pointer
*
* Defines and templates for deleting allocated memory and
* testing for zero and clearing the pointer at the same time.
*/
template <class T>
inline void delete_null(T& p)
{
	if (p)
	{
		delete p;
		p = NULL;
	}
}

//
template <class T>
inline void delete_anull(T& p)
{
	if (p)
	{
		delete[] p;
		p = NULL;
	}
}

//
template <class T>
inline void free_null(T& p)
{
	if (p)
	{
		free(p);
		p = NULL;
	}
}

template <class T>
inline void swap_it(T& t1, T& t2)
{
	T t(t1);
	t1 = t2;
	t2 = t;
}

template <class T>
class scope_delete
{
	public:
		explicit scope_delete(T* p) {P = p;}

		~scope_delete() {delete_null(P);}

		void disable_delete() {P = NULL;}

	private:
		T* P;
};

template <class T>
class scope_free
{
	public:
		explicit scope_free(T* p) {P = p;}

		~scope_free() {free_null(P);}

		void disable_free() {P = NULL;}

	private:
		T* P;
};

/**
* A pair of smart pointer template classes. Provides basic conversion
* operator to T*, as well as dereferencing (*), and 0-checking (!).
* These classes assume that they alone are responsible for deleting the
* object or array unless Relinquish() is called.
*/
template <typename T>
class TPointerBase
{
	public:
		T& operator*() {return *P;}

		explicit operator T*() {return P;}

		int operator!() const {return P == 0;}

		T* Relinquish()
		{
			T* p = P;
			P = 0;
			return p;
		}

	protected:
		explicit TPointerBase(T* pointer) : P(pointer) {}

		TPointerBase() : P(0) {}

		T* P;

	private:
		// Prohibit use of new
		void* operator new(size_t) noexcept {return nullptr;}

		// Delete only sets pointer to null.
		void operator delete(void* p) {((TPointerBase<T>*) p)->P = 0;}
};

/**
* Pointer to a single object. Provides member access operator ->
*/
template <typename T>
class TPointer : public TPointerBase<T>
{
	private:
		typedef TPointerBase<T> TBase;

	public:
		TPointer() : TBase() {}

		explicit TPointer(T* pointer) : TBase(pointer) {}

		~TPointer() {delete TBase::P;}

		TPointer<T>& operator=(T* src)
		{
			delete TBase::P;
			TBase::P = src;
			return *this;
		}

		// Could throw exception if P==0
		T* operator->() {return TBase::P;}
};

/**
* Pointer to an array of type T. Provides an array subscript operator
* and uses array delete[]
*/
template <typename T>
class TAPointer : public TPointerBase<T>
{
	private:
		typedef TPointerBase<T> TBase;

	public:
		TAPointer() : TBase() {}

		explicit TAPointer(T array[]) : TBase(array) {}

		~TAPointer() {delete[] TBase::P;}

		TAPointer<T>& operator=(T src[])
		{
			delete[] TBase::P;
			TBase::P = src;
			return *this;
		}

		T& operator[](int i) {return TBase::P[i];}  // Could throw exception if P==0
};

/**
* Envelope-letter type of smart pointers. In this implementation the envelope
* acts as a smart pointer to a reference-counted internal letter. This allows
* a lot of flexibility & safety in working with a single object (associated
* with the letter) being assigned to numerous envelopes in many scopes. An 'A'
* version is provided for use with arrays.
*<pre>
* Use like:
*  TEnvelope<T> e1 = new T(x,y,x);  // e1 now owns a T in a letter
*  e1->Func();                      // invokes Func() on the new T
*  Func(*e1);                       // passing a T& or a T this way
*  TEnvelope<T> e2 = e1;            // e2 safely shares the letter with e1
*
*  TAEnvelope<T> e1 = new T[99];    // e1 now owns T[] in a letter
*  e1[i].Func();                    // invokes Func() on a T element
*  Func(e1[i]);                     // passing a T& or a T this way
*  TAEnvelope<T> e2 = e1;           // e2 safely shares the letter with e1
*</pre>
*/
template <class T>
class TEnvelope
{
	public:
		explicit TEnvelope(T* object) : Letter(new TLetter(object)) {}

		TEnvelope(const TEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}

		~TEnvelope() {Letter->Release();}

		TEnvelope& operator=(const TEnvelope& src);
		TEnvelope& operator=(T* object);

		T* operator->() {return Letter->Object;}

		T& operator*() {return *Letter->Object;}

		T* operator()() {return Letter->Object;}

		explicit operator T*() {return Letter->Object;}

		int RefCount() const {return Letter ? Letter->RefCount : 0;}

	private:
		struct TLetter
		{
			explicit TLetter(T* object) : Object(object), RefCount(1) {}

			~TLetter()
			{
				delete Object;
			}

			void AddRef()
			{
				RefCount++;
			}

			void Release()
			{
				if (--RefCount == 0)
				{
					delete this;
				}
			}

			T* Object;
			int RefCount;
		};

		TLetter* Letter;
};

//
template <class T>
TEnvelope<T>& TEnvelope<T>::operator=(const TEnvelope<T>& src)
{
	if (this != &src)
	{
		Letter->Release();
		Letter = src.Letter;
		Letter->AddRef();
	}
	return *this;
}

//
template <class T>
TEnvelope<T>& TEnvelope<T>::operator=(T* object)
{
	Letter->Release();
	Letter = new TLetter(object);  // Assumes non-null! Use with new
	return *this;
}

/**
* class TAEnvelope
*/
template <class T>
class TAEnvelope
{
	public:
		explicit TAEnvelope(T array[]) : Letter(new TLetter(array)) {}

		TAEnvelope(const TAEnvelope& src) : Letter(src.Letter) {Letter->AddRef();}

		~TAEnvelope() {Letter->Release();}

		TAEnvelope& operator=(const TAEnvelope& src);
		TAEnvelope& operator=(T array[]);

		T& operator[](int i) {return Letter->Array[i];}

		T* operator*() {return Letter->Array;}

	private:
		struct TLetter
		{
			explicit TLetter(T array[]) : Array(array), RefCount(1) {}

			~TLetter() {delete[] Array;}

			void AddRef() {RefCount++;}

			void Release()
			{
				if (--RefCount == 0)
				{delete this;}
			}

			T* Array;
			int RefCount;
		};

		TLetter* Letter;
};

//
template <class T>
TAEnvelope<T>& TAEnvelope<T>::operator=(const TAEnvelope<T>& src)
{
	if (this != &src)
	{
		Letter->Release();
		Letter = src.Letter;
		Letter->AddRef();
	}
	return *this;
}

//
template <class T>
TAEnvelope<T>& TAEnvelope<T>::operator=(T array[])
{
	Letter->Release();
	Letter = new TLetter(array);  // Assumes non-null! Use with new
	return *this;
}

/**
 * Template class for managing bit maks preferably when bits are
 * defined as enumerate values.
 */
template <class T>
struct TSet
{
	/**
	 * Default constructor.
	 */
	TSet() : Bits(0) {}

	/**
	 * Init constructor.
	 */
	explicit TSet(int bits) : Bits(bits) {}

	/**
	 * Copy constructor.
	 */
	TSet(const TSet& set) : Bits(set.Bits) {}

	/**
	 * Assign operator.
	 */
	TSet& operator=(const TSet& set)
	{
		Bits = set.Bits;
		return *this;
	}

	/**
	 * Returns true when the bit was set.
	 */
	bool Has(T bit) const {return (Bits & (1L << bit)) > 0;}

	/**
	 * Sets bits in the mask.
	 */
	TSet& Set(T bit)
	{
		Bits |= (1L << bit);
		return *this;
	}

	/**
	 * Unsets bits in the mask.
	 */
	TSet& Unset(T bit)
	{
		Bits &= ~(1L << bit);
		return *this;
	}

	/**
	 * Toggles a bit in the mask.
	 */
	TSet& Toggle(T bit) {return Has(bit) ? Unset(bit) : Set(bit);}

	/**
	 * Compare equal operator.
	 */
	bool operator==(const TSet& set) const {return Bits == set.Bits;}

	/**
	 * Compare unequal operator.
	 */
	bool operator!=(const TSet& set) const {return Bits != set.Bits;}

	/**
	 * Operators and functions that are also available in the VCL 'Set' template.
	 * Returns true if bit is in the bit mask.
	 */
	bool Contains(T bit) const {return Has(bit);}

	/**
	 * Operator for adding bits to the mask.
	 */
	TSet& operator<<(T bit)
	{
		Set(bit);
		return *this;
	}

	/**
	 * Operator for removing bits from the mask.
	 */
	TSet& operator>>(T bit)
	{
		Unset(bit);
		return *this;
	}

	/**
	 * Holds the actual bits.
	 */
	int Bits;
};

//
// Template for creating large memory inexpensive bitmasks or bit sets.
//
template <size_t Size>
class TBitSet
{
	public:
		/**
		 * Default constructor.
		 */
		TBitSet();
		/**
		 * Tests if a bit has been set.
		 */
		bool Has(int bit);
		/**
		 * Sets a single bit.
		 */
		void Set(int bit);
		/**
		 * Sets a single bit.
		 */
		void Reset(int bit);
		/**
		 * Clears all bits.
		 */
		void Clear();
		/**
		 * Returns true if one of the bits has been set.
		 */
		bool IsClear();

	private:
		/**
		 * Holds the actual data.
		 */
		int FMask[Size / sizeof(int) + 1]{};
		//
		enum {BITSPERBYTE = 8};
};

//
template <size_t Size>
inline
TBitSet<Size>::TBitSet()
	:FMask({0})
{
	Clear();
}

//
template <size_t Size>
inline
bool TBitSet<Size>::Has(int bit)
{
	if (bit < Size && bit >= 0)
	{
		return (FMask[bit / (sizeof(int) * BITSPERBYTE)] & (1 << (bit % (sizeof(int) * BITSPERBYTE))));
	}
	return false;
}

//
template <size_t Size>
inline
void TBitSet<Size>::Set(int bit)
{
	if (bit < Size && bit >= 0)
	{
		FMask[bit / (sizeof(int) * BITSPERBYTE)] |= (1 << (bit % (sizeof(int) * BITSPERBYTE)));
	}
}

//
template <size_t Size>
inline
void TBitSet<Size>::Reset(int bit)
{
	if (bit < Size && bit >= 0)
	{
		FMask[bit / (sizeof(int) * BITSPERBYTE)] &= ~(1 << (bit % (sizeof(int) * BITSPERBYTE)));
	}
}

//
template <size_t Size>
inline
void TBitSet<Size>::Clear()
{
	memset(&FMask, 0, sizeof(FMask));
}

//
template <size_t Size>
bool TBitSet<Size>::IsClear()
{
	for (size_t i = 0; i < sizeof(FMask) / sizeof(int); i++)
	{
		if (FMask[i])
		{
			return false;
		}
	}
	return true;
}

/**
* creates a formatted string and returns it in a string class instance.
*/
_MISC_FUNC std::string stringf(const char* fmt, ...);

/**
 * Better implementation of 'stringf' ?
 */
template <typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	// Extra space for '\0'
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	// We don't want the '\0' inside
	return std::string(buf.get(), buf.get() + size - 1);
}

// converts a integer value to bit string '0' and '1' characters
// where the first character is the first bit
_MISC_FUNC std::string bittostring(unsigned long wrd, size_t count);

// Returns numeric the value of the passed hexadecimal character
_MISC_FUNC char hexcharvalue(char ch);

// Converts a hexadeciamal string to a block of data
_MISC_FUNC size_t stringhex(const char* hexstr, void* buffer, size_t sz);

// Converts a block of data to a hexadecimal string
_MISC_FUNC std::string hexstring(const void* buffer, size_t sz);

/**
* Checks if a keyboard key pas pressed.
*/
_MISC_FUNC bool kbhit();
/**
 * Add missing gettid() system call.
 */
_MISC_FUNC pid_t gettid() noexcept;

/**
* Counted vector of strings.
*/
typedef mcvector<std::string> strings;
/**
* Counted vector iterator of strings.
*/
typedef mciterator<std::string> strings_iter;
/**
 * Implodes a string list into a single string.
 */
_MISC_FUNC std::string implode(strings strs, std::string glue, bool skip_empty = false);
/**
 * Explodes the passed string into a strings using the separator.
 * When skip_empty is true empty strings are ignored.
 */
_MISC_FUNC strings explode(std::string str, std::string separator, bool skip_empty = false);

#if !IS_WIN
/**
 * Type for storing an MD5 hash.
 */
// Set byte alignment to 1 byte.
#pragma pack(push, 1)
typedef union
{
	unsigned char data[MD5_DIGEST_LENGTH];
	// For faster compare.
	int64_t ints[2];
} md5hash_t;
// Restore the pack option.
#pragma pack(pop)
/**
 * Compare operator for storing an MD5 hash.
 */
bool operator==(const md5hash_t& h1, const md5hash_t& h2);
/**
 * Returns the MD5 hash of the passed string.
 */
_MISC_FUNC md5hash_t md5(const char* s);
/**
 * Returns the MD5 hash of the passed string.
 */
_MISC_FUNC md5hash_t md5(std::string s);
/**
 * Returns the md5 hexadecimal string of the passed string.
 */
_MISC_FUNC std::string md5str(std::string s);
/**
 * Returns the string md5 hexadecimal representation string of the hash.
 */
_MISC_FUNC std::string md5hexstr(const md5hash_t& hash);

#endif // WIN32

/**
* Return a line from the input stream.
*/
_MISC_FUNC std::string getline(std::istream& is);
/**
* Converts the passed string into a lower case one and returns it.
*/
_MISC_FUNC std::string getcwdstr();
/**
* Returns the timespec as function return value as clock_gettime().
*/
_MISC_FUNC timespec gettime();
/**
* Returns the unmangled function name returned by type ID.
*/
_MISC_FUNC std::string demangle(const char* name);
/**
* Compares the 2 times.
* Returns -1, 0, 1 respectively for smaller, equal en larger.
*/
_MISC_FUNC int timespeccmp(const timespec& ts1, const timespec& ts2);
/**
* Converts the passed string into a lower case one and returns it.
*/
_MISC_FUNC std::string tolower(std::string s);
/**
* Converts the passed string into a upper case one and returns it.
*/
_MISC_FUNC std::string toupper(std::string s);
/**
* Trims a passed string at both sides and returns it.
*/
_MISC_FUNC std::string trim(std::string s, const std::string& t = " ");
/**
* Trims a passed string left and returns it.
*/
_MISC_FUNC std::string trim_left(std::string s, const std::string& t = " ");
/**
* Trims a passed string right and returns it.
*/
_MISC_FUNC std::string trim_right(std::string s, const std::string& t = " ");
/**
* Returns the same string but now uses a new buffer making the string thread save.
*/
_MISC_FUNC std::string unique(const std::string& s);
/**
* Returns the same string but now uses a new buffer making the string thread save.

inline
string operator + ( const char* cp, const string& s)
//string operator + (const string& s, const char* cp)
{
	return string(cp).append(s);
}
*/

/**
* Converts an integer to a string.
* itoa converts value to a null-terminated string and stores the result
* in string. With itoa, value is an integer
* <b>Note:</b> The space allocated for string must be large enough to hold
* the returned string, including the terminating null character (\0).
* itoa can return up to 33 bytes.
*/
_MISC_FUNC char* itoa(int value, char* string, int radix/* = 10*/);
/*
* The strncspn() function calculates the length of the initial segment of 's'
* which consists entirely of characters not in reject up to a maximum 'n'.
* When no reject chars were found n is returned.
*/
_MISC_FUNC size_t strncspn(const char* s, size_t n, const char* reject);
/*
* The strncspn() function calculates the length of the initial segment of 's'
* which consists entirely of characters in accept up to a maximum 'n'.
*/
_MISC_FUNC  size_t strnspn(const char* s, size_t n, const char* accept);
/*
 * Find the first occurrence of find in s, where the search is limited to the
 * first slen characters of s.
 */
_MISC_FUNC const char* strnstr(const char* s, const char* find, size_t n);
/**
* Matches a string against a wildcard string such as "*.*" or "bl?h.*" etc.
*/
_MISC_FUNC int wildcmp(const char* wild, const char* str, bool case_s);

inline
int wildcmp(const std::string& wild, const std::string& str, bool case_s)
{
	return wildcmp(wild.c_str(), str.c_str(), case_s);
}

// Returns all the files from the passed directory using a wildcard.
_MISC_FUNC bool getfiles(strings& files, std::string directory, std::string wildcard);

/**
 * Checks if the passed path exists (by doing a access() using F_OK).
 */
_MISC_FUNC bool file_exists(const char* path);

inline
bool file_exists(const std::string& path)
{
	return file_exists(path.c_str());
}

#ifndef WIN32

/**
 * Makes all directories recursively in the path.
 */
_MISC_FUNC bool file_mkdir(const char* path, __mode_t mode = 0755);

inline
bool file_mkdir(const std::string& path, __mode_t mode = 0755)
{
	return file_mkdir(path.c_str(), mode);
}

#endif


/**
 * Writes a buffer to a file.
 */
_MISC_FUNC bool file_write(const char* path, const void* buf, size_t sz, bool append = false);

inline
bool file_write(const std::string& path, const void* buf, size_t sz, bool append = false)
{
	return file_write(path.c_str(), buf, sz, append);
}

inline
bool file_write(const std::string& path, const std::string& s, bool append = false)
{
	return file_write(path.c_str(), s.c_str(), s.length(), append);
}

/**
 * Same as basename() but using an std::string.
 */
_MISC_FUNC std::string file_basename(const std::string& path);
/**
 * Same as dirname() but using an std::string.
 */
_MISC_FUNC std::string file_dirname(const std::string& path);
/**
 * Same as unlink() but using an std::string.
 */
_MISC_FUNC bool file_unlink(const std::string& path);
/**
 * Same as rename() but using an std::string.
 */
_MISC_FUNC bool file_rename(const std::string& oldpath, const std::string& newpath);
/**
 * Easier typename.
 */
typedef struct stat stat_t;
/**
 * Same as stat() but using an std::string.
 */
_MISC_FUNC bool file_stat(const std::string& path, stat_t& _stat);
/**
 * Finds the files using the passed wildcard.
 */
_MISC_FUNC bool file_find(sf::strings& files, const std::string& wildcard);
/**
 * Formats the time to a the given format.
 * When the format is NULL the XML format is used.
 * When the timeinfo is NULL the current local time is used.
 */
_MISC_FUNC std::string time_format(const struct tm* timeinfo, const char* format = nullptr);
/**
 * When time -1 the current time is used.
 */
_MISC_FUNC std::string time_format(time_t time = -1, const char* format = nullptr, bool gmtime = false);
/**
 * Returns the unix time from the passed string in the passed format.
 * When the format is NULL the XML format is used.
 * When gmtime is true GMT is used otherwise the local time.
 */
_MISC_FUNC time_t time_str2time(std::string str, const char* format = nullptr, bool gmtime = false);
/**
 * Same as mktime() only GMT is the result value not the localtime when gmtime is true.
 */
_MISC_FUNC time_t time_mktime(struct tm* tm, bool gmtime = false);
/**
 * Intermediate type to beable to create passwd_t struct/class.
 */

#ifndef WIN32

typedef struct passwd passwd_type;

/**
 * Extends struct passwd and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct passwd_t : passwd_type
{
	public:
		// Constructor allocating the right amount of memory.
		passwd_t();
		// Destructor
		~passwd_t();

		//
		explicit operator bool() const {return valid;}

	private:
		// Clears the passwd_type part only.
		void reset();
		// Holds the buffer size.
		ssize_t bufsz;
		// Holds the buffer for the passwd_type dynamic fields.
		char* buf;
		// Flag telling if this instance contains data or not.
		bool valid;

		friend bool proc_getpwnam(std::string name, passwd_t& pwd);
		friend bool proc_getpwuid(uid_t uid, passwd_t& pwd);
};

/**
 * Wrapper for getpwnam_r() but simplified using passwd_t.
 * Returns true when an entry was found.
 * Incase of an error it throws en exception.
 */
bool proc_getpwnam(std::string name, passwd_t& pwd);
/**
 * Wrapper for getpwuid_r() but simplified using passwd_t.
 * Returns true when an entry was found.
 * Incase of an error it throws en exception.
 */
bool proc_getpwuid(uid_t uid, passwd_t& pwd);
/**
 * Wrapper for setuid().
 * Incase of an error it throws en exception.
 */
void proc_setuid(uid_t uid);
/**
 * Wrapper for seteuid().
 * Incase of an error it throws en exception.
 */
void proc_seteuid(uid_t uid);

/**
 * Intermediate type to beable to create group_t struct/class.
 */
typedef struct group group_type;

/**
 * Extends struct group and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct group_t : group_type
{
	public:
		// Constructor allocating the right amount of memory.
		group_t();
		// Destructor
		~group_t();

		//
		explicit operator bool() const {return valid;}

	private:
		// Clears the passwd_type part only.
		void reset();
		// Holds the buffer size.
		ssize_t bufsz;
		// Holds the buffer for the group_type dynamic fields.
		char* buf;
		// Flag telling if this instance contains data or not.
		bool valid;

		friend bool proc_getgrnam(std::string name, group_t& grp);
		friend bool proc_getgrgid(gid_t gid, group_t& grp);
};

/**
 * Wrapper for getgrnam_r() but simplified using group_t.
 * Returns true when an entry was found.
 * Incase of an error it throws en exception.
 */
bool proc_getgrnam(std::string name, group_t& grp);
/**
 * Wrapper for getpwuid_r() but simplified using group_t.
 * Returns true when an entry was found.
 * Incase of an error it throws en exception.
 */
bool proc_getgrgid(gid_t gid, group_t& grp);
/**
 * Wrapper for setgid().
 * Incase of an error it throws en exception.
 */
void proc_setgid(gid_t gid);
/**
 * Wrapper for setegid().
 * Incase of an error it throws en exception.
 */
void proc_setegid(gid_t gid);
/**
 * Wrapper for setfsuid().
 * Incase of an error it throws en exception.
 */
void proc_setfsuid(uid_t uid);
/**
 * Wrapper for setfsgid().
 * Incase of an error it throws en exception.
 */
void proc_setfsgid(gid_t gid);

#endif // WIN32

} // namespace sf

#endif // MISC_GENUTILS_H

