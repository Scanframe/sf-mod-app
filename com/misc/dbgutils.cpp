/*
Contains debugging macro's that are defined when define DEBUG_LEVEL is non zero.
*/

#include "dbgutils.h"

#include <csignal>

#if IS_GNU
#include <cxxabi.h>
#endif

#include <string>
#include <fstream>

#if IS_QT
#include <QException>
#include <QIcon>
#include <QMessageBox>
#endif

#if IS_WIN
#include <debugapi.h>
#endif

//#define __DBG_MT_SAFEGUARD

#ifdef __DBG_MT_SAFEGUARD
namespace {
static sf::TCriticalSection CriticalSection;
}
#endif

namespace sf
{
// Raise signal SIGTRAP when debugger present otherwise the process crashes.
void DebugBreak()
{
#if IS_WIN
	if (IsDebuggerPresent())
	{
	 ::DebugBreak();
	}
#else
	static int dbg_attached = -1;
	if (-1 == dbg_attached)
	{
		dbg_attached = 1;
		// Call lambda function to handle.
		signal(SIGTRAP, [](int)
		{
			dbg_attached = 0;
			signal(SIGTRAP, SIG_DFL);
		});
	}
	if (dbg_attached > 0)
	{
		raise(SIGTRAP);
	}
#endif
}

static unsigned int DefaultDebugOutputType = dotCLOG;

class notify_exception :public std::exception
{
	public:

		explicit notify_exception(const char* s) :msg(s) {}

		const char* what() const noexcept override;

	private:
		std::string msg;
};

const char* notify_exception::what() const noexcept
{
	return msg.c_str();
}

// Debug output stream for easy streaming of information.
debug_ostream::debug_ostream(int type)
	:std::ostream(std::cout.rdbuf())
	 , std::streambuf()
	 , Type(type)
{
	// Assign the buffer again because some times it is not enough
	this->rdbuf(this);
}

debug_ostream::~debug_ostream()
{
	// When the message was not output yet do it now.
	if (msg.length())
	{
		UserOutputDebugString(Type, msg.c_str());
	}
}

std::size_t debug_ostream::do_sputn(const char* s, std::size_t count)
{
	msg.append(s, 0, static_cast<std::string::size_type>(count));
	return count;
}

void debug_ostream::execute()
{
	UserOutputDebugString(Type, msg.c_str());
	// Reset the buffer for another message
	msg.resize(0);
}

int debug_ostream::overflow(int c)
{
	if (c != EOF)
	{
		char tc[2];
		tc[0] = static_cast<char>(c);
		tc[1] = 0;
		if (c != END_OF_MSG)
		{
			do_sputn(tc, 1);
		}
		else
		{
			execute();
		}
	}
	return 1;
}

void UserOutputDebugString(unsigned int type, const char* s)
{
#ifdef __DBG_MT_SAVEGUARD
	// Critical section to prevent simultaneous modification of the Fifo.
	sf::TCriticalSection::TLock lock(CriticalSection);
#endif
	// When default is specified set the the bit.
	if (type & dotDEFAULT)
	{
		type |= DefaultDebugOutputType;
	}
	// If the debug string bit is enabled call 'OutputDebugString'.
	if (type & dotDBGSTR)
	{
		//::OutputDebugString((std::string(s) + "\n").c_str());
	}
	auto tm = double(clock()) / (CLOCKS_PER_SEC);
#if !IS_WIN
	// Value 'CLOCKS_PER_SEC' Seems not to be correct in Linux some how.
	tm /= 10;
#endif
	// If the log bit is enabled write the line as is.
	if (type & dotCLOG)
	{
		std::clog.precision(3);
		std::clog << std::fixed << tm << ' ' << s << '\n';
	}
	// Find the file separator character '\x1C'.
	char* sep = strchr(const_cast<char*>(s), '\x1C');
	// make it a newline character when found.
	if (sep)
	{
		*sep = '\n';
	}
	// Execute functions according the bits.
	if (type & dotCERR)
	{
		std::cerr << tm << ' ' << s << '\n';
	}
	if (type & dotCOUT)
	{
		std::cout << tm << ' ' << s << '\n';
	}
	if (type & dotMSGBOX || type & dotTHROW)
	{
		const char* caption = "Notification";
		// Find caption separator in text
		char* text = strchr(const_cast<char*>(s), _CLS_SEP);
		// When found set caption pointer
		if (text)
		{
			// Terminate caption string
			*text = '\0';
			// Increment pointer by one to skip '\0' character.
			text++;
			// Set caption pointer.
			caption = s;
		}
		else
		{
			// if no separator was found all buffer text is message text
			text = const_cast<char*>(s);
		}
		//
		if (type & dotMSGBOX)
		{
			static bool sentry = false;
			// Prevent reentry so message boxes are not created in the background.
			if (!sentry)
			{
				sentry = true;
				// Release lock to prevent lockup multiple threads.
#ifdef __DBG_MT_SAVEGUARD
				lock.Release();
#endif
#if IS_QT
				QMessageBox(QMessageBox::Warning, caption, text).exec();
#endif
				sentry = false;
			}
		}
		if (type & dotTHROW)
		{
			throw notify_exception(text);
		}
	}
	// If the debug break bit is enabled call.
	if (type & dotDBGBRK)
	{
		// Raise signal SIGTRAP when debugger present otherwise the process crashes in Linux.
		DebugBreak();
	}
}

void SetDefaultDebugOutput(unsigned int type)
{
	// Prevent throw from being set.
	DefaultDebugOutputType = type & unsigned(~dotTHROW);
}

unsigned int GetDefaultDebugOutput()
{
	return DefaultDebugOutputType;
}

bool IsDebug()
{
	static int flag = -1;
	if (flag < 0)
	{
		std::ifstream cls("/proc/self/cmdline");
		std::string cmd_line((std::istreambuf_iterator<char>(cls)), std::istreambuf_iterator<char>());
		flag = 0;
		if (cmd_line.find(" --debug") != std::string::npos)
		{
			flag = 1;
		}
	}
	return flag > 0;
}

std::string Demangle(const char* name)
{
#if IS_GNU
	int status;
	char* nm = abi::__cxa_demangle(name, nullptr, nullptr, &status);
	std::string rv(nm);
	free(nm);
	return rv;
#else
	return name;
#endif
}

#if IS_QT

// Initialization of the initial handler/sentry.
QtMessageHandler MessageHandler::_initial(nullptr);

void MessageHandler::enable(bool enabled)
{
	if (enabled)
	{
		// Install our handler if it was not installed yet.
		if (!_initial)
		{
			_initial = qInstallMessageHandler(_handler);
		}
	}
	else
	{
		// Restore the initial handler if it was not replaced yet.
		if (_initial)
		{
			qInstallMessageHandler(_initial);
			//
			_initial = nullptr;
		}
	}
}

// Auto install the message handler before the main function.
__attribute__((constructor)) void QtMessageHandlerEnabler()
{
	MessageHandler::enable(true);
}

// Auto uninstall the message handler after the main function.
__attribute__((destructor)) void QtMessageHandlerDisabler()
{
	MessageHandler::enable(false);
}

void MessageHandler::_handler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
	QByteArray text = msg.toLocal8Bit();
	switch (type)
	{
		default:
			_initial(type, ctx, msg);
			break;

		case QtDebugMsg:
			std::clog << text.constData() << std::endl;
			//fprintf(stderr, "Debug: %s \n(%s:%u, %s)\n", text.constData(), ctx.file, ctx.line, ctx.function);
			break;

		case QtInfoMsg:
			std::clog << text.constData() << std::endl;
			//fprintf(stderr, "Info: %s \n(%s:%u, %s)\n", text.constData(), ctx.file, ctx.line, ctx.function);
			break;

		case QtWarningMsg:
			//std::cerr << text.constData() << std::endl;
			fprintf(stderr, "Warning: \n%s (%s:%u, %s)\n", text.constData(), ctx.file, ctx.line, ctx.function);
			break;

		case QtCriticalMsg:
			//std::cerr << text.constData() << std::endl;
			fprintf(stderr, "Critical: \n%s (%s:%u, %s)\n", text.constData(), ctx.file, ctx.line, ctx.function);
			break;

		case QtFatalMsg:
			//std::cerr << text.constData() << std::endl;
			fprintf(stderr, "Fatal: %s \n(%s:%u, %s)\n", text.constData(), ctx.file, ctx.line, ctx.function);
			abort();
			break;
	}
}

#endif

} // namespace sf
