#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include "Exception.h"

namespace sf
{

ExceptionBase::ExceptionBase()
	:_msg(new char[BUF_SIZE])
{
}

ExceptionBase::ExceptionBase(const ExceptionBase& ex)
	:_msg(new char[BUF_SIZE])
{
	::strncpy(_msg, ex._msg, BUF_SIZE);
}

ExceptionBase::~ExceptionBase() noexcept
{
	delete[] _msg;
	_msg = nullptr;
}

const char* ExceptionBase::what() const noexcept
{
	return _msg;
}

ExceptionBase::ExceptionBase(const char* fmt, ...)
	:_msg(nullptr)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(_msg, BUF_SIZE, fmt, arg_ptr);
	va_end(arg_ptr);
}

void ExceptionBase::FormatMsg(const char* fmt, ...)
{
	_msg = new char[BUF_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(_msg, BUF_SIZE, fmt, arg_ptr);
	va_end(arg_ptr);
}

Exception::Exception()
	: ExceptionBase() {}

Exception::Exception(const Exception& ex) = default;

Exception::Exception(const char* fmt, ...)
	: ExceptionBase()
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsnprintf(_msg, BUF_SIZE, fmt, arg_ptr);
	va_end(arg_ptr);
}

Exception Exception::Function(const char* mangled_name, const char* func, const char* fmt, ...)
{
	char* Fmt = new char[BUF_SIZE];
	int status;
	char* nm = abi::__cxa_demangle(mangled_name, 0, nullptr, &status);
	snprintf(Fmt, BUF_SIZE, "%s::%s() %s", nm, func, fmt);
	free(nm);
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(_msg, BUF_SIZE, Fmt, argptr);
	va_end(argptr);
	delete[] Fmt;
	return Exception(*this);
}

ExceptionSystemCall::ExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func)
{
	_msg = new char[BUF_SIZE];
	int status;
	char* nm = (char*) "";
	if (mangled_name)
	{
		nm = abi::__cxa_demangle(mangled_name, 0, nullptr, &status);
	}
	snprintf(_msg, BUF_SIZE, "%s::%s() %s (%i) %s", nm, func, syscall, error, strerror(error));
	if (mangled_name)
	{
		free(nm);
	}
}

} // namespace sf