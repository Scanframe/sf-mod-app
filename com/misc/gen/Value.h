#pragma once

#include "../global.h"
#include "TVector.h"
#if IS_QT
	#include <QString>
#endif

namespace sf
{

/**
 * Value container class for performing arithmetic functions.
 */
class _MISC_CLASS Value
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
			/** Value is an other referenced instance.*/
			vitReference = -1,
			vitInvalid = 0,
			/** When a type is assigned it will use the assigned type.*/
			vitUndefined,
			/** Integer value using no additional allocated memory.*/
			vitInteger,
			/** Floating point value using no additional allocated memory.*/
			vitFloat,
			/** String value having additional allocated memory.*/
			vitString,
			/** Binary untyped value having additional allocated memory.*/
			vitBinary,
			/** Custom value for now handled the same as a binary typed.*/
			vitCustom,
			/** Last entry used for iterations.*/
			vitLastEntry
		};

		/**
		 * Default constructor
		 */
		Value() = default;

		/**
		 * Copy constructor
		 */
		Value(const Value& v);

		/**
		 * Reference constructor.
		 */
		explicit Value(Value* v);

		/**
		 * Constructs an empty but typed value.
		 * @param type
		 */
		explicit Value(EType type);

		/**
		 * Floating point type constructor for implicit vitFloat.
		 * @param v The floating point value.
		 */
		explicit Value(flt_type v);

		/**
		 * Double type constructor for implicit vitString.
		 * @param v The string value.
		 */
		explicit Value(const char* v);

		/**
		 * Double type constructor for implicit vitString.
		 * @param v The std string value.
		 */
		explicit Value(const std::string& v);

#if IS_QT

		/**
		 * QString type constructor for implicit vitString.
		 * @param v The Qt string value.
		 */
		explicit Value(const QString& v);

#endif

		/**
		 * Bool type constructor for implicit vitInteger.
		 * @param v The boolean value.
		 */
		explicit Value(bool v);

		/**
		 * Integer type constructor for implicit vitInteger.
		 * @param v The int value.
		 */
		explicit Value(int v);

		/**
		 * Unsigned integer type constructor for implicit vitInteger.
		 * @param v The unsigned int value.
		 */
		explicit Value(unsigned v);

		/**
		 * Long integer type constructor for implicit vitInteger.
		 * @param v The long integer value.
		 */
		explicit Value(long v);

		/**
		 * int_type type constructor for implicit vitInteger.
		 * @param v The int_type  value.
		 */
		explicit Value(int_type v);

		/**
		 * Binary type constructor for implicit vitBinary.
		 * @param v The raw value.
		 * @param size Size of the raw value.
		 */
		Value(const void* v, size_t size);

		/**
		 * Specific type constructor.
		 * @param type Type
		 * @param content
		 * @param size Only specified when not of type vitInteger or vitFloat
		 */
		Value(EType type, const void* content, size_t size = 0);

		/**
		 * Destructor.
		 */
		~Value();

		/**
		 * Set this instance to references the instance passed as pointer.
		 * An existing reference type is undone.
		 * @param v The instance pointer
		 * @return Itself.
		 */
		Value& set(Value* v);

		/**
		 * Copies the content and type of the passed value.
		 * An existing reference type is undone.
		 * @param v
		 * @return Itself.
		 */
		Value& set(const Value& v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(bool v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(int v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(unsigned v);

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(long v);

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(int_type v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(flt_type v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const char* v);

		/**
		 * Sets the type and content.
		 * An existing reference type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const std::string& v);

#if IS_QT

		/**
		 * Sets the type and content.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const QString& as);

#endif

		/**
		 * Basic type for setting the type size only important for 'BINARY'.
		 * @param type
		 * @param content
		 * @param size
		 * @return Itself
		 */
		Value& set(int type, const void* content, size_t size = 0);

		/**
		 * Sets the type implicitly to vitBinary.
		 * An existing reference type is undone.
		 * @param v Raw data pointer.
		 * @param size Size of the raw data.
		 * @return Itself.
		 */
		Value& set(const void* v, size_t size);

		/**
		 * Assigns a value of an instance but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const Value& v);

		/**
		 * Assigns a boolean value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(bool v);

		/**
		 * Assigns an integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(int v);

		/**
		 * Assigns an unsigned integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(unsigned v);

		/**
		 * Assigns an long integer value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(long v);

		/**
		 * Assigns a floating point value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(flt_type v);

		/**
		 * Assigns a string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const char* v);

		/**
		 * Assigns a string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const std::string& v);

#if IS_QT

		/**
		 * Assigns a Qt string value but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const QString& s);

#endif

		/**
		 * Assigns raw binary data but not changing the current type.
		 * Except for undefined and invalid type.
		 * @param v The new value.
		 * @param size Size of the raw value.
		 * @return Itself
		 */
		Value& assign(const void* v, size_t size);

		/**
		 * Returns the current type for this instance.
		 * @return
		 */
		[[nodiscard]] EType getType() const;

		/**
		 * Returns the type corresponding to the passed type string.
		 * @param type Name of the type.
		 * @return Enumeration value.
		 */
		static EType getType(const char* type);

		/**
		 * Returns the type string of the passed type enumeration value.
		 * @param type
		 * @return
		 */
		static const char* getType(EType type);

		/**
		 * Converts the instance to a new type.
		 * @param type Enumeration type.
		 * @return False on error during conversion.
		 */
		bool setType(EType type);

		/**
		 * Checks if the instance is valid. If the type is equals vitInvalid.
		 * @return false if the instance is valid.
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * Checks if this is a numeric type (vitFloat or vitInteger) of instance.
		 * @return false if the type is numerical.
		 */
		[[nodiscard]] bool isNumber() const;

		/**
		 * Returns a floating point value of the current value if possible.
		 * if the current type is a string the 'cnv_err' is Set to
		 * the error position and is zero on success.
		 */
		flt_type getFloat(int* cnv_err) const;

		/**
		 * Returns a floating point value of the current value if possible.
		 */
		[[nodiscard]] flt_type getFloat() const;

		/**
		 * Returns a integer value of the current value if possible.
		 * if the current type is a string the 'cnv_err' is Set to
		 * the error position and is zero on success.
		 */
		int_type getInteger(int* cnv_err) const;

		/**
		 * Returns a integer value of the current value if possible.
		 */
		[[nodiscard]] int_type getInteger() const;

		/**
		 * Returns size of the occupied space.
		 * @return
		 */
		[[nodiscard]] size_t getSize() const;

		/**
		 * Returns the pointer to the binary buffer if vitBinary.
		 * @return Pointer to raw data or nullptr when invalid.
		 */
		[[nodiscard]] const void* getBinary() const;

		/**
		 * Returns pointer to the data.
		 * @return
		 */
		[[nodiscard]] const char* getData() const;

		/**
		 * Returns string with the specified precision.
		 * @param precision
		 * @return
		 */
		[[nodiscard]] std::string getString(int precision = INT_MAX) const;

		/**
		 * Rounds the current instance to a multiple of the passed value
		 * @param v
		 * @return
		 */
		Value& round(const Value& v);

		/**
		 * Returns non-zero if the instance is type is non zero
		 * @return
		 */
		[[nodiscard]] bool isZero() const;

#if IS_QT

		/**
		 * Compare operator for the QString class.
		 * @return True when equal
		 */
		Value& operator=(const QString& v);

		/**
		 * Conversion operator for QString class.
		 */
		explicit operator QString() const;

#endif

		/**
		 * Type operator returning a string.
		 */
		explicit operator std::string() const;

		/**
		 * Type operator returning a boolean.
		 */
		explicit operator bool() const;

		/**
		 * Boolean invert operator.
		 */
		bool operator!() const
		{
			return !isZero();
		}

		/**
		 * Equal operator.
		 */
		int operator==(const Value& v) const;

		/**
		 * Not equal operator.
		 */
		int operator!=(const Value& v) const;

		/**
		 * Larger then operator.
		 */
		int operator>(const Value& v) const;

		/**
		 * Larger then or equal operator.
		 */
		int operator>=(const Value& v) const;

		/**
		 * Less then operator.
		 */
		int operator<(const Value& v) const;

		/**
		 * Less then or equal operator.
		 */
		int operator<=(const Value& v) const;

		/**
		 * Multiply operator.
		 */
		Value& operator*=(const Value& v);

		/**
		 * Divide operator.
		 */
		Value& operator/=(const Value& v);

		/**
		 * Modulus operator.
		 */
		Value& operator%=(const Value& v);

		/**
		 * Add operator.
		 */
		Value& operator+=(const Value& v);

		/**
		 * Subtract operator.
		 */
		Value& operator-=(const Value& v);

		/**
		 * Assignment operator that only change the contents.
		 */
		Value& operator=(const Value& v);

		/**
		 * Assignment operator that only change the contents.
		 */
		Value& operator=(const std::string& v);

		/**
		 * Assignment operator setting this instance to references the past instance pointer.
		 */
		Value& operator=(Value* v);

		/**
		 * Limits on content sizes.
		 */
		enum
		{
			maxSTRING = 0xFFFF,
			maxBINARY = 0xFFFF,
			maxSPECIAL = 0xFFFF
		};

		/**
		 * Multiply this with the passed value.
		 */
		[[nodiscard]] Value mul(const Value& v) const;

		/**
		 * Divide this with the passed value.
		 */
		[[nodiscard]] Value div(const Value& v) const;

		/**
		 * Add this with the passed value.
		 */
		[[nodiscard]] Value add(const Value& v) const;

		/**
		 * Subtract this with the passed value.
		 */
		[[nodiscard]] Value sub(const Value& v) const;

		/**
		 * Modulus this with the passed value.
		 */
		[[nodiscard]] Value mod(const Value& v) const;

		/**
		 * Compare this with the passed value.
		 * @returns 0 = Equal, > 0 = Larger, < 0 = Smaller
		 */
		[[nodiscard]] int compare(const Value& v) const;

	private:
		/**
		 * Turns the type to the 'invalid' value.
		 */
		void makeInvalid();

		/**
		 * Type of the content stored.
		 * Default type is vitUndefined.
		 */
		EType _type{vitUndefined};

		/**
		 * Size of the allocated memory pointed to by 'Data'.
		 */
		size_t _size{0};

		/**
		 * Pointer to the memory location.
		 */
		union
		{
			/**
			 * Pointer to special, binary or string data.
			 */
			char* _ptr;
			/**
			 * Floating point value.
			 */
			flt_type _flt;
			/**
			 * Integer value.
			 */
			int_type _int;
			/**
			 * Reference pointer to other instance.
			 */
			Value* _ref;
		} _data{nullptr};

		/**
		 * Additional size to allocate.
		 * Minimal is 1 for terminating a string.
		 */
		static const size_t _sizeExtra;

	public:
		/**
	  * Holds the invalid string when needed.
	  */
		static const char* _invalidStr;
		/**
		 * Holds all the type name strings from the EType enumerate.
		 */
		static const char* _typeNames[];
		/**
		 * Vector type and implicit iterator for this class.
		 */
		typedef TVector<Value> vector_type;

		friend Value operator*(const Value& v1, const Value& v2);

		friend Value operator/(const Value& v1, const Value& v2);

		friend Value operator%(const Value& v1, const Value& v2);

		friend Value operator+(const Value& v1, const Value& v2);

		friend Value operator-(const Value& v1, const Value& v2);

		friend _MISC_FUNC std::ostream& operator<<(std::ostream& os, const Value& v);

		friend _MISC_FUNC std::istream& operator>>(std::istream& is, Value& v);
};

inline
Value& Value::set(bool v)
{
	return set(long(v));
}

inline
Value& Value::set(int v)
{
	return set(long(v));
}

inline
Value& Value::set(unsigned v)
{
	return set(long(v));
}

inline
Value& Value::set(long v)
{
	int_type ll(v);
	return set(vitInteger, &ll);
}

inline
Value& Value::set(int_type v)
{
	return set(vitInteger, &v);
}

inline
Value& Value::set(flt_type v)
{
	return set(vitFloat, &v);
}

inline
Value& Value::set(const char* v)
{
	return set(vitString, (void*) v);
}

inline
Value& Value::set(const std::string& v)
{
	return set(vitString, (const void*) v.c_str());
}

#if IS_QT

inline
Value& Value::set(const QString& as)
{
	return set(as.toStdString());
}

#endif

inline
Value& Value::set(const void* v, size_t size)
{
	return set(vitBinary, v, size);
}

inline
Value& Value::assign(const bool v)
{
	return assign(long(v));
}

inline
Value& Value::assign(const int v)
{
	return assign(long(v));
}

inline
Value& Value::assign(const unsigned v)
{
	return assign(long(v));
}

inline
Value& Value::assign(const long v)
{
	return assign(Value(v));
}

inline
Value& Value::assign(const flt_type v)
{
	return assign(Value(v));
}

inline
Value& Value::assign(const char* v)
{
	return assign(Value(v));
}

inline
Value& Value::assign(const std::string& v)
{
	return assign(Value(v));
}

#if IS_QT

inline
Value& Value::assign(const QString& s)
{
	return assign(Value(s));
}

#endif

inline
Value& Value::assign(const void* v, size_t size)
{
	return assign(Value(v, size));
}

inline
Value::EType Value::getType() const
{
	return (_type == vitReference) ? _data._ref->getType() : _type;
}

inline
bool Value::isValid() const
{
	return (_type == vitReference) ? _data._ref->isValid() : _type != vitInvalid;
}

inline
bool Value::isNumber() const
{
	return (_type == vitReference) ? _data._ref->isNumber() : _type == vitInteger || _type == vitFloat;
}

inline
size_t Value::getSize() const
{
	return (_type == vitReference) ? _data._ref->getSize() : _size;
}

inline
Value::operator bool() const
{
	return !isZero();
}

inline
Value::operator std::string() const
{
	return getString();
}

inline
int Value::operator==(const Value& v) const
{
	return !compare(v);
}

inline
int Value::operator!=(const Value& v) const
{
	return compare(v) != 0;
}

inline
int Value::operator>(const Value& v) const
{
	return compare(v) > 0;
}

inline
int Value::operator>=(const Value& v) const
{
	return compare(v) >= 0;
}

inline
int Value::operator<(const Value& v) const
{
	return compare(v) < 0;
}

inline
int Value::operator<=(const Value& v) const
{
	return compare(v) <= 0;
}

inline
Value& Value::operator*=(const Value& v)
{
	return assign(mul(v));
}

inline
Value& Value::operator/=(const Value& v)
{
	return assign(div(v));
}

inline
Value& Value::operator%=(const Value& v)
{
	return assign(mod(v));
}

inline
Value& Value::operator+=(const Value& v)
{
	return assign(add(v));
}

inline
Value& Value::operator-=(const Value& v)
{
	return assign(sub(v));
}

inline
Value& Value::operator=(const Value& v)
{
	assign(v);
	return *this;
}

inline
Value& Value::operator=(const std::string& v)
{
	assign(v);
	return *this;
}

#if IS_QT

inline
Value& Value::operator=(const QString& v)
{
	assign(v);
	return *this;
}

#endif

inline
Value& Value::operator=(Value* v)
{
	set(v);
	return *this;
}

inline
Value& Value::set(Value* v)
{
	return set(vitReference, (void*) v);
}

inline
Value::flt_type Value::getFloat() const
{
	return getFloat(nullptr);
}

inline
Value::int_type Value::getInteger() const
{
	return getInteger(nullptr);
}

inline
Value operator*(const Value& v1, const Value& v2)
{
	return v1.mul(v2);
}

inline
Value operator/(const Value& v1, const Value& v2)
{
	return v1.div(v2);
}

inline
Value operator%(const Value& v1, const Value& v2)
{
	return v1.mod(v2);
}

inline
Value operator+(const Value& v1, const Value& v2)
{
	return v1.add(v2);
}

inline
Value operator-(const Value& v1, const Value& v2)
{
	return v1.sub(v2);
}

// std stream operators for the Value class.
_MISC_FUNC std::ostream& operator<<(std::ostream& os, const Value& v);

_MISC_FUNC std::istream& operator>>(std::istream& is, Value& v);

} // namespace
