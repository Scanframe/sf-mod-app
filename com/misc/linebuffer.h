
#ifndef MISC_LINEBUFFER_H
#define MISC_LINEBUFFER_H

#include <iostream>
#include "fifoclass.h"
#include "closure.h"
#include "global.h"

namespace sf
{

/**
 *  Special std::streambuf class.
 */
class _MISC_CLASS LineBuffer : public std::streambuf
{
	public:
		// Constructor settings the maximum lines the buffer can hold.
		explicit LineBuffer(unsigned max_lines);
		// Virtual destructor.
		~LineBuffer() override;

		// Returns requested line in the buffer.
		std::string GetLine(unsigned line) const;

		// Returns the amount of lines in the buffer.
		int GetLineCount() const;

		// Returns the total lines passed through the fifo buffer.
		int GetTotalLineCount() const {return FTotalLineCount;}

		// Clears the current line buffer.
		void Clear();

		// Sets the maximum lines the stream can hold. Flushes all lines first.
		void SetMaxLines(unsigned max_lines);

		//typedef void (* NewLineEvent)(LineBuffer* sender, const std::string& line);
		// Event type.
		typedef Closure<void, LineBuffer* /*sender*/, const std::string& /*line*/> EvNewLine;

		// When a new line is added this event is triggered.
		void setNewLineHandler(const EvNewLine& event);
		//
		EvNewLine& newLineHandler();

	private:
		//  Holds the total count of lines passed in the fifo buffer.
		int FTotalLineCount;
		//
		// Virtual overloaded functions of the 'std::streambuf' class.
		//
		int overflow(int c/*=EOF*/) override;
		std::streamsize xsputn(const char* s, std::streamsize count) override;

		// Holds the pointers to the string strings
		TFifoClass<std::string*> FLineFifo;
		// Holds the handler.
		EvNewLine FOnNewLine;
};

} // namespace sf

#endif // MISC_LINEBUFFER_H