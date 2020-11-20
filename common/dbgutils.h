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
	_NOTIFY(flags, text)
	_COND_NOTIFY(cond, flags, text)
	_CLASS_NOTIFY(flags, text)
	_COND_CLASS_NOTIFY(cond, flags, text)

	Where 'flags' can be one or a combination of the following flags:
	----------------------------------------------------------------
	DO_DEFAULT  Predefined combination of flags.
	DO_THROW    Throw an exception.
	DO_MSGBOX   Notify through a message box.
	DO_CLOG     Notify through the clog output stream.
	DO_COUT     Notify through the cout output stream.
	DO_CERR     Notify through the cerr output stream.
	DO_DBGSTR   Notify through function OutputDebugString.
	DO_DBGBRK   Call debug break by raising SIGTRAP.

	Remarks
	-------
	CLASS_?? notifications use the classes NameOf() function.
*/

#ifndef MISC_DBGUTILS_H
#define MISC_DBGUTILS_H

#include "defs.h"
// Include for cout, cerr ,clog and ostream.
#include <iostream>

// Forward definition.
class debug_ostream;

// This define should be defined externaly in the project or make file.
#ifndef _DEBUG_LEVEL
#  define _DEBUG_LEVEL 1
#endif

// Define that converts a this pointer to the class type name.
#define _RTTI_TYPENAME misc::Demangle(typeid(*this).name())
// Coverts the passed type into a name.
#define _RTTI_NAME(this) misc::Demangle(typeid(this).name())
// Only the filename part of __FILE__.
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// Equals end of page character 12.
#define END_OF_MSG ('\f')


namespace misc {

//
enum EDebugOutputType
{
	dotTHROW =   1<<0,  // throw and exception.
	dotMSGBOX =  1<<1,  // Notify through a message box.
	dotCLOG =    1<<2,  // Notify through the clog output stream.
	dotCOUT =    1<<3,  // Notify through the cout output stream.
	dotCERR =    1<<4,  // Notify through the cerr output stream.
	dotDBGSTR =  1<<5,  // Notify through function OutputDebugString.
	dotDBGBRK =  1<<6,  // Call dbug break function.
	dotDEFAULT = 1<<7   // Call dbug break function.
};

// Allows the passed string to be presented in various ways.
_MISC_FUNC void _MISC_CALL UserOutputDebugString(int type, const char* s);
// Set and get function for the type of output when DO_DEFAULT is passed.
_MISC_FUNC void _MISC_CALL SetDefaultDebugOutput(int type);
_MISC_FUNC int _MISC_CALL GetDefaultDebugOutput();
// Returns true if debuging is passed at the command line.
_MISC_FUNC bool _MISC_CALL IsDebug();
_MISC_FUNC std::string _MISC_CALL Demangle(const char* name);


class debug_ostream
	: public std::ostream
	, protected std::streambuf
{
	public:
		// Constructor
		debug_ostream(int type);
		// Destructor.
		~debug_ostream();
		// Executes what is streamed and set.
		void execute();

	private:
		std::string msg;
		int Type;
		//
		virtual int overflow(int c=EOF);
//		std::streamsize xsputn(const char *s, streamsize count);
		std::size_t do_sputn(const char *s, std::size_t count);
};

} // namespace misc

// Defined types of debug output of which combinations are alolowed.
#define DO_DEFAULT misc::dotDEFAULT
#define DO_COUT    misc::dotCOUT
#define DO_CLOG    misc::dotCLOG
#define DO_CERR    misc::dotCERR
#define DO_MSGBOX  misc::dotMSGBOX
#define DO_THROW   misc::dotTHROW
#define DO_DBGSTR  misc::dotDBGSTR
#define DO_DBGBRK  misc::dotDBGBRK

// Class name and file name separator character
#define _CLS_SEP '|'

// None conditional throw defines.
#define _NORM_THROW(a) {misc::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _CLASS_THROW(a) {misc::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << NameOf() << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _RTTI_THROW(a) {misc::debug_ostream dbg_os(dotTHROW|dotDBGSTR); dbg_os << _RTTI_TYPENAME << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '\x1C' << '\t' << a;}
#define _COND_NORM_THROW(p, a) {if (p) _NORM_THROW(a);}
#define _COND_CLASS_THROW(p, a) {if (p) _CLASS_THROW(a);}
#define _COND_RTTI_THROW(p, a) {if (p) _RTTI_THROW(a);}

#if (_DEBUG_LEVEL == 1)

#define _NORM_NOTIFY(f, a) {misc::debug_ostream dbg_os(f); dbg_os  << a;}
#define _CLASS_NOTIFY(f, a) {misc::debug_ostream dbg_os(f); dbg_os << NameOf() << _CLS_SEP << a;}
#define _RTTI_NOTIFY(f, a) {misc::debug_ostream dbg_os(f); dbg_os << _RTTI_TYPENAME << _CLS_SEP << a;}
#define _COND_NORM_NOTIFY(p, f, a) {if (p) {_NORM_NOTIFY(f, a);}}
#define _COND_CLASS_NOTIFY(p, f, a) {if (p) {_CLASS_NOTIFY(f, a);}}
#define _COND_RTTI_NOTIFY(p, f, a) {if (p) {_RTTI_NOTIFY(f, a);}}

#elif (_DEBUG_LEVEL == 2)

#define _NORM_NOTIFY(f, a) {misc:: dbg_os(f); dbg_os << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
#define _CLASS_NOTIFY(f, a) {misc:: dbg_os(f); dbg_os << NameOf() << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
#define _RTTI_NOTIFY(f, a) {misc:: dbg_os(f); dbg_os << _RTTI_TYPENAME << _CLS_SEP << __FILENAME__ << ':' << __LINE__ << '@' << '\t' << a;}
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

#endif // MISC_DBGUTILS_H
