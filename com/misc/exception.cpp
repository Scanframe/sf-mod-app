#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxxabi.h>
#include "exception.h"

TExceptionSystemCall::TExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func)
{
	Msg = new char[BUF_SIZE];
	int status;
	char* nm = (char*) "";
	if (mangled_name)
	{
		nm = abi::__cxa_demangle(mangled_name, 0, NULL, &status);
	}
	snprintf(Msg, BUF_SIZE, "%s::%s() %s (%i) %s", nm, func, syscall, error, strerror(error));
	if (mangled_name)
	{
		free(nm);
	}
}

TExceptionBase::TExceptionBase()
	: Msg(NULL)
{
}

//
TExceptionBase::TExceptionBase(const TExceptionBase& ex)
	: Msg(new char[BUF_SIZE])
{
	::strncpy(Msg, ex.Msg, BUF_SIZE);
}

TExceptionBase::~TExceptionBase() throw()
{
	if (Msg)
	{
		delete[] Msg;
	}
	Msg = NULL;
}

const char* TExceptionBase::what() const throw()
{
	return Msg;
}

TExceptionBase::TExceptionBase(const char* fmt, ...)
	: Msg(NULL)
{
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(Msg, BUF_SIZE, fmt, argptr);
	va_end(argptr);
}

//
void TExceptionBase::FormatMsg(const char* fmt, ...)
{
	Msg = new char[BUF_SIZE];
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(Msg, BUF_SIZE, fmt, argptr);
	va_end(argptr);
}

TException::TException()
	: TExceptionBase()
{
}

TException::TException(const TException& ex)
	: TExceptionBase(ex)
{
}

TException::TException(const char* fmt, ...)
	: TExceptionBase()
{
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(Msg, BUF_SIZE, fmt, argptr);
	va_end(argptr);
}

TException& TException::Function(const char* mangled_name, const char* func, const char* fmt, ...)
{
	Msg = new char[BUF_SIZE];
	char* Fmt = new char[BUF_SIZE];
	int status;
	char* nm = abi::__cxa_demangle(mangled_name, 0, NULL, &status);
	snprintf(Fmt, BUF_SIZE, "%s::%s() %s", nm, func, fmt);
	free(nm);
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(Msg, BUF_SIZE, Fmt, argptr);
	va_end(argptr);
	delete[] Fmt;
	return *this;
}
