#ifndef MISC_CLOSURE_H
#define MISC_CLOSURE_H

#include <functional>
#include <assert.h>

namespace misc
{

template <typename _Result, typename... _Args>
struct Closure
{
	/**
	 * Function type of the lambda or function.
	 */
	typedef std::function<_Result(_Args...)> _Type;
	/**
	 * Member holding the function.
	 */
	_Type Func = NULL;

	/**
	 * Default constructor
	 */
	Closure()
		: Func(NULL) {}

	/**
	 * Default constructor
	 */
	Closure(const Closure* c)
		: Func(c->Func)
	{
	}

	/**
	 * Default constructor
	 */
	Closure(const _Type fn)
		: Func(fn) {}

	/**
	 * Make the call to the member function.
	 */
	_Result operator()(_Args... args)
	{
		// Bail out when called and not assigned.
		assert(isAssigned());
		// Call the assigned function.
		return Func(args...);
	}

	/**
	 * Checks if the closure is valid for calling.
	 */
	inline bool isAssigned() const
	{
		return Func != NULL;
	}
};

}

#endif // MISC_CLOSURE_H
