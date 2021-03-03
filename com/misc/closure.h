#pragma once

#include <functional>
#include <cassert>

namespace sf
{

template<typename Result, typename... Args>
class Closure
{
	public:
		/**
		 * Function type of the lambda or function.
		 */
		typedef std::function<Result(Args...)> func_type;

		/**
		 * Default constructor
		 */
		Closure() = default;

		/**
		 * Copy constructor
		 */
		Closure(const Closure& c)
		{
			Func = c ? c.Func : nullptr;
		}

		/**
		 * Function assignment constructor.
		 * @param fn
		 */
		explicit Closure(const func_type fn)
			:Func(fn) {}

		/**
		 * Function assignment member.
		 * @param fn
		 */
		Closure& Assign(const func_type fn)
		{
			Func = fn;
			return *this;
		}

		/**
		 * Function assignment member.
		 * @param fn
		 */
		Closure& Unassign()
		{
			Func = nullptr;
			return *this;
		}

		/**
		 * Closure assignment operator.
		 * @param c
		 */
		Closure& operator=(Closure c)
		{
			Func = c.Func;
			return *this;
		}

		/**
		 * Make the call to the member function.
		 * @param args List of arguments specified by the template.
		 * @return Result type
		 */
		Result operator()(Args... args);

		/**
		 * Checks if the closure is valid for calling.
		 */
		[[nodiscard]] inline bool isAssigned() const
		{
			return Func != nullptr;
		}

		/**
		 * Checks if the closure is valid for calling.
		 */
		explicit operator bool() const
		{
			return isAssigned();
		}

	private:
		/**
		 * Member holding the function.
		 */
		func_type Func{nullptr};

};

template<typename Result, typename... Args>
Result Closure<Result, Args...>::operator()(Args... args)
{
	// Bail out when called and not assigned.
	if (!isAssigned())
	{
		throw std::bad_function_call();
	}
	// Call the assigned function.
	return Func(args...);
}

} // namespace
