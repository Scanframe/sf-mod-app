#pragma once

#include "../global.h"
#include "string.h"
#include <cxxabi.h>
#include <exception>
#include <memory>
#include <string.h>
#include <string>

namespace sf
{

/**
 * @brief Exception implementation inherited from std::exception.
 */
template<typename T>
class ExceptionBase : public std::exception
{
	public:
		/**
		 * @brief Default Constructor.
		 */
		ExceptionBase() noexcept {}

		/**
		 * @brief Copy constructor.
		 */
		ExceptionBase(const ExceptionBase& ex) noexcept
		{
#if IS_WIN
			::strncpy_s(_msg.get(), BUF_SIZE, ex._msg.get(), strlen(_msg.get()));
#else
			::strncpy(_msg.get(), ex._msg.get(), BUF_SIZE);
#endif
		}

		/**
		 * @brief Move constructor.
		 */
		ExceptionBase(const ExceptionBase&& ex) noexcept
			: _msg(std::move(ex._msg))
		{
		}

		/**
		 * @brief Overloaded from base class 'std::exception'.
		 */
		[[nodiscard]] const char* what() const noexcept override
		{
			return _msg.get();
		}

		/**
		 * @brief Formats the exception message according sprintf().
		 */
		template<typename... Args>
		void formatMessage(const char* format, Args... args) noexcept
		{
			::snprintf(_msg.get(), BUF_SIZE, format, args...);
		}
		/**
		 * @brief Formats the exception message when no arguments are given.
		 */
		void formatMessage(const char* message) noexcept
		{
#if IS_WIN
			::strncpy_s(_msg.get(), BUF_SIZE, message, strlen(message));
#else
			::strncpy(_msg.get(), message, BUF_SIZE);
#endif
		}

		/**
		 * @brief  Formatting function with a class type_info and formatting the message.
		 * @example
		 * @code
		 * ExceptionType.Function(typeid(*this).name(), __FUNCTION__, "Terminating (%d)", 123);
		 * @endcode
		 */
		template<typename... Args>
		T Function(const char* mangled_name, const char* func, const char* format, Args&&... args)
		{
			formatMessage(sf::demangle(mangled_name).append("::").append(func).append("() ").append(format).c_str(), args...);
			return *dynamic_cast<T*>(this);
		}

		/**
		 * @brief Formatting function with a class type_info and passing the message without formatting.
		 */
		T Function(const char* mangled_name, const char* func, const char* message)
		{
			formatMessage(sf::demangle(mangled_name).append("::").append(func).append("() ").append(message).c_str());
			return *dynamic_cast<T*>(this);
		}

	protected:
		/**
		 * @brief Pointer to message string.
		 */
		std::unique_ptr<char> _msg{new char[BUF_SIZE]};
		/**
		 * @brief Enumerate which sets the buffer size for formatting.
		 */
		enum
		{
			BUF_SIZE = 1024
		};
};

/**
 * @brief Exception implementation.
 */
class _MISC_CLASS Exception : public ExceptionBase<Exception>
{
	public:
		/**
		 * @brief Default Constructor.
		 */
		Exception() noexcept;

		/**
		 * @brief Copy constructor.
		 */
		Exception(const Exception& ex) noexcept = default;

		/**
		 * @brief Constructor initializing message.
		 */
		explicit Exception(const char* message) noexcept;

		/**
		 * @brief Formatting constructor.
		 */
		template<typename... Args>
		explicit Exception(const char* format, Args&&... args) noexcept
			: ExceptionBase()
		{
			formatMessage(format, args...);
		}
};

/**
 * @brief Exception implementation for system calls failing within a wrapper class.
 *
 * @code
 * throw ExceptionSystemCall("pthread_mutex_lock", error, typeid(*this).name(), __FUNCTION__);
 * @endcode
 */
class _MISC_CLASS ExceptionSystemCall : public Exception
{
	public:
		/**
		 * @brief Formatting Constructor with a class type_info, function and error value.
		 */
		ExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func);
};

}// namespace sf
