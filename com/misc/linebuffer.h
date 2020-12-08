
#ifndef MISC_LINEBUFFER_H
#define MISC_LINEBUFFER_H

#include <iostream>
#include "fifoclass.h"
#include "closure.h"
#include "global.h"

namespace sf
{

/**
 *  Special std::streambuf class for capturing lines.
 */
class _MISC_CLASS LineBuffer : public std::streambuf
{
	public:
		/**
		 * Constructor settings the maximum lines the buffer can hold.
		 */
		explicit LineBuffer(unsigned max_lines);
		/**
		 * Overridden virtual destructor.
		 */
		~LineBuffer() override;
		/**
		 * Returns requested line in the buffer.
		 */
		std::string getLine(unsigned line) const;
		/**
		 * Returns the amount of lines in the buffer.
		 */
		int lineCount() const;
		/**
		 * Returns the total lines passed through the fifo buffer.
		 */
		int totalLineCount() const {return _totalLineCount;}
		/**
		 * Clears the current line buffer.
		 */
		void clear();
		/**
		 * Sets the maximum lines the stream can hold. Flushes all lines first.
		 */
		void setMaxLines(unsigned max_lines);
		/**
		 * Closure Event type.
		 */
		typedef Closure<void, LineBuffer* /*sender*/, const std::string& /*line*/> event_t;
		/**
		 * When a new line is added this event is triggered.
		 */
		void setNewLineHandler(const event_t& event);
		/**
		 * returns the new line handler.
		 * @return
		 */
		const event_t& newLineHandler();

	private:
		// Virtual overloaded function of the 'std::streambuf' class.
		int overflow(int c/*=EOF*/) override;
		// Virtual overloaded function of the 'std::streambuf' class.
		std::streamsize xsputn(const char* s, std::streamsize count) override;
		// Holds the pointers to the string strings
		TFifoClass<std::string*> _lineFifo;
		//  Holds the total count of lines passed in the fifo buffer.
		int _totalLineCount;
		// Holds the event handler.
		event_t _onOnNewLine;
};

} // namespace sf

#endif // MISC_LINEBUFFER_H