#pragma once

#include "Thread.h"

namespace sf
{
/**
 * @brief Class which combines a thread class and closure template.
 *
 * Allows linking of a lambda function to be called as a thread run function.
 * Usage is like:
 * ```c++
 * ThreadClosure tc(ThreadClosure::func_type([](Thread& thread)->int
 * {
 * 	while (!thread.shouldTerminate());
 * 	{
 * 		if (Thread::sleep(TimeSpec(0.3), true))
 * 		{
 * 			std::clog << "Woken up normal" << std::endl;
 * 		}
 * 		else
 * 		{
 * 			std::clog << "Woken up by interruption" << std::endl;
 * 		}
 * 	}
 * 	return 0;
 * }));
 * tc.start();
 * sf::Thread::sleep(sf::TimeSpec(0.5));
 * tc.terminateAndWait();
 * ```
 */
class ThreadClosure
	: public Thread
	, public TClosure<int, Thread&>
{
	public:
		/**
		 * @brief Default constructor.
		 */
		ThreadClosure(const std::string& name = "")
			: Thread(name)
			, TClosure<int, Thread&>()
		{
		}

		/**
		 * @brief Thread run function assigment constructor.
		 * @param func Function ruin by the thread.
		 */
		explicit ThreadClosure(const func_type& func, const std::string& name = "")
			: Thread(name)
			, TClosure<int, Thread&>(func)
		{
		}

	protected:
		/**
		 * Overrides run function and calls the closure assigned one.
		 */
		int run() override
		{
			return isAssigned() ? call(*this) : 0;
		}
};

}// namespace sf
