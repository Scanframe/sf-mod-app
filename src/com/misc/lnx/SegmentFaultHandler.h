#pragma once
#include <csetjmp>
#include <csignal>
#include <functional>
#include <misc/global.h>
#include <vector>

namespace sf
{

/**
 * @brief Class which intercepts SIGSEGV signals caused in a callback lambda function and recovers from it.
 * Useful in unit testing where a SIGSEGV is expected to happen.
 */
class _MISC_CLASS SegmentFaultHandler
{
	public:
		/**
		 * @brief Default constructor.
		 * @param cb Callback function.
		 */
		explicit SegmentFaultHandler(const std::function<void()>& cb);

		/**
		 * @brief Destructor cleaning up.
		 */
		~SegmentFaultHandler();

		/**
		 * @brief When no SIGSEGV was triggered it returns true.
		 */
		explicit operator bool() const;

	private:
		struct Entry
		{
				jmp_buf _buf{};
				int _counter{0};
		};
		/**
		 * @brief Holds the entry pointer for this instance.
		 */
		Entry* _entry{nullptr};
		/**
		 * @brief Holds the handler before ours was installed.
		 */
		static __sighandler_t _savedHandler;
		/**
		 * @brief Holds the jump buffers and counters.
		 */
		static std::vector<Entry> _buffers;
		/**
		 * @brief Installed handler.
		 * @param cause Cause of calling the handler.
		 */
		static void handler(int cause);
};

}// namespace sf
