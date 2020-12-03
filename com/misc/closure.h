#ifndef MISC_CLOSURE_H
#define MISC_CLOSURE_H

#include <functional>
#include <cassert>

namespace sf
{

template <typename Result, typename... Args>
struct Closure
{
	/**
	 * Function type of the lambda or function.
	 */
	typedef std::function<Result(Args...)> _Type;
	/**
	 * Member holding the function.
	 */
	_Type Func = NULL;

	/**
	 * Default constructor
	 */
	Closure()
		: Func(nullptr) {}

	/**
	 * Pointer constructor
	 */
	explicit Closure(const Closure* c)
		: Func(c ? c->Func : NULL) {}

	/**
	 * Default constructor
	 */
	explicit Closure(const _Type fn)
		: Func(fn) {}

	/**
	 * Make the call to the member function.
	 */
	Result operator()(Args... args)
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
		return Func != nullptr;
	}
};

} // namespace sf

#endif // MISC_CLOSURE_H
