#include "misc/gen/string.h"
#include <cstdio>
#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <iostream>
#include <misc/gen/dbgutils.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>

void printBacktrace1()
{
	const int maxFrames = 64;// Maximum number of frames to capture
	void* buffer[maxFrames];

	// Capture backtrace
	int numFrames = backtrace(buffer, maxFrames);

	// Convert to symbols
	char** symbols = backtrace_symbols(buffer, numFrames);
	if (symbols == nullptr)
	{
		std::cerr << "Error: Unable to obtain backtrace symbols" << std::endl;
		return;
	}

	// Print stack trace
	std::cout << "Backtrace:" << std::endl;
	for (int i = 0; i < numFrames; ++i)
	{
		std::cout << symbols[i] << std::endl;
	}

	free(symbols);// Free memory allocated by backtrace_symbols
}

void printBacktrace2()
{
	constexpr int maxFrames = 64;
	void* buffer[maxFrames];

	int numFrames = backtrace(buffer, maxFrames);
	char** symbols = backtrace_symbols(buffer, numFrames);

	if (!symbols)
	{
		std::cerr << "Error: Unable to obtain backtrace symbols" << std::endl;
		return;
	}

	std::cout << "Backtrace:\n";
	for (int i = 0; i < numFrames; ++i)
	{
		char* demangled = nullptr;
		char* leftPar = nullptr;
		char* plus = nullptr;

		// Try to find the mangled function name inside the string
		for (char* p = symbols[i]; *p; ++p)
		{
			if (*p == '(') leftPar = p;
			else if (*p == '+')
				plus = p;
		}

		if (leftPar && plus && leftPar < plus)
		{
			*plus = '\0';// Null-terminate before '+'
			int status;
			demangled = abi::__cxa_demangle(leftPar + 1, nullptr, nullptr, &status);
			*plus = '+';// Restore original string
		}

		if (demangled)
		{
			std::cout << symbols[i] << " : " << demangled << "\n";
			free(demangled);
		}
		else
		{
			std::cout << symbols[i] << "\n";
		}
	}
	free(symbols);
}

void printBacktrace3()
{
	constexpr int maxFrames = 64;
	void* buffer[maxFrames];

	int numFrames = backtrace(buffer, maxFrames);
	char** symbols = backtrace_symbols(buffer, numFrames);

	if (!symbols)
	{
		std::cerr << "Error: Unable to obtain backtrace symbols" << std::endl;
		return;
	}

	std::cout << "Backtrace (most recent call first):\n";
	for (int i = 0; i < numFrames; ++i)
	{
		std::cout << symbols[i] << std::endl;

		// Extract address from symbol (format: <executable>(+0xoffset) [0xaddress])
		std::string symbol = symbols[i];
		std::string address;
		size_t start = symbol.find("[0x");
		size_t end = symbol.find("]", start);
		if (start != std::string::npos && end != std::string::npos)
		{
			address = symbol.substr(start + 1, end - start - 1);
		}
		if (!address.empty())
		{
			// Resolve symbol using addr2line
			std::ostringstream cmd;
			cmd << "addr2line -e " << symbols[0] << " " << address;
			std::string command = cmd.str();
			system(command.data());
		}
	}

	free(symbols);
}

void printBacktrace4()
{
	unw_cursor_t cursor;
	unw_context_t context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	std::cout << "Backtrace:\n";
	while (unw_step(&cursor) > 0)
	{
		unw_word_t offset, pc;
		char funcName[256];

		unw_get_reg(&cursor, UNW_REG_IP, &pc);
		if (unw_get_proc_name(&cursor, funcName, sizeof(funcName), &offset) == 0)
		{
			std::cout << funcName << "{" << sf::demangle(funcName) << "} + " << std::hex << offset << " [0x" << pc << "]\n";
		}
		else
		{
			std::cout << "Unknown function at [0x" << pc << "]\n";
		}
	}
}

void print_backtrace()
{
	const int max_frames = 64;
	void* buffer[max_frames];
	std::string name;
	// Capture the backtrace.
	int num_frames = backtrace(buffer, max_frames);
	// Retrieve symbols for the backtrace.
	char** symbols = backtrace_symbols(buffer, num_frames);
	if (symbols == nullptr)
	{
		std::cerr << "Failed to retrieve backtrace symbols." << std::endl;
		return;
	}
	std::cerr << "Backtrace (most recent call first):" << std::endl;
	for (int i = 0; i < num_frames; ++i)
	{
		char* demangled_name = nullptr;
		char* left_paren = nullptr;
		char* plus_sign = nullptr;
		// Try to find the function name in the symbol.
		for (char* p = symbols[i]; *p; ++p)
		{
			if (*p == '(')
			{
				left_paren = p;
			}
			else if (*p == '+')
			{
				plus_sign = p;
			}
		}
		if (left_paren && plus_sign && left_paren < plus_sign)
		{
			// Null-terminate before '+'.
			*plus_sign = '\0';
			int status;
			demangled_name = abi::__cxa_demangle(left_paren + 1, nullptr, nullptr, &status);
			name = sf::demangle(left_paren + 1);
			// Restore the original symbol
			*plus_sign = '+';
		}
		if (demangled_name /*!name.empty()*/)
		{
			std::cerr << symbols[i] << " : " << demangled_name << std::endl;
			free(demangled_name);
			std::cerr << symbols[i] << " : " << name << std::endl;
		}
		else
		{
			// Print the original symbol if demangling fails.
			std::cerr << symbols[i] << std::endl;
		}
	}
	free(symbols);
}

/*
typedef void (*cxa_throw_type)(void*, std::type_info*, void (*)(void*));

// Pointer to the original __cxa_throw function
cxa_throw_type original_cxa_throw = nullptr;

extern "C" void __cxa_throw(void* thrown_exception, void* ti_ptr, void(_GLIBCXX_CDTOR_CALLABI* dest)(void*))
{
	const auto tinfo = static_cast<std::type_info*>(ti_ptr);
	if (!original_cxa_throw)
	{
		// Locate the original __cxa_throw using dlsym.
		original_cxa_throw = (cxa_throw_type) dlsym(RTLD_NEXT, "__cxa_throw");
	}
	std::cerr << "Exception of type '" << (tinfo ? sf::demangle(tinfo->name()) : "unknown") << "' thrown." << std::endl;
	print_backtrace();
	// Call the original __cxa_throw to proceed with normal exception handling
	original_cxa_throw(thrown_exception, tinfo, dest);
	//while (true);
}
*/

void throwing_function(int dummy)
{
	printBacktrace4();
	//print_backtrace();
	//throw std::runtime_error("Exception thrown");
}

int main()
{
	try
	{
		throwing_function(123);
	}
	catch (...)
	{}
}

/*
// Example signal handler to capture crashes
void signalHandler(int signum) {
	std::cerr << "Received signal " << signum << std::endl;
	printBacktrace();
	exit(signum);
}

// Example usage: Register signal handler for segmentation fault
int main() {
	signal(SIGSEGV, signalHandler); // Handle segmentation faults

	// Example crash to test backtrace
	int* p = nullptr;
	*p = 42; // Will cause segmentation fault

	return 0;
}
*/
