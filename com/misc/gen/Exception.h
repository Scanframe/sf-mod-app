#pragma once

#include <exception>
#include <string>
#include "../global.h"

namespace sf
{

/**
 * Exception implementation.
 */
class _MISC_CLASS ExceptionBase :public std::exception
{
	public:
		/**
		 * Default Constructor.
		 */
		ExceptionBase();

		/**
		 * Default Constructor.
		 */
		ExceptionBase(const ExceptionBase& ex);

		/**
		 * Destructor.
		 */
		~ExceptionBase() noexcept override;

		/**
		 * Formatting Constructor.
		 */
		explicit ExceptionBase(const char* fmt, ...);

		/**
		 * Overloaded from base class 'std::exception'.
		 */
		[[nodiscard]] const char* what() const noexcept override;

	protected:
		/**
		 * Format message from a derived constructor.
		 */
		void FormatMsg(const char* fmt, ...);

		/**
		 * Pointer to message string.
		 */
		char* _msg;
		/**
		 * Enumerate which sets the buffer size.
		 */
		enum {BUF_SIZE = 1024};
};

/**
 * Exception implementation.
 */
class _MISC_CLASS Exception :public ExceptionBase
{
	public:
		/**
		 * Default Constructor.
		 */
		Exception();

		/**
		 * Default Constructor.
		 */
		Exception(const Exception& ex);

		/**
		 * Formatting Constructor.
		 */
		explicit Exception(const char* fmt, ...);

		/**
		 * Formatting Constructor with a class type_info, and formatting.
		 * First boolean is to get a distinct constructor.
		 */
		Exception& Function(const char* mangled_name, const char* func, const char* fmt, ...);
};

/**
 * Exception implementation for syscalls.
 */
class _MISC_CLASS ExceptionSystemCall :public ExceptionBase
{
	public:
		/**
		 * Formatting Constructor with a class type_info, function and error value.
		 */
		ExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func);
};

} // namespace
