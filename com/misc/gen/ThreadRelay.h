#pragma once

#include "../global.h"
#include "Sync.h"
#include "Semaphore.h"
#include "Condition.h"
#include "Thread.h"
#include <functional>

namespace sf
{

/**
 * @brief Provides a way to stop executing a thread until the main thread enables the thread again.
 *
 * ```c++
 * std::unique_ptr<ThreadRelay> threadRelay;
 *
 * class Worker
 * {
 *   public:
 *     int method1(int v1);
 * };
 *
 * int Worker::method1(int v1)
 * {
 *   int rv;
 *   // Create the relay instance and use the bool operator to check if the code needs execution.
 *   if (ThreadRelay::Relay1<Worker, decltype(&Worker::method1), int, int>
 *     (*threadRelay, this, &Worker::method1, rv, v1))
 *   {
 *     return Thread::getCurrentId() + v1;
 *   }
 *   return rv;
 * }
 * ```
 */
class _MISC_CLASS ThreadRelay :public Sync
{
	public:

		/**
		 * @brief Default constructor which also sets the thread execution ownership.
		 */
		ThreadRelay();

		/**
		 * @brief Called by main thread for instance. When a lock of this calling thread is requested.
		 * @return Amount of calls made.
		 */
		int checkForWork();

		/**
		 * @brief The calling thread takes thread execution ownership when zero is passed or sets it when not zero.
		 */
		void makeOwner(Thread::id_type threadId = 0);

		/**
		 * @brief Base class for the ThreadRelay::Relay0,1,2,3 templates for storing in a list.
		 */
		class _MISC_CLASS RelayBase
		{
			public:
				/**
				 * @brief Constructor.
				 */
				inline explicit RelayBase(ThreadRelay& tr);

				/**
				 * @brief Default virtual destructor.
				 */
				virtual ~RelayBase() = default;

				/**
				 * @brief Pure virtual function to override in the derived template class to call a method.
				 */
				virtual void call() = 0;

				/**
				 * @brief Blocks the thread when the thread id is different then the thread which created the #ThreadRelay instance.
				 * @return True when acquired.
				 */
				explicit operator bool();

			protected:
				/**
				 * @brief Prevents the execution of the method more the once.
				 */
				std::atomic<bool> _sentry{false};
				/**
				 * @brief Prevents the execution of the method more the once.
				 */
				ThreadRelay& _tr;
				/**
				 * @brief Holds the semaphore to stop the thread until the method is executed.
				 */
				Semaphore _semaphore;

				friend ThreadRelay;
		};

		/**
		 * @brief Template for a method having 0 arguments.
		 */
		template<typename ClassType, typename MethodType, typename Ret>
		class Relay0 :public RelayBase
		{
			public:
				Relay0(ThreadRelay& tr, ClassType* cls, MethodType mtd, Ret& ret)
					:RelayBase(tr), _cls(cls), _mtd(mtd), _ret(ret) {}

			private:
				void call() override
				{
					_ret = ((*_cls).*(Ret(ClassType::*)()) _mtd)();
				}

				ClassType* _cls;
				MethodType _mtd;
				Ret& _ret;
		};

		/**
		 * @brief Template for a method having 1 arguments.
		 */
		template<typename ClassType, typename MethodType, typename Ret, typename Arg1>
		class Relay1 :public RelayBase
		{
			public:
				Relay1(ThreadRelay& tr, ClassType* cls, MethodType mtd, Ret& ret, Arg1& arg1)
					:RelayBase(tr), _cls(cls), _mtd(mtd), _ret(ret), _arg1(arg1) {}

			private:
				void call() override
				{
					_ret = ((*_cls).*(Ret(ClassType::*)(Arg1)) _mtd)(_arg1);
				}

				ClassType* _cls;
				MethodType _mtd;
				Ret& _ret;
				Arg1& _arg1;
		};

		/**
		 * @brief Template for a method having 2 arguments.
		 */
		template<typename ClassType, typename MethodType, typename Ret, typename Arg1, typename Arg2>
		class Relay2 :public RelayBase
		{
			public:
				Relay2(ThreadRelay& tr, ClassType* cls, MethodType mtd, Ret& ret, Arg1& arg1, Arg2& arg2)
					:RelayBase(tr), _cls(cls), _mtd(mtd), _ret(ret), _arg1(arg1), _arg2(arg2) {}

			private:
				void call() override
				{
					_ret = ((*_cls).*(Ret(ClassType::*)(Arg1, Arg2)) _mtd)(_arg1, _arg2);
				}

				ClassType* _cls;
				MethodType _mtd;
				Ret& _ret;
				Arg1& _arg1;
				Arg2& _arg2;
		};

		/**
		 * @brief Template for a method having 3 arguments.
		 */
		template<typename ClassType, typename MethodType, typename Ret, typename Arg1, typename Arg2, typename Arg3>
		class Relay3 :public RelayBase
		{
			public:
				Relay3(ThreadRelay& tr, ClassType* cls, MethodType mtd, Ret& ret, Arg1& arg1, Arg2& arg2, Arg3& arg3)
					:RelayBase(tr), _cls(cls), _mtd(mtd), _ret(ret), _arg1(arg1), _arg2(arg2), _arg3(arg3) {}

			private:
				void call() override
				{
					_ret = ((*_cls).*(Ret(ClassType::*)(Arg1, Arg2, Arg3)) _mtd)(_arg1, _arg2, _arg3);
				}

				ClassType* _cls;
				MethodType _mtd;
				Ret& _ret;
				Arg1& _arg1;
				Arg2& _arg2;
				Arg3& _arg3;
		};

		/**
		 * @brief Template for a method having 4 arguments.
		 */
		template<typename ClassType, typename MethodType, typename Ret, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		class Relay4 :public RelayBase
		{
			public:
				Relay4(ThreadRelay& tr, ClassType* cls, MethodType mtd, Ret& ret, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4)
					:RelayBase(tr), _cls(cls), _mtd(mtd), _ret(ret), _arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4) {}

			private:
				void call() override
				{
					_ret = ((*_cls).*(Ret(ClassType::*)(Arg1, Arg2, Arg3, Arg4)) _mtd)(_arg1, _arg2, _arg3, _arg4);
				}

				ClassType* _cls;
				MethodType _mtd;
				Ret& _ret;
				Arg1& _arg1;
				Arg2& _arg2;
				Arg3& _arg3;
				Arg4& _arg4;
		};

	private:
		// Thread in which the methods are called to be called in.
		Thread::id_type _threadId;
		// List containing the instance waiting to be called.
		std::vector<RelayBase*> _list{};
};

inline
ThreadRelay::RelayBase::RelayBase(ThreadRelay& tr)
	:_tr(tr)
{
}

}
