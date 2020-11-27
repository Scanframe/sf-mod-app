#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "global.h"
#include <exception>
#include <string>

/**
 * Exception implementation.
 */
class _MISC_CLASS TExceptionBase :public std::exception
{
	public:
		/**
		 * Default Constructor.
		 */
		TExceptionBase();
		/**
		 * Default Constructor.
		 */
		TExceptionBase(const TExceptionBase& ex);
		/**
		 * Destructor.
		 */
		virtual ~TExceptionBase() throw();
		/**
		 * Formatting Constructor.
		 */
		TExceptionBase(const char *fmt, ...);
		/**
		 * Overloaded from base class 'std::exception'.
		 */
		const char* what() const throw();

	protected:
		/**
		 * Format message from a derived constructor.
		 */
		void FormatMsg(const char* fmt, ...);
		/**
		 * Pointer to message string.
		 */
		char* Msg;
		/**
		 * Enumerate which sets the buffer size.
		 */
		enum {BUF_SIZE = 1024};
};

/**
* Exception implementation.
*/
class _MISC_CLASS TException :public TExceptionBase
{
	public:
		/**
		* Default Constructor.
		*/
		TException();
		/**
		* Default Constructor.
		*/
		TException(const TException& ex);
		/**
		* Formatting Constructor.
		*/
		TException(const char *fmt, ...);
		/**
		* Formatting Constructor with a class type_info, and formating.
		* First boolean is to get a distinct constructor.
		*/
		TException& Function(const char* mangled_name, const char* func, const char* fmt, ...);
};

/**
* Exception implementation for  syscalls.
*/
class _MISC_CLASS TExceptionSystemCall :public TExceptionBase
{
	public:
		/**
		* Formatting Constructor with a class type_info, function and error value.
		*/
		TExceptionSystemCall(const char* syscall, int error, const char* mangled_name, const char* func);
};

#endif // EXCEPTION_H
