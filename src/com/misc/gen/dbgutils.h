/**
	@brief contains debugging macro's that are defined when define DEBUG_LEVEL is defined as non-zero.

	Introduces debugging/logging macro's:
	====================================

	Defines that change on define _DEBUG_LEVEL:
	------------------------------------------
	NORM_NOTIFY(flags, text)
	FUNC_NOTIFY(flags, text)
	RTTI_NOTIFY(flags, text)
	COND_NORM_NOTIFY(cond, flags, text)
	COND_FUNC_NOTIFY(cond, flags, text)
	COND_RTTI_NOTIFY(cond, flags, text)

	Where 'flags' can be one or a combination of the following flags:
	----------------------------------------------------------------
	DO_DEFAULT  Predefined combination of flags.
	DO_THROW    Throw an exception.
	DO_MSGBOX   Notify through a message box.
	DO_CLOG     Notify through the std::clog output stream.
	DO_COUT     Notify through the std::cout output stream.
	DO_CERR     Notify through the std::cerr output stream.
	DO_DBGSTR   Notify through function OutputDebugString.
	DO_DBGBRK   Call debug break by raising SIGTRAP.

*/

#pragma once
#include <cstring>
#include <iomanip>
#include <iostream>
#include <misc/global.h>
#include <utility>

#if IS_QT
	#include <QDebug>
	#include <QString>
	#include <QtGlobal>
#endif

// This define should be defined externally in the project or make file.
#ifndef SF_DEBUG_LEVEL
	#define SF_DEBUG_LEVEL 1
#endif

// Coverts the passed type into a name.
#define SF_RTTI_NAME(self) sf::Demangle(typeid(self).name())
// Full current class member function name.
#define SF_RTTI_FUNCTION sf::Demangle(typeid(self).name()).append("::").append(__FUNCTION__)
// Define that converts a 'this' pointer to the class type name.
#define SF_RTTI_TYPENAME SF_RTTI_NAME(*this)
// Only the filename part of __FILE__.
#define SF_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if IS_QT

	// Converts the passed type into a name.
	#define SF_Q_RTTI_NAME(self) QString::fromStdString(SF_RTTI_NAME(self))
	// Define that converts a 'this' pointer to the class type name.
	#define SF_Q_RTTI_TYPENAME SF_Q_RTTI_NAME(*this)

inline QDebug& operator<<(QDebug& qd, const std::string& ss)
{
	return qd << QString::fromStdString(ss);
}

namespace sf
{

/**
 * Class to contain own QT message handler.
 * Is automatically installed on loading and uninstalled on unloading.
 */
class _MISC_CLASS MessageHandler
{
	public:
		/**
		 * Enables or resets the interception of messages by our own handler.
		 */
		static void enable(bool enabled = true);

	private:
		/**
		 * Holds the initial handler and  functions as a sentry for handling in @enable().
		 */
		static QtMessageHandler _initial;

		/**
		 * Handler function to install.
		 * @param type
		 * @param ctx
		 * @param msg
		 */
		static void _handler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);
};

}// namespace sf

#endif

namespace sf
{
/**
 * Debug output type flags which can be combined to create a set of flags.
 */
enum EDebugOutputType : int
{
	/** Throw an exception.*/
	dotThrow = 1U << 0U,
	/** Notify through a message box.*/
	dotMessageBox = 1U << 1U,
	/** Notify through the std::clog output stream.*/
	dotStdLog = 1U << 2U,
	/** Notify through the std::cout output stream.*/
	dotStdOut = 1U << 3U,
	/** Notify through the std::cerr output stream.*/
	dotStdErr = 1U << 4U,
	/** Notify through function OutputDebugString.*/
	dotDebugString = 1U << 5U,
	/** Call debug break function.*/
	dotDebugBreak = 1U << 6U,
	/** Use default set notify output type.*/
	dotDefault = 1U << 7U
};

/**
 * @brief Allows the passed string to be presented in various ways.
 * @param type Single or combined (or '|') value of #EDebugOutputType enumerate.
 * @param str Notification string.
 */
_MISC_FUNC void UserOutputDebugString(unsigned int type, const char* str) noexcept;

/**
 * @brief Set and get function for the type of output when DO_DEFAULT is passed.
 *
 * @param type Single or combined (or '|') value of #EDebugOutputType enumerate.
 */
_MISC_FUNC void SetDefaultDebugOutput(unsigned int type);

/**
 * @brief Returns the combination of EDebugOutputType which is the default output.
 */
_MISC_FUNC unsigned int GetDefaultDebugOutput();

/**
 * @brief Prints the backtrace to std::cerr.
 */
_MISC_FUNC void printBacktrace();

/**
 * @brief Gets the debug status
 * @return True if debugging is passed at the command line.
 */
_MISC_FUNC bool isDebug();

/**
 * @brief Gets the debugger active status.
 * @return True if a debugger is attached at startup.
 */
_MISC_FUNC bool isDebuggerActive();

/**
 * @brief Causes a break when debugging.
 */
_MISC_FUNC void debugBreak();

/**
 * @brief Closes the console window
 * @param always When true the console is closed even when debugging argument '--debug' on command line.
 */
_MISC_FUNC void freeConsole(bool always = false);

/**
 * @brief Demangles the passed rtti mangled type name.
 * @param name Mangled name
 * @return Demangled name.
 */
_MISC_FUNC std::string Demangle(const char* name);

/**
 * @brief Gets the demangled type name of the passed type T.
 * @tparam T Type to get the type string from.
 * @return Demangled type string.
 */
template<typename T>
std::string rtti_name(const T&)
{
	return Demangle(typeid(T).name());
}

/**
 * Exception thrown from a notification.
 */
class notify_exception : public std::exception
{
	public:
		explicit notify_exception(std::string s)
			: msg(std::move(s))
		{}

		[[nodiscard]] const char* what() const noexcept override
		{
			return msg.c_str();
		}

	private:
		std::string msg;
};

/**
 * @brief Debug output stream.
 */
class _MISC_CLASS debug_ostream : public std::ostringstream
{
	public:
		explicit debug_ostream(int type)
			: std::ostringstream()
			, _type(type)
		{}

		~debug_ostream() override;

	private:
		int _type;
};

}// namespace sf

// Defined types of debug output of which combinations are allowed.
#define DO_DEFAULT sf::dotDefault
#define DO_COUT sf::dotStdOut
#define DO_CLOG sf::dotStdLog
#define DO_CERR sf::dotStdErr
#define DO_MSGBOX sf::dotMessageBox
#define DO_DBGSTR sf::dotDebugString
#define DO_DBGBRK sf::dotDebugBreak

// Class name and file name separator character
#define SF_CLS_SEP ';'

#if (SF_DEBUG_LEVEL == 1)

	#define SF_NORM_NOTIFY(f, a)                                                                                                                                 \
		{                                                                                                                                                          \
			sf::debug_ostream(f) << a;                                                                                                                               \
		}
// Only Clang and GNU have __PRETTY_FUNCTION__ defined.
	#if defined(__clang__) || defined(__GNUC__)
		#define SF_FUNC_NOTIFY(f, a)                                                                                                                               \
			{                                                                                                                                                        \
				sf::debug_ostream(f) << __PRETTY_FUNCTION__ << ' ' << a;                                                                                               \
			}
	#else
		#define SF_FUNC_NOTIFY(f, a)                                                                                                                               \
			{                                                                                                                                                        \
				sf::debug_ostream(f) << __FUNCTION__ << "() " << a;                                                                                                    \
			}
	#endif
	#define SF_RTTI_NOTIFY(f, a)                                                                                                                                 \
		{                                                                                                                                                          \
			sf::debug_ostream(f) << SF_RTTI_TYPENAME << "::" << __FUNCTION__ << SF_CLS_SEP << " " << a;                                                              \
		}
	#define SF_COND_NORM_NOTIFY(p, f, a)                                                                                                                         \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				SF_NORM_NOTIFY(f, a);                                                                                                                                  \
			}                                                                                                                                                        \
		}
	#define SF_COND_FUNC_NOTIFY(p, f, a)                                                                                                                         \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				SF_FUNC_NOTIFY(f, a);                                                                                                                                  \
			}                                                                                                                                                        \
		}
	#define SF_COND_RTTI_NOTIFY(p, f, a)                                                                                                                         \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				SF_RTTI_NOTIFY(f, a);                                                                                                                                  \
			}                                                                                                                                                        \
		}
	#if IS_QT
		#define SF_Q_NOTIFY(a)                                                                                                                                     \
			{                                                                                                                                                        \
				qDebug() << a;                                                                                                                                         \
			}
	#endif

#elif (SF_DEBUG_LEVEL == 2)

	#define _NORM_NOTIFY(f, a)                                                                                                                                   \
		{                                                                                                                                                          \
			sf::debug_ostream(f) << __FUNCTION__ << ' ' << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;                                                      \
		}
// Only Clang and GNU have __PRETTY_FUNCTION__ defined.
	#if defined(__clang__) || defined(__GNUC__)
		#define SF_FUNC_NOTIFY(f, a)                                                                                                                               \
			{                                                                                                                                                        \
				sf::debug_ostream(f) << __PRETTY_FUNCTION__ << ' ' << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;                                             \
			}
	#else
		#define SF_FUNC_NOTIFY(f, a)                                                                                                                               \
			{                                                                                                                                                        \
				sf::debug_ostream(f) << __FUNCTION__ << "() " << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;                                                  \
			}
	#endif
	#define _RTTI_NOTIFY(f, a)                                                                                                                                   \
		{                                                                                                                                                          \
			sf::debug_ostream(f) << _RTTI_TYPENAME << "::" << __FUNCTION__ << ' ' << SF_CLS_SEP << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;              \
		}
	#define _COND_NORM_NOTIFY(p, f, a)                                                                                                                           \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				_NORM_NOTIFY(f, a);                                                                                                                                    \
			}                                                                                                                                                        \
		}
	#define _COND_FUNC_NOTIFY(p, f, a)                                                                                                                           \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				_FUNC_NOTIFY(f, a);                                                                                                                                    \
			}                                                                                                                                                        \
		}
	#define _COND_RTTI_NOTIFY(p, f, a)                                                                                                                           \
		{                                                                                                                                                          \
			if (p)                                                                                                                                                   \
			{                                                                                                                                                        \
				_RTTI_NOTIFY(f, a);                                                                                                                                    \
			}                                                                                                                                                        \
		}
	#if IS_QT
		#define SF_Q_NOTIFY(a)                                                                                                                                     \
			{                                                                                                                                                        \
				qDebug() << a;                                                                                                                                         \
			}// NOLINT(bugprone-macro-parentheses)
	#endif

#else// _DEBUG_LEVEL == 0

	#define _NORM_NOTIFY(f, a)                                                                                                                                   \
		{                                                                                                                                                          \
		}
	#define _FUNC_NOTIFY(f, a)                                                                                                                                   \
		{                                                                                                                                                          \
		}
	#define _RTTI_NOTIFY(f, a)                                                                                                                                   \
		{                                                                                                                                                          \
		}
	#define _COND_NOTIFY(p, f, a)                                                                                                                                \
		{                                                                                                                                                          \
		}
	#define _COND_FUNC_NOTIFY(p, f, a)                                                                                                                           \
		{                                                                                                                                                          \
		}
	#define _COND_RTTI_NOTIFY(p, f, a)                                                                                                                           \
		{                                                                                                                                                          \
		}
	#if IS_QT
		#define SF_Q_NOTIFY(a)                                                                                                                                     \
			{                                                                                                                                                        \
			}
	#endif

#endif//DEBUG_LEVEL
