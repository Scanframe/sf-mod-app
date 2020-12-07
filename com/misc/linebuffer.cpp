#include "linebuffer.h"
#include "dbgutils.h"

namespace sf
{

LineBuffer::LineBuffer(unsigned max_lines)
	: std::streambuf()
	, FTotalLineCount(0)
	, FLineFifo(int(max_lines))
	, FOnNewLine(nullptr)
{
}

LineBuffer::~LineBuffer()
{
	if (FLineFifo.IsValid())
	{
		Clear();
	}
}

void LineBuffer::Clear()
{
	// Delete all string instances in the fifo
	std::string* item = nullptr;
	while (FLineFifo.Get(item))
	{
		delete item;
	}
}

// Sets the maximum lines the stream can hold. Flushes all lines first.
void LineBuffer::SetMaxLines(unsigned max_lines)
{
	Clear();
	FLineFifo.Set(int(max_lines));
}

std::string LineBuffer::GetLine(unsigned line) const
{
	std::string retval;
	if (line < (unsigned) GetLineCount())
	{
		std::string* s = FLineFifo[int(line)];
		retval = *s;
	}
	return retval;
}

int LineBuffer::GetLineCount() const
{
	return FLineFifo.Size();
}

int LineBuffer::overflow(int c)
{
	if (c == '\x12')
	{
		Clear();
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
	std::string* latest = FLineFifo.Latest();
	std::string* cur;
	// Put new string instance in the fifo if it is empty.
	if (!latest)
	{
		FLineFifo.Put(latest = new std::string());
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
				FTotalLineCount++;
				// remove one line if Fifo is full
				if (FLineFifo.IsFull())
				{
					latest = FLineFifo.Get();
					*latest = "";
					FLineFifo.Put(latest);
				}
				else // Add a new line to the fifo
				{
					latest = new std::string();
					FLineFifo.Put(latest);
				}
				// Call handler on new line
				if (FOnNewLine.isAssigned())
				{
					FOnNewLine(this, *cur);
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

void LineBuffer::setNewLineHandler(const LineBuffer::EvNewLine& event)
{
	FOnNewLine = event;
}

LineBuffer::EvNewLine& LineBuffer::newLineHandler()
{
	return FOnNewLine;
}

} // namespace sf
