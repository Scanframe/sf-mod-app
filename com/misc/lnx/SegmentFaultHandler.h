#pragma once

#include <csignal>
#include <csetjmp>
#include <vector>
#include <functional>

#include "../global.h"

namespace sf
{

/**
 * @brief Class which intercepts SIGSEGV signals caused in a callback lambda function and recovers from it.
 * Useful in unit testing where a SIGSEGV is expected to happen.
 * @param cb
 */
class _MISC_CLASS SegmentFaultHandler
{
	public:
		/**
		 * Default constructor.
		 */
		explicit SegmentFaultHandler(const std::function<void()>& cb);

		/**
		 * When no SIGSEGV was triggered it returns true.
		 */
		explicit operator bool() const;

		/**
		 * Destructor cleaning up.
		 */
		~SegmentFaultHandler();

	private:
		struct Entry
		{
			jmp_buf _buf{};
			int _counter{0};
		};
		/**
		 * Holds the entry pointer for this instance.
		 */
		Entry* _entry{nullptr};
		/**
		 * Holds the handler before ours was installed.
		 */
		static __sighandler_t _savedHandler;
		/**
		 * Holds the jump buffers and counters.
		 */
		static std::vector<Entry> _buffers;
		/**
		 * Installed handler.
		 * @param cause Cause of calling the handler.
		 */
		static void handler(int cause);
};

}