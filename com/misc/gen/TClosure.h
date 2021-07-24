#pragma once

#include <functional>
#include <cassert>

namespace sf
{

template<typename Result, typename... Args>
class TClosure
{
	public:
		/**
		 * @brief Function type of the lambda or function.
		 */
		typedef std::function<Result(Args...)> func_type;

		/**
		 * @brief Default constructor
		 */
		TClosure() = default;

		/**
		 * @brief Copy constructor
		 */
		TClosure(const TClosure& c)
		{
			_func = c ? c._func : nullptr;
		}

		/**
		 * @brief Function assignment constructor.
		 */
		explicit TClosure(const func_type& fn)
			:_func(fn) {}

		/**
		 * @brief Function assignment method for static function.
		 */
		TClosure& assign(const func_type& fn)
		{
			_func = fn;
			return *this;
		}

		/**
		 * @brief Binds a non static class member function to this instance.
		 *
		 * Short for passing a std::bind(...) result the #assign().<br>
		 * For example:
		 * ```c++
		 * bool MyClass::method(const char* s, int i);
		 *
		 * TClosure<bool, const char*, int> c2
		 *
		 * c2.assign(std::bind(&MyClass::method, this, std::placeholders::_1, std::placeholders::_2))
		 * ```
		 * @tparam ClassType type having the to be called method.
		 * @tparam MethodType type from the passed class type.
		 * @tparam BoundArgs Only std::placeholders::_? enumeration values. (see std::bind)
		 *
		 * @param cls Pointer of the class having the passed method.
		 * @param mtd Pointer to method of the given class.
		 * @param args Amount of arguments needed to call the method using **std::placeholders::_?** enumeration values.
		 */
		template<typename ClassType, typename MethodType, typename... BoundArgs>
		void assign(ClassType* cls, MethodType mtd, BoundArgs... args)
		{
			TClosure<Result, Args...>::assign(std::bind(mtd, cls, args...)); // NOLINT(modernize-avoid-bind)
		}

		/**
		 * @brief Function assignment member.
		 */
		TClosure& unassign()
		{
			_func = nullptr;
			return *this;
		}

		/**
		 * @brief Closure assignment operator.
		 */
		TClosure& operator=(TClosure c)
		{
			_func = c._func;
			return *this;
		}

		/**
		 * @brief Closure assignment operator.
		 */
		TClosure& operator=(func_type f)
		{
			_func = f;
			return *this;
		}

		/**
		 * @brief Make the call to the member function.
		 *
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result operator()(Args... args) const
		{
			return call(args...);
		}

		/**
		 * @brief Checks if the closure is valid for calling.
		 */
		[[nodiscard]] inline bool isAssigned() const
		{
			return _func != nullptr;
		}

		/**
		 * @brief Checks if the closure is valid for calling.
		 */
		explicit operator bool() const
		{
			return isAssigned();
		}

	protected:
		/**
		 * @brief Makes the call to the member function.
		 *
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result call(Args... args) const;

	private:
		/**
		 * @brief Member holding the function.
		 */
		func_type _func{nullptr};
};

template<typename Result, typename... Args>
Result TClosure<Result, Args...>::call(Args... args) const
{
	// Bail out when called and not assigned.
	if (!isAssigned())
	{
		throw std::bad_function_call();
	}
	// Call the assigned function.
	return _func(args...);
}

}
