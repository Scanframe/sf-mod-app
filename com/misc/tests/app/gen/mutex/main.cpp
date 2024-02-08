#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <condition_variable>
#include <misc/gen/Mutex.h>
#include <misc/gen/Condition.h>
#include <misc/gen/Thread.h>

/* compile with gcc -pthread lockwait.c */

pthread_t* thread_data{nullptr};
pthread_cond_t cv;
pthread_mutex_t raw_mutex;

void* thread_raw(void* v)
{
	int error = 0;
	printf("Locking and waiting. Type 'u' to unlock me.\n");
	error = pthread_mutex_lock(&raw_mutex);
	if (error)
	{
		perror(__FUNCTION__ );
	}
	error = pthread_cond_wait(&cv, &raw_mutex);
	if (error)
	{
		perror(__FUNCTION__ );
	}
	printf("I've been unlocked.\n");
	error = pthread_mutex_unlock(&raw_mutex);
	if (error)
	{
		perror(__FUNCTION__ );
	}
	return nullptr;
}

void create_or_signal(bool create)
{
	if (create)
	{
		pthread_create(thread_data, nullptr, thread_raw, nullptr);
	}
	else
	{
		pthread_cond_signal(&cv);
	}
}

sf::Condition sf_condition; // NOLINT(cert-err58-cpp)
sf::Mutex sf_mutex; // NOLINT(cert-err58-cpp)

void* sf_thread(void* v)
{
	printf("SF Locking and waiting. Type 'u' to unlock me.\n");
	sf::Mutex::Lock lock(sf_mutex);
	sf_condition.wait(sf_mutex);
	printf("SF I've been unlocked.\n");
	//lock.release();
	return nullptr;
}

sf::ThreadClosure sf_thread_closure([](sf::Thread& thread) -> int // NOLINT(cert-err58-cpp)
{
	sf_thread(nullptr);
	return 0xff;
});

void sf_create_or_signal(bool create)
{
	if (create)
	{
		//pthread_create(thread_data, nullptr, sf_thread, nullptr);
		sf_thread_closure.start();
	}
	else
	{
		sf_condition.notifyOne(sf_mutex);
	}
}

int main_()
{
	char cmd[1024];
	thread_data = (pthread_t*) malloc(sizeof(pthread_t));
	printf("Type 'l' to run a thread that locks and waits.\n");
	printf("Type 'u' to unlock the same thread.\n");
	while (fscanf(stdin, "%s", cmd) != EOF)
	{
		if (strcmp(cmd, "l") == 0)
		{
			sf_create_or_signal(true);
		}
		else if (strcmp(cmd, "u") == 0)
		{
			sf_create_or_signal(false);
		}
		else if (strcmp(cmd, "x") == 0)
		{
			break;
		}
	}
	sf::free_null(thread_data);
	return 0;
}


int main()
{
	sf::Mutex mutex;
	sf::Condition condition;
	sf::ThreadClosure tc(sf::ThreadClosure::func_type([&](sf::Thread& thread) -> int
	{
		sf::Mutex::Lock lock(mutex);
		condition.wait(mutex);
		SF_NORM_NOTIFY(DO_CLOG, "Thread unlocked");
		return 0xff;
	}));
	//
	//tc.setDebug(true);
	// Start the thread.
	tc.start();
	//
	if (tc.getStatus() != sf::Thread::tsRunning)
	{
		// Give thread some time.
		::usleep(20000);
		SF_NORM_NOTIFY(DO_CLOG, "Main thread waiting for Running state.");
	}
	// Signal the locked thread.
	condition.notifyOne(mutex);
	//
	if (tc.getStatus() != sf::Thread::tsFinished)
	{
		// Give thread some time.
		::usleep(20000);
		SF_NORM_NOTIFY(DO_CLOG, "Main thread waiting for Finished state.");
	}
	//
	tc.terminateAndWait();
	// Check the exit.
	if (tc.getExitCode() != 0xff)
	{
		SF_NORM_NOTIFY(DO_CLOG, "Exit code not expected!");
	}
	return 0;
}


