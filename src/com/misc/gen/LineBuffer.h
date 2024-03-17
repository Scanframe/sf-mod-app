#pragma once

#include <iostream>

#include "TFifoClass.h"
#include "TClosure.h"
#include "../global.h"

namespace sf
{

/**
 *  @brief Special std::streambuf class for capturing lines.
 */
class _MISC_CLASS LineBuffer : public std::streambuf
{
	public:
		/**
		 * @brief Constructor settings the maximum lines the buffer can hold.
		 */
		explicit LineBuffer(unsigned max_lines);
		/**
		 * @brief Overridden virtual destructor.
		 */
		~LineBuffer() override;
		/**
		 * @brief Returns requested line in the buffer.
		 */
		[[nodiscard]] std::string getLine(unsigned line) const;
		/**
		 * Returns the amount of lines in the buffer.
		 */
		[[nodiscard]] int lineCount() const;
		/**
		 * @brief Returns the total lines passed through the fifo buffer.
		 */
		[[nodiscard]] int totalLineCount() const {return _totalLineCount;}
		/**
		 * Clears the current line buffer.
		 */
		void clear();
		/**
		 * @brief Sets the maximum lines the stream can hold. Flushes all lines first.
		 */
		void setMaxLines(unsigned max_lines);
		/**
		 * @brief Closure Event type.
		 */
		typedef TClosure<void, LineBuffer* /*sender*/, const std::string& /*line*/> event_t;
		/**
		 * @brief When a new line is added this event is triggered.
		 */
		void setNewLineHandler(const event_t& event);
		/**
		 * @brief Returns the new line handler.
		 * @return the Handler.
		 */
		const event_t& newLineHandler();

	private:
		/**
		 * @brief Virtual overloaded function of the 'std::streambuf' class.
		 */
		int overflow(int c/*=EOF*/) override;
		/**
		 * @brief Virtual overloaded function of the 'std::streambuf' class.
		 */
		std::streamsize xsputn(const char* s, std::streamsize count) override;
		/**
		 * @brief Holds the pointers to the string strings.
		 */
		TFifoClass<std::string*> _lineFifo;
		/**
		 * @brief Holds the total count of lines passed in the fifo buffer.
		 */
		int _totalLineCount;
		/**
		 * @brief Holds the event handler.
		 */
		event_t _onOnNewLine;
};

} // namespace

