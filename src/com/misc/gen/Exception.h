#pragma once

#include "../global.h"
#include <exception>
#include <string>

namespace sf
{

/**
 * @brief Exception implementation.
 */
class _MISC_CLASS ExceptionBase :public std::exception
{
	public:
		/**
		 * @brief Default Constructor.
		 */
		ExceptionBase();

		/**
		 * @brief Copy constructor.
		 */
		ExceptionBase(const ExceptionBase& ex);

		/**
		 * @brief Destructor.
		 */
		~ExceptionBase() noexcept override;

		/**
		 * @brief Formatting Constructor.
		 */
		explicit ExceptionBase(const char* fmt, ...);

		/**
		 * @brief Overloaded from base class 'std::exception'.
		 */
		[[nodiscard]] const char* what() const noexcept override;

	protected:
		/**
		 * @brief Format message from a derived constructor.
		 */
		void FormatMsg(const char* fmt, ...);

		/**
		 * @brief Pointer to message string.
		 */
		char* _msg;
		/**
		 * @brief Enumerate which sets the buffer size.
		 */
		enum {BUF_SIZE = 1024};
};

/**
 * @brief Exception implementation.
 */
class _MISC_CLASS Exception :public ExceptionBase
{
	public:
		/**
		 * @brief Default Constructor.
		 */
		Exception();

		/**
		 * @brief Default Constructor.
		 */
		Exception(const Exception& ex);

		/**
		 * @brief Formatting Constructor.
		 */
		explicit Exception(const char* fmt, ...);

		/**
		 * @brief  Formatting Constructor with a class type_info, and formatting.
		 *
		 * First boolean is to get a distinct constructor.
		 */
		Exception Function(const char* mangled_name, const char* func, const char* fmt, ...);
};

/**
 * @brief Exception implementation for syscalls.
 */
class _MISC_CLASS ExceptionSystemCall :public ExceptionBase
{
	public:
		/**
		 * @brief Formatting Constructor with a class type_info, function and error value.
		 */
		ExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func);
};

}
