#pragma once
#include <misc/gen/TClosure.h>

namespace sf
{

template<typename Result, typename... Args>
TClosure<Result, Args...>::TClosure(const TClosure& c)
{
	_func = c ? c._func : nullptr;
}

template<typename Result, typename... Args>
TClosure<Result, Args...>::TClosure(const func_type& fn)
	: _func(fn)
{}

template<typename Result, typename... Args>
TClosure<Result, Args...>& TClosure<Result, Args...>::assign(const func_type& fn)
{
	_func = fn;
	return *this;
}

template<typename Result, typename... Args>
template<typename ClassType, typename MethodType, typename... BoundArgs>
TClosure<Result, Args...>& TClosure<Result, Args...>::assign(ClassType* cls, MethodType mtd, BoundArgs... args)
{
	TClosure::assign(std::bind(mtd, cls, args...));
	return *this;
}

template<typename Result, typename... Args>
TClosure<Result, Args...>& TClosure<Result, Args...>::unassign()
{
	_func = nullptr;
	return *this;
}

template<typename Result, typename... Args>
TClosure<Result, Args...>& TClosure<Result, Args...>::operator=(const TClosure& c)
{
	_func = c._func;
	return *this;
}

template<typename Result, typename... Args>
TClosure<Result, Args...>& TClosure<Result, Args...>::operator=(const func_type& f)
{
	_func = f;
	return *this;
}

template<typename Result, typename... Args>
Result TClosure<Result, Args...>::operator()(Args... args) const
{
	// Bail out when called and not assigned.
	if (!isAssigned())
	{
		throw std::bad_function_call();
	}
	// Call the assigned function.
	return _func(args...);
}

template<typename Result, typename... Args>
bool TClosure<Result, Args...>::isAssigned() const
{
	return _func != nullptr;
}

template<typename Result, typename... Args>
TClosure<Result, Args...>::operator bool() const
{
	return isAssigned();
}

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

}// namespace sf