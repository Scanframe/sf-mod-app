#include "Backtrace.h"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <iostream>
// Make the unwind include file reference the correct function from the library.
#define UNW_LOCAL_ONLY
#include <dlfcn.h>
#include <libunwind.h>
#include <string.h>

namespace sf::backtrace
{

std::string demangle(const char* name)
{
	std::string rv;
	// Check if not null.
	if (name != nullptr)
	{
		int status;
		char* nm = abi::__cxa_demangle(name, nullptr, nullptr, &status);
		if (status == 0)
		{
			rv.assign(nm);
			free(nm);
		}
		else
		{
			rv = name;
		}
	}
	else
	{
		rv = "<nullptr>";
	}
	return rv;
}

__sighandler_t original_signal_handler{nullptr};

void signal_handler(int signum)
{
	std::cerr << "Signal(" << std::dec << signum << "): " << strsignal(signum) << std::endl;
	print(std::cerr);
	// Call the original signal handler if there was one.
	if (original_signal_handler)
	{
		original_signal_handler(signum);
	}
	exit(signum + 128);
}

void print(std::ostream& os)
{
	unw_cursor_t cursor;
	unw_context_t context;
	unw_getcontext(&context);
	unw_init_local(&cursor, &context);
	os << "Printing Backtrace >>>\n";
	while (unw_step(&cursor) > 0)
	{
		unw_word_t offset, pc;
		char funcName[256];
		unw_get_reg(&cursor, UNW_REG_IP, &pc);
		if (unw_get_proc_name(&cursor, funcName, sizeof(funcName), &offset) == 0)
		{
			os << demangle(funcName) << " + " << std::hex << offset << " [0x" << pc << "]\n";
		}
		else
		{
			os << "Unknown function at [0x" << pc << "]\n";
		}
	}
}

void test_throw()
{
	throw std::runtime_error("Runtime error exception");
}

void initialize()
{
	// Handle segmentation faults.
	original_signal_handler = signal(SIGSEGV, signal_handler);
}

void uninitialize()
{
	//std::cout << "backtrace::" << __FUNCTION__ << "\n";
}

}// namespace sf::backtrace

typedef void (*cxa_throw_type)(void*, std::type_info*, void (*)(void*));

// Pointer to the original __cxa_throw function
cxa_throw_type original_cxa_throw = nullptr;

extern "C" void __cxa_throw(void* thrown_exception, void* ti_ptr, void(_GLIBCXX_CDTOR_CALLABI* dest)(void*))
{
	const auto tinfo = static_cast<std::type_info*>(ti_ptr);
	if (!original_cxa_throw)
	{
		// Locate the original __cxa_throw using dlsym.
		original_cxa_throw = cxa_throw_type(dlsym(RTLD_NEXT, "__cxa_throw"));
	}
	std::cerr << "Exception of type '" << (tinfo ? sf::backtrace::demangle(tinfo->name()) : "unknown") << "' thrown." << std::endl;
	sf::backtrace::print(std::cerr);
	// Call the original __cxa_throw to proceed with normal exception handling.
	original_cxa_throw(thrown_exception, tinfo, dest);
	// To satisfy the compiler not returning.
	while (true);
}
