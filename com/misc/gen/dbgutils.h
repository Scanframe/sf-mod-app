/*
	Contains debuging macro's that are defined when define DEBUG_LEVEL is
	defined as non-zero.

	Introduces macro's:
	===================

	Defines to throw an exception:
	_NORM_THROW(a)
	_CLASS_THROW(a)
	_RTTI_THROW(a)
	_COND_NORM_THROW(p, a)
	_COND_CLASS_THROW(p, a)
	_COND_RTTI_THROW(p, a)

	Defines that change on define _DEBUG_LEVEL:
	_NORM_NOTIFY(flags, text)
	_CLASS_NOTIFY(flags, text)
	_RTTI_NOTIFY(flags, text)
	_COND_NORM_NOTIFY(cond, flags, text)
	_COND_CLASS_NOTIFY(cond, flags, text)
	_COND_RTTI_NOTIFY(cond, flags, text)

	Define that are fixed: (Can be removed when _DEBUG_LEVEL is -1)
	----------------------
	_NORM_NOTIFY(flags, text)
	_COND_NOTIFY(cond, flags, text)
	_CLASS_NOTIFY(flags, text)
	_COND_CLASS_NOTIFY(cond, flags, text)

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

	Remarks
	-------
	CLASS_?? notifications use the classes NameOf() function.
*/

#pragma once

#include "../global.h"
// Include for std::cout, std::cerr ,std::clog and std::ostream.
#include <iostream>
#include <iomanip>
#include <cstring>

#if IS_QT
	#include <QtGlobal>
	#include <QString>
	#include <QDebug>
#endif

// This define should be defined externally in the project or make file.
#ifndef _DEBUG_LEVEL
#  define _DEBUG_LEVEL 1
#endif

// Coverts the passed type into a name.
#define _RTTI_NAME(self) sf::Demangle(typeid(self).name())
// Define that converts a this pointer to the class type name.
#define _RTTI_TYPENAME _RTTI_NAME(*this)
// Only the filename part of __FILE__.
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// Equals end of page character 12.
#define END_OF_MSG ('\f')

#if IS_QT

// Coverts the passed type into a name.
#define _Q_RTTI_NAME(self) QString::fromStdString(_RTTI_NAME(self))
// Define that converts a this pointer to the class type name.
#define _Q_RTTI_TYPENAME _Q_RTTI_NAME(*this)

inline
QDebug& operator<<(QDebug& qd, const std::string& ss)
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

}

#endif

namespace sf
{
//
enum EDebugOutputType
{
	dotTHROW = 1U << 0U,  /** throw and exception.*/
	dotMSGBOX = 1U << 1U,  /** Notify through a message box.*/
	dotCLOG = 1U << 2U,  /** Notify through the clog output stream.*/
	dotCOUT = 1U << 3U,  /** Notify through the cout output stream.*/
	dotCERR = 1U << 4U,  /** Notify through the cerr output stream.*/
	dotDBGSTR = 1U << 5U,  /** Notify through function OutputDebugString.*/
	dotDBGBRK = 1U << 6U,  /** Call debug break function.*/
	dotDEFAULT = 1U << 7U   /** Use default Set notify output type.*/
};

// Allows the passed string to be presented in various ways.
_MISC_FUNC void UserOutputDebugString(unsigned int type, const char* s);

// Set and get function for the type of output when DO_DEFAULT is passed.
_MISC_FUNC void SetDefaultDebugOutput(unsigned int type);

// Returns the combination of EDebugOutputType which is the default output.
_MISC_FUNC unsigned int GetDefaultDebugOutput();

// Returns true if debugging is passed at the command line.
_MISC_FUNC bool IsDebug();

// Demangle the passed rtti type name.
_MISC_FUNC std::string Demangle(const char* name);

// Output stream used as
class _MISC_CLASS debug_ostream
	:public std::ostream, protected std::streambuf
{
	public:
		// Constructor
		explicit debug_ostream(int type);

		// Destructor.
		~debug_ostream() override;

		// Executes what is streamed and Set.
		void execute();

	private:
		std::string msg;
		int Type;

		int overflow(int c) override;

//		std::streamsize xsputn(const char *s, streamsize count);
		std::size_t do_sputn(const char* s, std::size_t count);
};

} // namespace sf

// Defined types of debug output of which combinations are allowed.
#define DO_DEFAULT sf::dotDEFAULT
#define DO_COUT    sf::dotCOUT
#define DO_CLOG    sf::dotCLOG
#define DO_CERR    sf::dotCERR
#define DO_MSGBOX  sf::dotMSGBOX
#define DO_THROW   sf::dotTHROW
#define DO_DBGSTR  sf::dotDBGSTR
#define DO_DBGBRK  sf::dotDBGBRK

// Class name and file name separator character
#define _CLS_SEP '|'

// None conditional throw defines.
#define _NORM_THROW(a) {sf::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _CLASS_THROW(a) {sf::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << NameOf() << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _RTTI_THROW(a) {sf::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << _RTTI_TYPENAME << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _COND_NORM_THROW(p, a) {if (p) _NORM_THROW(a);}
#define _COND_CLASS_THROW(p, a) {if (p) _CLASS_THROW(a);}
#define _COND_RTTI_THROW(p, a) {if (p) _RTTI_THROW(a);}

#if (_DEBUG_LEVEL == 1)

#define _NORM_NOTIFY(f, a) {sf::debug_ostream dbg_os(f); dbg_os  << a;}
#define _CLASS_NOTIFY(f, a) {sf::debug_ostream dbg_os(f); dbg_os << NameOf() << _CLS_SEP << a;}
#define _RTTI_NOTIFY(f, a) {sf::debug_ostream dbg_os(f); dbg_os << _RTTI_TYPENAME << _CLS_SEP << a;}
#define _COND_NORM_NOTIFY(p, f, a) {if (p) {_NORM_NOTIFY(f, a);}}
#define _COND_CLASS_NOTIFY(p, f, a) {if (p) {_CLASS_NOTIFY(f, a);}}
#define _COND_RTTI_NOTIFY(p, f, a) {if (p) {_RTTI_NOTIFY(f, a);}}

#elif (_DEBUG_LEVEL == 2)

#define _NORM_NOTIFY(f, a) {sf:: dbg_os(f); dbg_os << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
#define _CLASS_NOTIFY(f, a) {sf:: dbg_os(f); dbg_os << NameOf() << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
#define _RTTI_NOTIFY(f, a) {sf:: dbg_os(f); dbg_os << _RTTI_TYPENAME << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
#define _COND_NORM_NOTIFY(p, f, a) {if (p) {_NORM_NOTIFY(f, a);}}
#define _COND_CLASS_NOTIFY(p, f, a) {if (p) {_CLASS_NOTIFY(f, a);}}
#define _COND_RTTI_NOTIFY(p, f, a) {if (p) {_RTTI_NOTIFY(f, a);}}

#else // _DEBUG_LEVEL == 0

#define _NORM_NOTIFY(f, a) {}
#define _CLASS_NOTIFY(f, a) {}
#define _RTTI_NOTIFY(f, a) {}
#define _COND_NOTIFY(p, f, a) {}
#define _COND_CLASS_NOTIFY(p, f, a) {}
#define _COND_RTTI_NOTIFY(p, f, a) {}

#endif //DEBUG_LEVEL
