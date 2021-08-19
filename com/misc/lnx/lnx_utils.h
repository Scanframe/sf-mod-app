#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cerrno>
#include <string>
#include <iostream>
#include <memory>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <ftw.h>
#include <filesystem>
#include <openssl/md5.h>
#include "../gen/gen_utils.h"
#include "../global.h"

namespace sf
{

/**
* Checks if a keyboard key pas pressed.
*/
_MISC_FUNC bool kb_hit();
/**
 * Add missing gettid() system call.
 */
_MISC_FUNC pid_t gettid() noexcept;

/**
 * Returns all the files from the passed directory using a wildcard.
 */
_MISC_FUNC bool getFiles(strings& files, std::string directory, std::string wildcard);

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
 * Returns the filepath of the passed file descriptor.
 * @param fd Open file descriptor.
 * @return path
 */
_MISC_FUNC std::filesystem::path file_fd_path(int fd);


/**
 * Easier typename.
 */
typedef struct stat stat_t;
/**
 * Same as stat() but using an std::string.
 */
_MISC_FUNC bool file_stat(const std::string& path, stat_t& _stat);
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
_MISC_FUNC time_t time_str2time(const std::string& str, const char* format = nullptr, bool gmtime = false);
/**
 * Same as mktime() only GMT is the result value not the localtime when gmtime is true.
 */
_MISC_FUNC time_t time_mktime(struct tm* tm, bool gmtime = false);


// Set byte alignment to 1 byte.
#pragma pack(push, 1)
/**
 * Type for storing an MD5 hash.
 */
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
_MISC_FUNC bool operator==(const md5hash_t& h1, const md5hash_t& h2);
/**
 * Returns the MD5 hash of the passed string.
 */
_MISC_FUNC md5hash_t md5(const char* s);
/**
 * Returns the MD5 hash of the passed string.
 */
_MISC_FUNC md5hash_t md5(const char* s, size_t sz);
/**
 * Returns the MD5 hash of the passed string.
 */
_MISC_FUNC md5hash_t md5(const std::string& s);
/**
 * Returns the md5 hexadecimal string of the passed string.
 */
_MISC_FUNC std::string md5str(const std::string& s);
/**
 * Returns the string md5 hexadecimal representation string of the hash.
 */
_MISC_FUNC std::string md5hexstr(const md5hash_t& hash);

/**
 * Makes all directories recursively in the path.
 */
_MISC_FUNC bool file_mkdir(const char* path, __mode_t mode = 0755);

inline
bool file_mkdir(const std::string& path, __mode_t mode = 0755)
{
	return file_mkdir(path.c_str(), mode);
}

/**
 * Intermediate type to be able to create passwd_t struct/class.
 */
typedef struct passwd passwd_type;

/**
 * Extends struct passwd and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct passwd_t :passwd_type
{
	public:
		/**
		 * Constructor allocating the right amount of memory.
		 */
		passwd_t();

		/**
		 * Destructor
		 */
		~passwd_t();

		//
		explicit operator bool() const {return valid;}

	private:
		/**
		 * Clears the passwd_type part only.
		 */
		void reset();

		/**
		 * Holds the buffer size.
		 */
		ssize_t buf_sz{0};
		/**
		 * Holds the buffer for the passwd_type dynamic fields.
		 */
		char* buf{nullptr};
		/**
		 * Flag telling if this instance contains data or not.
		 */
		bool valid{};

		friend bool proc_getpwnam(std::string name, passwd_t& pwd);

		friend bool proc_getpwuid(uid_t uid, passwd_t& pwd);
};

/**
 * Wrapper for getpwnam_r() but simplified using passwd_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
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
 * In case of an error it throws en exception.
 */
void proc_setuid(uid_t uid);

/**
 * Wrapper for seteuid().
 * In case of an error it throws en exception.
 */
void proc_seteuid(uid_t uid);

/**
 * Intermediate type to be able to create group_t struct/class.
 */
typedef struct group group_type;

/**
 * Extends struct group and auto allocates needed memory.
 * Used as a result type with auto memory cleanup .
 */
struct group_t :group_type
{
	public:
		/**
		 * Constructor allocating the right amount of memory.
		 */
		group_t();

		/**
		 * Destructor
		 */
		~group_t();

		/**
		 * Boolean operator returning validity.
		 */
		explicit operator bool() const
		{
			return valid;
		}

	private:
		/**
		 * Clears the passwd_type part only.
		 */
		void reset();

		/**
		 * Holds the buffer size.
		 */
		ssize_t buf_sz{0};
		/**
		 * Holds the buffer for the group_type dynamic fields.
		 */
		char* buf{nullptr};
		/**
		 * Flag telling if this instance contains data or not.
		 */
		bool valid{false};

		friend bool proc_getgrnam(const std::string& name, group_t& grp);

		friend bool proc_getgrgid(gid_t gid, group_t& grp);
};

/**
 * Wrapper for getgrnam_r() but simplified using group_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
_MISC_FUNC bool proc_getgrnam(const std::string& name, group_t& grp);

/**
 * Wrapper for getpwuid_r() but simplified using group_t.
 * Returns true when an entry was found.
 * In case of an error it throws en exception.
 */
_MISC_FUNC bool proc_getgrgid(gid_t gid, group_t& grp);

/**
 * Wrapper for setgid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setgid(gid_t gid);

/**
 * Wrapper for setegid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setegid(gid_t gid);

/**
 * Wrapper for setfsuid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setfsuid(uid_t uid);

/**
 * Wrapper for setfsgid().
 * In case of an error it throws en exception.
 */
_MISC_FUNC void proc_setfsgid(gid_t gid);

}


