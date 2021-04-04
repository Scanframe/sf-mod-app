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
		 * Function type of the lambda or function.
		 */
		typedef std::function<Result(Args...)> func_type;

		/**
		 * Default constructor
		 */
		TClosure() = default;

		/**
		 * Copy constructor
		 */
		TClosure(const TClosure& c)
		{
			_func = c ? c._func : nullptr;
		}

		/**
		 * Function assignment constructor.
		 * @param fn
		 */
		explicit TClosure(const func_type& fn)
			:_func(fn) {}

		/**
		 * Function assignment member.
		 * @param fn
		 */
		TClosure& assign(const func_type& fn)
		{
			_func = fn;
			return *this;
		}

		/**
		 * Function assignment member.
		 * @param fn
		 */
		TClosure& unassign()
		{
			_func = nullptr;
			return *this;
		}

		/**
		 * Closure assignment operator.
		 * @param c
		 */
		TClosure& operator=(TClosure c)
		{
			_func = c._func;
			return *this;
		}

		/**
		 * Closure assignment operator.
		 * @param f
		 */
		TClosure& operator=(func_type f)
		{
			_func = f;
			return *this;
		}

		/**
		 * Make the call to the member function.
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result operator()(Args... args)
		{
			return call(args...);
		}

		/**
		 * Checks if the closure is valid for calling.
		 */
		[[nodiscard]] inline bool isAssigned() const
		{
			return _func != nullptr;
		}

		/**
		 * Checks if the closure is valid for calling.
		 */
		explicit operator bool() const
		{
			return isAssigned();
		}

	protected:
		/**
		 * Makes the call to the member function.
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result call(Args... args);

	private:
		/**
		 * Member holding the function.
		 */
		func_type _func{nullptr};
};

template<typename Result, typename... Args>
Result TClosure<Result, Args...>::call(Args... args)
{
	// Bail out when called and not assigned.
	if (!isAssigned())
	{
		throw std::bad_function_call();
	}
	// Call the assigned function.
	return _func(args...);
}

} // namespace
