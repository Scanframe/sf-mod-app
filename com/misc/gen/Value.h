#pragma once

#include "../global.h"
#include "TVector.h"
#if IS_QT
	#include <QString>
#endif

namespace sf
{

/**
 * @brief Value container class able to performing arithmetic functions.
 *
 * Class designed to store settings and to manipulate them using overloaded basic arithmetic operators.
 */
class _MISC_CLASS Value
{
	public:
		/**
		 * @brief Type used internally for storing integers.
		 */
		typedef int64_t int_type;
		/**
		 * @brief Type used internally for storing floating point value.
		 */
		typedef double flt_type;

		/**
		 * @brief Enumerate for available types.
		 */
		enum EType
		{
			/** Instance an alias of another and only referencing.*/
			vitReference = -1,
			/** Type is invalid after reading from input stream.*/
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
		};

		/**
		 * @brief Default constructor.
		 */
		Value() = default;

		/**
		 * @brief Copy constructor.
		 *
		 * Copies the content and type of the passed instance.
		 */
		Value(const Value& v);

		/**
		 * @brief Reference constructor.
		 *
		 * Creates an alias instance to reference the instance passed by pointer.
		 * @param v Instance pointer to reference to.
		 */
		explicit Value(Value* v);

		/**
		 * @brief Constructs an empty but typed value.
		 *
		 * @param type
		 */
		explicit Value(EType type);

		/**
		 * @brief Floating point type constructor for implicit vitFloat.
		 *
		 * @param v The floating point value.
		 */
		explicit Value(flt_type v);

		/**
		 * @brief Double type constructor for implicit vitString.
		 *
		 * @param v The string value.
		 */
		explicit Value(const char* v);

		/**
		 * @brief Double type constructor for implicit vitString.
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
		 * @brief Boolean type constructor for implicit vitInteger.
		 *
		 * @param v The boolean value.
		 */
		explicit Value(bool v);

		/**
		 * @brief 32bit integer type constructor for implicit vitInteger.
		 *
		 * @param v The boolean value.
		 */
		explicit Value(int v);

		/**
		 * @brief 32-bit unsigned integer type constructor for implicit vitInteger.
		 *
		 * @param v The unsigned int value.
		 */
		explicit Value(unsigned v);

		/**
		 * @brief # 64bit int_type type constructor for implicit vitInteger.
		 *
		 * @param v The int_type value.
		 */
		explicit Value(int_type v);

		/**
		 * @brief Binary type constructor for implicit vitBinary.
		 *
		 * @param v The raw value.
		 * @param size Size of the raw value.
		 */
		Value(const void* v, size_t size);

		/**
		 * @brief  Specific type constructor.
		 *
		 * @param type Type
		 * @param content Content to store in this instance.
		 * @param size Only specified when not of type vitInteger or vitFloat
		 */
		Value(EType type, const void* content, size_t size = 0);

		/**
		 * @brief Destructor.
		 *
		 * Frees the allocated memory if some was allocated.
		 */
		~Value();

		/**
		 * @brief Set this instance to references to the instance passed here as pointer.
		 *
		 * Makes this instance an alias for another instance.
		 * @param v The instance pointer to reference to.
		 * @return Itself.
		 */
		Value& set(Value* v);

		/**
		 * @brief Copies the content and type of the passed value.
		 *
		 * An existing reference type is undone.
		 * @param v
		 * @return Itself.
		 */
		Value& set(const Value& v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(bool v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(int v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(unsigned v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(int_type v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(flt_type v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const char* v);

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const std::string& v);

#if IS_QT

		/**
		 * @brief Sets the type and content.
		 *
		 * An existing type is undone.
		 * @param v value.
		 * @return Itself.
		 */
		Value& set(const QString& as);

#endif

		/**
		 * @brief Sets the instance type and content.
		 *
		 * Called by constructors and all #set functions.
		 * @param type Type of the new content.
		 * @param content Pointer to the raw content.
		 * @param size Size when not the types #vitInteger or #vitFloat
		 * @return Itself
		 */
		Value& set(int type, const void* content, size_t size = 0);

		/**
		 * @brief Sets the type implicitly to vitBinary.
		 *
		 * An existing type is undone.
		 * @param v Raw data pointer.
		 * @param size Size of the raw data.
		 * @return Itself.
		 */
		Value& set(const void* v, size_t size);

		/**
		 * @brief Assigns a value of an instance but not changing the current type.
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const Value& v);

		/**
		 * @brief Assigns a boolean value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(bool v);

		/**
		 * @brief Assigns an integer value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(int v);

		/**
		 * @brief Assigns an unsigned integer value but not changing the current type.

		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(unsigned v);

		/**
		 * @brief Assigns an long integer value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(long v);

		/**
		 * @brief Assigns a floating point value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(flt_type v);

		/**
		 * @brief a string value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const char* v);

		/**
		 * @brief a string value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const std::string& v);

#if IS_QT

		/**
		 * @brief a Qt string value but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @return Itself
		 */
		Value& assign(const QString& s);

#endif

		/**
		 * @brief raw binary data but not changing the current type.
		 *
		 * Except for #vitUndefined and #vitInvalid type.
		 * @param v The new value.
		 * @param size Size of the raw value.
		 * @return Itself
		 */
		Value& assign(const void* v, size_t size);

		/**
		 * @brief the current type for this instance.
		 *
		 * @return The current type.
		 */
		[[nodiscard]] EType getType() const;

		/**
		 * @brief the type corresponding to the passed type string.
		 *
		 * @param type Name of the type.
		 * @return Enumeration value.
		 */
		static EType getType(const char* type);

		/**
		 * @brief the type string of the passed type enumeration value.
		 *
		 * @param type The type.
		 * @return Name of the type.
		 */
		static const char* getType(EType type);

		/**
		 * @brief Convert the instance to the passed type.
		 *
		 * @param type New type.
		 * @return False on error during conversion.
		 */
		bool setType(EType type);

		/**
		 * @brief	Checks if the instance is valid. If the type is equals vitInvalid.
		 *
		 * @return True when the instance is valid.
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * @brief	Checks if this is a numeric type (#vitFloat or #vitInteger) of instance.
		 *
		 * @return True if the type is numerical.
		 */
		[[nodiscard]] bool isNumber() const;

		/**
		 * @brief	Returns a floating point value of the current value if possible.
		 *
		 * When the current type is a string the 'cnv_err' is Set to
		 * the error position and is zero on success.
		 */
		flt_type getFloat(int* cnv_err) const;

		/**
		 * @brief	Returns a floating point value of the current value if possible.
		 */
		[[nodiscard]] flt_type getFloat() const;

		/**
		 * @brief	Returns a integer value of the current value if possible.
		 * if the current type is a string the 'cnv_err' is Set to
		 * the error position and is zero on success.
		 */
		int_type getInteger(int* cnv_err) const;

		/**
		 * @brief	Returns a integer value of the current value if possible.
		 */
		[[nodiscard]] int_type getInteger() const;

		/**
		 * @brief	Returns size of the occupied space.
		 */
		[[nodiscard]] size_t getSize() const;

		/**
		 * @brief	Returns the pointer to the binary buffer if #vitBinary.
		 *
		 * @return Pointer to raw data or nullptr when invalid.
		 */
		[[nodiscard]] const void* getBinary() const;

		/**
		 * @brief	Returns pointer to the data.
		 *
		 * Only when the type is  #vitString, vitBinary or vitCustom.
		 */
		[[nodiscard]] const char* getData() const;

		/**
		 * @brief Gets the string of the value with the specified precision.
		 *
		 * This is only useful when when the type is #vitFloat or #vitInteger
		 * @param precision When not set uses the natural formatting of the value. (#vitFloat only)
		 * @return Formatted string of the value.
		 */
		[[nodiscard]] std::string getString(int precision = std::numeric_limits<int>::max()) const;

		/**
		 * @brief Rounds the current instance to a multiple of the passed value.
		 * @param v Value to round.
		 * @return Rounded value.
		 */
		Value& round(const Value& v);

		/**
		 * @brief Returns if the instance is type is zero.
		 *
		 * For non numeric values this is when the data length is zero.<br>
		 * For numerical values this is when the value it self is zero.<br>
		 * For floating point value a special approach is taken because floating point values are approximations by default.<br>
		 * So the absolute value is compared to the minimum positive subnormal value.
		 * @return True when zero.
		 */
		[[nodiscard]] bool isZero() const;

#if IS_QT

		/**
		 * @brief operator for the QString class.
		 * @return True when equal
		 */
		Value& operator=(const QString& v);

		/**
		 * @brief Conversion operator for QString class.
		 */
		explicit operator QString() const;

#endif

		/**
		 * @brief Type operator returning a string.
		 */
		explicit operator std::string() const;

		/**
		 * @brief Type operator returning a boolean.
		 */
		explicit operator bool() const;

		/**
		 * @brief Boolean invert operator.
		 */
		bool operator!() const
		{
			return !isZero();
		}

		/**
		 * @brief Equal operator.
		 */
		int operator==(const Value& v) const;

		/**
		 * @brief Not equal operator.
		 */
		int operator!=(const Value& v) const;

		/**
		 * @brief Larger then operator.
		 */
		int operator>(const Value& v) const;

		/**
		 * @brief Larger then or equal operator.
		 */
		int operator>=(const Value& v) const;

		/**
		 * @brief Less then operator.
		 */
		int operator<(const Value& v) const;

		/**
		 * @brief Less then or equal operator.
		 */
		int operator<=(const Value& v) const;

		/**
		 * @brief Multiply by operator.
		 */
		Value& operator*=(const Value& v);

		/**
		 * @brief Divide by operator.
		 */
		Value& operator/=(const Value& v);

		/**
		 * @brief Modulus operator.
		 */
		Value& operator%=(const Value& v);

		/**
		 * @brief Add operator.
		 */
		Value& operator+=(const Value& v);

		/**
		 * @brief Subtract operator.
		 */
		Value& operator-=(const Value& v);

		/**
		 * @brief Assignment operator that only change the contents. Calls actually #assign()
		 */
		Value& operator=(const Value& v);

		/**
		 * @brief Assignment operator that only change the contents.
		 */
		Value& operator=(const std::string& v);

		/**
		 * @brief Assignment operator setting this instance to references the past instance pointer.
		 */
		Value& operator=(Value* v);

		/**
		 * @brief Limits on content sizes.
		 */
		enum
		{
			/** Maximum for string values.*/
			maxString = 0xFFFF,
			/** Maximum for binary values.*/
			maxBinary = 0xFFFF,
			/** Maximum for custom values.*/
			maxCustom = 0xFFFF
		};

		/**
		 * @brief Multiply this with the passed value.
		 */
		[[nodiscard]] Value mul(const Value& v) const;

		/**
		 * @brief Divide this with the passed value.
		 */
		[[nodiscard]] Value div(const Value& v) const;

		/**
		 * @brief Add this with the passed value.
		 */
		[[nodiscard]] Value add(const Value& v) const;

		/**
		 * @brief Subtract this with the passed value.
		 */
		[[nodiscard]] Value sub(const Value& v) const;

		/**
		 * @brief Modulus this with the passed value.
		 */
		[[nodiscard]] Value mod(const Value& v) const;

		/**
		 * @brief Compare this with the passed value.
		 *
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

		/**
		 * Holds all the type name strings from the EType enumerate.
		 */
		static const char* _typeNames[];

	public:
		/**
	  * @brief Holds the invalid string when needed.
	  */
		static const char* _invalidStr;
		/**
		 * Vector type and implicit iterator for this class.
		 */
		typedef TVector<Value> vector_type;

		friend Value operator*(const Value& v1, const Value& v2);

		friend Value operator/(const Value& v1, const Value& v2);

		friend Value operator%(const Value& v1, const Value& v2);

		friend Value operator+(const Value& v1, const Value& v2);

		friend Value operator-(const Value& v1, const Value& v2);

		friend std::ostream& operator<<(std::ostream& os, const Value& v);

		friend std::istream& operator>>(std::istream& is, Value& v);
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

/**
 * stl output stream operator.
 */
_MISC_FUNC std::ostream& operator<<(std::ostream& os, const Value& v);

/**
 * stl input stream operator.
 */
_MISC_FUNC std::istream& operator>>(std::istream& is, Value& v);

} // namespace
