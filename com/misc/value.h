#pragma once

#include "global.h"
#include "mcvector.h"
#if IS_QT
	#include <QString>
#endif

namespace sf
{

// Forward definition.
class _MISC_CLASS TValue;

/**
 * Value container class for performing arithmetic functions.
 */
class TValue
{
	public:
		/**
		 * Type used internally for storing integers.
		 */
		typedef long long int_type;
		/**
		 * Type used internally for storing floating point value.
		 */
		typedef double flt_type;

		/**
		 * Enumerate for types.
		 */
		enum EType
		{
			// Value is an other referenced instance.
			vitREFERENCE = -1,
			vitINVALID = 0,
			// When a type is assigned it will use the assigned type.
			vitUNDEF,
			// 32-bit integer value using no additional allocated memory.
			vitINTEGER,
			// 64-bit floating point value using no additional allocated memory.
			vitFLOAT,
			// String value having additional allocated memory.
			vitSTRING,
			// Binary untyped value having additional allocated memory.
			vitBINARY,
			// Special value for now handled the same as a binary typed.
			vitSPECIAL,
			// Last entry used for iterations.
			vitLASTENTRY
		};

		/**
		 * Default constructor
		 */
		TValue() = default;

		/**
		 * Copy constructor
		 */
		TValue(const TValue& v);

		/**
		 * Reference constructor.
		 */
		explicit TValue(TValue* v);

		/**
		 * Constructs an empty but typed value.
		 * @param type
		 */
		explicit TValue(EType type);

		/**
		 * Floating point type constructor for implicit vitFLOAT.
		 * @param d The floating point value.
		 */
		explicit TValue(flt_type v);

		/**
		 * Double type constructor for implicit vitSTRING.
		 * @param s The string value.
		 */
		explicit TValue(const char* v);

		/**
		 * Double type constructor for implicit vitSTRING.
		 * @param s The std string value.
		 */
		explicit TValue(const std::string& v);

#if IS_QT

		/**
		 * QString type constructor for implicit vitSTRING.
		 * @param s The Qt string value.
		 */
		explicit TValue(const QString& s);

#endif

		/**
		 * Bool type constructor for implicit vitINTEGER.
		 * @param v The boolean value.
		 */
		explicit TValue(bool v);

		/**
		 * Integer type constructor for implicit vitINTEGER.
		 * @param v The int value.
		 */
		explicit TValue(int v);

		/**
		 * Unsigned integer type constructor for implicit vitINTEGER.
		 * @param v The unsigned int value.
		 */
		explicit TValue(unsigned v);

		/**
		 * Long integer type constructor for implicit vitINTEGER.
		 * @param v The long integer value.
		 */
		explicit TValue(long v);

		/**
		 * int_type type constructor for implicit vitINTEGER.
		 * @param v The int_type  value.
		 */
		explicit TValue(int_type v);

		/**
		 * Binary type constructor for implicit vitBINARY.
		 * @param v The raw value.
		 * @param size Size of the raw value.
		 */
		TValue(const void* v, size_t size);

		/**
		 * Specific type constructor.
		 * @param type Type
		 * @param content
		 * @param size Only specified when not of type vitINTEGER or vitFLOAT
		 */
		TValue(EType type, const void* content, size_t size = 0);

		/**
		 * Destructor.
		 */
		~TValue();

		/**
		 * Set this instance to references the instance passed as pointer.
		 * An existing reference type is undone.
		 * @param v The instance pointer
		 * @return Itself.
		 */
		TValue& Set(TValue* v);

		/**
		 * Copies the content and type of the passed value.
		 * An existing reference type is undone.
		 * @param v
		 * @return Itself.
		 */
		TValue& Set(const TValue& v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(bool v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(int v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(unsigned v);

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(long v);

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(int_type v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(flt_type v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(const char* v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(const std::string& v);

#if IS_QT

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		TValue& Set(const QString& as);

#endif

		/**
		 * Basic type for setting the type size only important for 'BINARY'.
		 * @param type
		 * @param content
		 * @param size
		 * @return Itself
		 */
		TValue& Set(int type, const void* content, size_t size = 0);

		/**
		 * Sets the type implicitly to vitBINARY.
		 * An existing reference type is undone.
		 * @param v Raw data pointer.
		 * @param size Size of the raw data.
		 * @return Itself.
		 */
		TValue& Set(const void* v, size_t size);

		/**
		 * Assigns a value of an instance but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(const TValue& v);

		/**
		 * Assigns a boolean value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(bool v);

		/**
		 * Assigns an integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(int v);

		/**
		 * Assigns an unsigned integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(unsigned v);

		/**
		 * Assigns an long integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(long v);

		/**
		 * Assigns a floating point value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(flt_type v);

		/**
		 * Assigns a string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(const char* v);

		/**
		 * Assigns a string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(const std::string& v);

#if IS_QT

		/**
		 * Assigns a Qt string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		TValue& Assign(const QString& s);

#endif

		/**
		 * Assigns raw binary data but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @param size Size of the raw value.
		 * @return Itself
		 */
		TValue& Assign(const void* v, size_t size);

		/**
		 * Returns the current type for this instance.
		 * @return
		 */
		[[nodiscard]] EType GetType() const;

		/**
		 * Returns the type corresponding to the passed type string.
		 * @param type Name of the type.
		 * @return Enumeration value.
		 */
		static EType GetType(const char* type);

		/**
		 * Returns the type string of the passed type enumeration value.
		 * @param type
		 * @return
		 */
		static const char* GetType(EType type);

		/**
		 * Converts the instance to a new type.
		 * @param type Enumeration type.
		 * @return False on error during conversion.
		 */
		bool SetType(EType type);

		/**
		 * Checks if the instance is valid. If the type is equals vitINVALID.
		 * @return false if the instance is valid.
		 */
		[[nodiscard]] bool IsValid() const;

		/**
		 * Checks if this is a numeric type (vitFLOAT or vitINTEGER) of instance.
		 * @return false if the type is numerical.
		 */
		[[nodiscard]] bool IsNummeric() const;

		/**
		 * Returns a floating point value of the current value if possible.
		 * if the current type is a string the 'cnv_err' is set to
		 * the error position and is zero on success.
		 */
		flt_type GetFloat(int* cnv_err) const;

		/**
		 * Returns a floating point value of the current value if possible.
		 */
		[[nodiscard]] flt_type GetFloat() const;

		/**
		 * Returns a integer value of the current value if possible.
		 * if the current type is a string the 'cnv_err' is set to
		 * the error position and is zero on success.
		 */
		int_type GetInteger(int* cnv_err) const;

		/**
		 * Returns a integer value of the current value if possible.
		 */
		[[nodiscard]] int_type GetInteger() const;

		/**
		 * Returns size of the occupied space.
		 * @return
		 */
		[[nodiscard]] size_t GetSize() const;

		/**
		 * Returns the pointer to the binary buffer if vitBINARY.
		 * @return Pointer to raw data or nullptr when invalid.
		 */
		[[nodiscard]] const void* GetBinary() const;

		/**
		 * Returns pointer to the data.
		 * @return
		 */
		[[nodiscard]] const char* GetData() const;

		// Returns string with the specified precision.
		[[nodiscard]] std::string GetString(int precision = INT_MAX) const;

		// Rounds the current instance to a multiple of the passed value
		TValue& Round(const TValue& v);

		// Returns non-zero if the instance is type is non zero
		[[nodiscard]] bool IsZero() const;

#if IS_QT

		TValue& operator=(const QString& v);

		explicit operator QString() const;

#endif

		explicit operator std::string() const;

		explicit operator int() const;

		bool operator!() const
		{
			return !IsZero();
		}

		// several comparison operators
		int operator==(const TValue& v) const;

		int operator!=(const TValue& v) const;

		int operator>(const TValue& v) const;

		int operator>=(const TValue& v) const;

		int operator<(const TValue& v) const;

		int operator<=(const TValue& v) const;

		// Several arithmetic operators that only change the contents.
		TValue& operator*=(const TValue& v);

		TValue& operator/=(const TValue& v);

		TValue& operator%=(const TValue& v);

		TValue& operator+=(const TValue& v);

		TValue& operator-=(const TValue& v);

		// Assignment operator that only change the contents.
		TValue& operator=(const TValue& v);

		TValue& operator=(const std::string& v);

		// Assignment operator setting this instance to references the past instance pointer.
		TValue& operator=(TValue* v);

		enum
		{
			maxSTRING = 0xFFFF,
			maxBINARY = 0xFFFF,
			maxSPECIAL = 0xFFFF
		};

		// const functions used by operators.
		[[nodiscard]] TValue Mul(const TValue& v) const;

		[[nodiscard]] TValue Div(const TValue& v) const;

		[[nodiscard]] TValue Add(const TValue& v) const;

		[[nodiscard]] TValue Sub(const TValue& v) const;

		[[nodiscard]] TValue Mod(const TValue& v) const;

		[[nodiscard]] int Compare(const TValue& v) const;

	private:
		/**
		 * Turns the type to the 'invalid' value.
		 */
		void MakeInvalid();

		/**
		 * Type of the content stored.
		 * Default type is vitUNDEF.
		 */
		EType _type{vitUNDEF};

		/**
		 * Size of the allocated memory pointed to by 'Data'.
		 */
		size_t _size{0};

		// Pointer to the memory location.
		union
		{
			// Pointer to special, binary or string data.
			char* _ptr;
			// Floating point value.
			flt_type _flt;
			// Integer value.
			int_type _int;
			// Reference pointer to other instance.
			TValue* _ref;
		} _data{nullptr};

		/**
	  * Holds the invalid string when needed.
	  */
		static const char* _invalidStr;
		/**
		 * Holds all the type name strings from the EType enumerate.
		 */
		static const char* _typeNames[];
		/**
		 * Additional size to allocate.
		 * Minimal is 1 for terminating a string.
		 */
		static const size_t _sizeExtra;

	public:
		/**
		 * Vector type and implicit iterator for this class.
		 */
		typedef mcvector<TValue> Vector;

		friend TValue operator*(const TValue& v1, const TValue& v2);

		friend TValue operator/(const TValue& v1, const TValue& v2);

		friend TValue operator%(const TValue& v1, const TValue& v2);

		friend TValue operator+(const TValue& v1, const TValue& v2);

		friend TValue operator-(const TValue& v1, const TValue& v2);

		friend _MISC_FUNC std::ostream& operator<<(std::ostream& os, const TValue& v);

		friend _MISC_FUNC std::istream& operator>>(std::istream& is, TValue& v);
};

inline
TValue& TValue::Set(bool v)
{
	return Set(long(v));
}

inline
TValue& TValue::Set(int v)
{
	return Set(long(v));
}

inline
TValue& TValue::Set(unsigned v)
{
	return Set(long(v));
}

inline
TValue& TValue::Set(long v)
{
	int_type ll(v);
	return Set(vitINTEGER, &ll);
}

inline
TValue& TValue::Set(int_type v)
{
	return Set(vitINTEGER, &v);
}

inline
TValue& TValue::Set(flt_type v)
{
	return Set(vitFLOAT, &v);
}

inline
TValue& TValue::Set(const char* v)
{
	return Set(vitSTRING, (void*) v);
}

inline
TValue& TValue::Set(const std::string& v)
{
	return Set(vitSTRING, (const void*) v.c_str());
}

#if IS_QT

inline
TValue& TValue::Set(const QString& as)
{
	return Set(as.toStdString());
}

#endif

inline
TValue& TValue::Set(const void* v, size_t size)
{
	return Set(vitBINARY, v, size);
}

inline
TValue& TValue::Assign(const bool v)
{
	return Assign(long(v));
}

inline
TValue& TValue::Assign(const int v)
{
	return Assign(long(v));
}

inline
TValue& TValue::Assign(const unsigned v)
{
	return Assign(long(v));
}

inline
TValue& TValue::Assign(const long v)
{
	return Assign(TValue(v));
}

inline
TValue& TValue::Assign(const flt_type v)
{
	return Assign(TValue(v));
}

inline
TValue& TValue::Assign(const char* v)
{
	return Assign(TValue(v));
}

inline
TValue& TValue::Assign(const std::string& v)
{
	return Assign(TValue(v));
}

#if IS_QT

inline
TValue& TValue::Assign(const QString& s)
{
	return Assign(TValue(s));
}

#endif

inline
TValue& TValue::Assign(const void* v, size_t size)
{
	return Assign(TValue(v, size));
}

inline
TValue::EType TValue::GetType() const
{
	return (_type == vitREFERENCE) ? _data._ref->GetType() : _type;
}

inline
bool TValue::IsValid() const
{
	return (_type == vitREFERENCE) ? _data._ref->IsValid() : _type != vitINVALID;
}

inline
bool TValue::IsNummeric() const
{
	return (_type == vitREFERENCE) ? _data._ref->IsNummeric() : _type == vitINTEGER || _type == vitFLOAT;
}

inline
size_t TValue::GetSize() const
{
	return (_type == vitREFERENCE) ? _data._ref->GetSize() : _size;
}

inline
TValue::operator int() const
{
	return !IsZero();
}

inline
TValue::operator std::string() const
{
	return GetString();
}

inline
int TValue::operator==(const TValue& v) const
{
	return !Compare(v);
}

inline
int TValue::operator!=(const TValue& v) const
{
	return Compare(v) != 0;
}

inline
int TValue::operator>(const TValue& v) const
{
	return Compare(v) > 0;
}

inline
int TValue::operator>=(const TValue& v) const
{
	return Compare(v) >= 0;
}

inline
int TValue::operator<(const TValue& v) const
{
	return Compare(v) < 0;
}

inline
int TValue::operator<=(const TValue& v) const
{
	return Compare(v) <= 0;
}

inline
TValue& TValue::operator*=(const TValue& v)
{
	return Assign(Mul(v));
}

inline
TValue& TValue::operator/=(const TValue& v)
{
	return Assign(Div(v));
}

inline
TValue& TValue::operator%=(const TValue& v)
{
	return Assign(Mod(v));
}

inline
TValue& TValue::operator+=(const TValue& v)
{
	return Assign(Add(v));
}

inline
TValue& TValue::operator-=(const TValue& v)
{
	return Assign(Sub(v));
}

inline
TValue& TValue::operator=(const TValue& v)
{
	Assign(v);
	return *this;
}

inline
TValue& TValue::operator=(const std::string& v)
{
	Assign(v);
	return *this;
}

#if IS_QT

inline
TValue& TValue::operator=(const QString& v)
{
	return Assign(v);
}

#endif

inline
TValue& TValue::operator=(TValue* v)
{
	Set(v);
	return *this;
}

inline
TValue& TValue::Set(TValue* v)
{
	return Set(vitREFERENCE, (void*) v);
}

inline
TValue::flt_type TValue::GetFloat() const
{
	return GetFloat(nullptr);
}

inline
TValue::int_type TValue::GetInteger() const
{
	return GetInteger(nullptr);
}

inline
TValue operator*(const TValue& v1, const TValue& v2)
{
	return v1.Mul(v2);
}

inline
TValue operator/(const TValue& v1, const TValue& v2)
{
	return v1.Div(v2);
}

inline
TValue operator%(const TValue& v1, const TValue& v2)
{
	return v1.Mod(v2);
}

inline
TValue operator+(const TValue& v1, const TValue& v2)
{
	return v1.Add(v2);
}

inline
TValue operator-(const TValue& v1, const TValue& v2)
{
	return v1.Sub(v2);
}

// std stream operators for the TValue class.
_MISC_FUNC std::ostream& operator<<(std::ostream& os, const TValue& v);

_MISC_FUNC std::istream& operator>>(std::istream& is, TValue& v);

} // namespace
