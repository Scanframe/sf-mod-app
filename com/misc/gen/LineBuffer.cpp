#include "LineBuffer.h"
#include "dbgutils.h"

namespace sf
{

LineBuffer::LineBuffer(unsigned max_lines)
	: std::streambuf()
	, _totalLineCount(0)
	, _lineFifo(int(max_lines))
	, _onOnNewLine(nullptr)
{
}

LineBuffer::~LineBuffer()
{
	if (_lineFifo.IsValid())
	{
		clear();
	}
}

void LineBuffer::clear()
{
	// Delete all string instances in the fifo
	std::string* item;
	while (_lineFifo.Get(item))
	{
		delete item;
	}
}

// Sets the maximum lines the stream can hold. Flushes all lines first.
void LineBuffer::setMaxLines(unsigned max_lines)
{
	clear();
	_lineFifo.Set(int(max_lines));
}

std::string LineBuffer::getLine(unsigned line) const
{
	std::string retval;
	if (line < (unsigned) lineCount())
	{
		std::string* s = _lineFifo[int(line)];
		retval = *s;
	}
	return retval;
}

int LineBuffer::lineCount() const
{
	return _lineFifo.Size();
}

int LineBuffer::overflow(int c)
{
	if (c == '\x12')
	{
		clear();
		return 1;
	}
	char ch[] =
		{
			(char) c, 0
		};
	if (c != EOF)
	{
		xsputn(ch, 1);
	}
	return 1;
}

std::streamsize LineBuffer::xsputn(const char* s, std::streamsize count)
{
	// get latest string inserted in fifo
	std::string* latest = _lineFifo.Latest();
	std::string* cur;
	// Put new string instance in the fifo if it is empty.
	if (!latest)
	{
		_lineFifo.Put(latest = new std::string());
	}
	// TODO: This can be done a lot smarter.
	// add characters
	for (int i = 0; i < count; i++)
	{
		switch (s[i])
		{
			// new line so new string
			case '\n':
				cur = latest;
				_totalLineCount++;
				// remove one line if Fifo is full
				if (_lineFifo.IsFull())
				{
					latest = _lineFifo.Get();
					*latest = "";
					_lineFifo.Put(latest);
				}
				else // Add a new line to the fifo
				{
					latest = new std::string();
					_lineFifo.Put(latest);
				}
				// Call handler on new line
				if (_onOnNewLine.isAssigned())
				{
					_onOnNewLine(this, *cur);
				}
				break;

				// skip this one
			case '\r':
				// remove last character
			case '\b':
				// SKIP
				break;

				// convert tab to some spaces
			case '\t':
				*latest += "  ";
				break;

				// system beep using system speaker
			case '\a':
				//MessageBeep(-1);
				break;

				// add character to string
			default:
				// QString
				latest->append(&s[i], 1);
		}//switch
	}
	return count;
}

void LineBuffer::setNewLineHandler(const LineBuffer::event_t& event)
{
	_onOnNewLine = event;
}

const LineBuffer::event_t& LineBuffer::newLineHandler()
{
	return _onOnNewLine;
}

} // namespace sf
