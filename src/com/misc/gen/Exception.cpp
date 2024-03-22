#include "Exception.h"
#include <cstdio>
#include <cstring>
#include <cxxabi.h>

namespace sf
{

Exception::Exception() noexcept
	: ExceptionBase()
{
}

Exception::Exception(const char* message) noexcept
	: ExceptionBase()
{
	formatMessage(message);
}

ExceptionSystemCall::ExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func)
{
	formatMessage("%s::%s() %s (%i) %s", demangle(mangled_name).c_str(), func, syscall, error, error_string(error).c_str());
}

}// namespace sf