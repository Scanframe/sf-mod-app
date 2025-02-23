#include <iostream>
#include <unw/Backtrace.h>

void throwing_function(int dummy)
{
	throw std::overflow_error("Overflow error exception");
}

void segfault_function()
{
	// Crash to test backtrace.
	int* p = nullptr;
	// Will cause segmentation fault.
	*p = 42;
}

int main(int argc, char** argv)
{
	try
	{
		throwing_function(123);
	}
	catch (std::exception& ex)
	{
		std::cout << "Caught exception: " << ex.what() << std::endl;
	}
	segfault_function();
	return 0;
}
