#include <iostream>

namespace
{
#if _GLIBCXX_USE_INIT_PRIORITY_ATTRIBUTE

	#pragma message("Fixing 'std::ios' missing initialization for this compiler.")

struct Initialize
{
		Initialize()
		{
			// Check if the stream buffer has been set so
			if (std::cout.rdbuf() == nullptr)
			{
				//static std::ios::Init sf_io_init;
			}
		}
};

// Initialize class using the highest priority allowed (lowest number).
static Initialize __attribute__((init_priority(101))) _initialize_;

#endif
}// namespace
