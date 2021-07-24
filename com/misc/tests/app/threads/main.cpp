#include <iostream>
#include <misc/gen/gen_utils.h>
#include <misc/gen/dbgutils.h>
#include <misc/gen/ElapseTimer.h>
#include <misc/gen/Thread.h>
#include <csignal>
#include <thread>
#include <chrono>

void TestNumberString()
{
	std::clog << sf::numberString(double(-0.99999), 2) << std::endl;
}

void TestStdSignal()
{
	constexpr int quit_signal = SIGINT;
	static thread_local volatile std::sig_atomic_t quit = false;
#if !IS_WIN
	// enforce POSIX semantics
	siginterrupt(quit_signal, true);
#endif
	// register signal handler
	std::signal(quit_signal, [](int)
	{
		quit = true;
	});

	auto t = std::thread([]()
	{
		char buf[10];
		while (!quit)
		{
			if (sleep(5))
			// /*if */(std::this_thread::sleep_for(std::chrono::seconds(10)));
			{
				if (errno == EINTR)
				{
					std::clog << "Sleep was interrupted due to a signal.\n";
					continue;
				}
			}
		}
		std::clog << "quit is true. Exiting\n";
	});

	// wait for a while and let the child thread initiate read
	sleep(1);

	// send signal to thread
	pthread_kill(t.native_handle(), quit_signal);

	t.join();
}

void TestStdThreads()
{
	class Foo
	{
		public:
			void bar()
			{
				for (int i = 0; i < 5; ++i)
				{
					std::clog << "Thread 3 executing\n";
					++n;
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}

			int n = 0;
	};
	Foo f;
	std::thread t5(&Foo::bar, &f);
	t5.join();
	std::clog << "Final value of f.n (foo::n) is " << f.n << '\n';
}

void TestThreads(int how = 0)
{
	struct Scope
	{
		~Scope()
		{
			SF_RTTI_NOTIFY(DO_DEFAULT, "Out of scope... " << sf::Thread::getCurrentId())
		}

	};
	sf::ThreadClosure tc(sf::ThreadClosure::func_type([how](sf::Thread& thread) -> int
	{
		Scope scope;
		switch (how)
		{
			case 1:
			{
				auto ch = std::cin.get();
				SF_NORM_NOTIFY(DO_DEFAULT, "Test: Woken up cin.get() > '" << std::hex << ch
				<< "' Fail(" << std::boolalpha << std::cin.fail() << ") " << std::dec << sf::Thread::getCurrentId())
				break;
			}
			default:
			{
				do
				{
					// Sleep for some time to get interrupted by a call to terminateAndWait().
					if (thread.sleep(sf::TimeSpec(10, 0), true))
					{
						SF_NORM_NOTIFY(DO_DEFAULT, "Test: Woken up normal:" << sf::Thread::getCurrentId())
					}
					else
					{
						SF_NORM_NOTIFY(DO_DEFAULT, "Test: Woken up by interruption: " << sf::Thread::getCurrentId())
					}
				}
				while (!thread.shouldTerminate());
				break;
			}
		}
		return 0;
	}));
	//
	tc.setDebug(true);
	//
	SF_NORM_NOTIFY(DO_DEFAULT, __FUNCTION__ << ": Before starting the thread ...");
	tc.start();
	SF_NORM_NOTIFY(DO_DEFAULT, __FUNCTION__ << ": After starting the thread ...");
	::usleep(100000);
	SF_NORM_NOTIFY(DO_DEFAULT, __FUNCTION__ << ": Terminate the thread and wait...");
	tc.terminateAndWait();
	SF_NORM_NOTIFY(DO_DEFAULT, __FUNCTION__ << ": Thread has ended with (" << tc.getExitCode() << ")");
}

int main(/*int argc, char* argv[]*/)
{
	TestNumberString();
#if !IS_WIN
	TestThreads(1);
	TestThreads(0);
	TestStdThreads();
	TestStdSignal();
#else
	TestThreads(0);
	TestStdThreads();
#endif
}
