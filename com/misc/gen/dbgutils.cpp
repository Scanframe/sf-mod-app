/*
Contains debugging macro's that are defined when define DEBUG_LEVEL is non zero.
*/

#include "target.h"
#include <ctime>

#if IS_GNU
#include <cxxabi.h>
#endif

#include <csignal>
#include <string>
#include <fstream>
#include <cstring>
#include <mutex>

#if IS_QT
#include <QApplication>
#include <QException>
#include <QIcon>
#include <QMessageBox>
#endif

#if IS_WIN
#include <debugapi.h>
#include <processenv.h>
#endif

#include "Mutex.h"
#include "dbgutils.h"

namespace sf
{

static std::mutex DbgMutex;

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

static unsigned int DefaultDebugOutputType = dotStdLog;

debug_ostream::~debug_ostream()
{
	if (str().length())
	{
		UserOutputDebugString(_type, str().c_str());
	}
}

void UserOutputDebugString(unsigned int type, const char* s) noexcept
{
	// Critical section to prevent simultaneous modification of the Fifo.
	std::lock_guard<std::mutex> lock(DbgMutex);
	// When default is specified Set the the bit.
	if (type & dotDefault)
	{
		type |= DefaultDebugOutputType;
	}
	// If the debug string bit is enabled call 'OutputDebugString'.
	if (type & dotDebugString)
	{
		//::OutputDebugString((std::string(s) + "\n").c_str());
	}
	auto tm = double(clock()) / (CLOCKS_PER_SEC);
#if !IS_WIN
	// Value 'CLOCKS_PER_SEC' Seems not to be correct in Linux some how.
	tm /= 10;
#endif
	// If the log bit is enabled write the line as is.
	if (type & dotStdLog)
	{
		auto old = std::clog.precision(3);
		std::clog << std::fixed << tm << ' ' << s << std::endl;
		std::clog.precision(old);
	}
	// Find the file separator character '\x1C'.
	char* sep = std::strchr(const_cast<char*>(s), '\x1C');
	// make it a newline character when found.
	if (sep)
	{
		*sep = '\n';
	}
	// Execute functions according the bits.
	if (type & dotStdErr)
	{
		std::cerr << tm << ' ' << s << std::endl;
	}
	if (type & dotStdOut)
	{
		std::cout << tm << ' ' << s << std::endl;
	}
	if (type & dotMessageBox || type & dotThrow)
	{
		const char* caption = "Notification";
		// Find caption separator in text
		char* text = strchr(const_cast<char*>(s), SF_CLS_SEP);
		// When found Set caption pointer
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
		if (type & dotMessageBox)
		{
			static bool sentry = false;
			// Prevent reentry so message boxes are not created in the background.
			if (!sentry)
			{
				sentry = true;
				// Release lock to prevent lockup multiple threads.
				DbgMutex.unlock();
#if IS_QT
				if (QApplication::instance())
				{
					QMessageBox(QMessageBox::Warning, caption, text).exec();
				}
#endif
				sentry = false;
			}
		}
	}
	// If the debug break bit is enabled call.
	if (type & dotDebugBreak)
	{
		// Raise signal SIGTRAP when debugger present otherwise the process crashes in Linux.
		DebugBreak();
	}
}

void SetDefaultDebugOutput(unsigned int type)
{
	// Prevent throw from being Set.
	DefaultDebugOutputType = type & unsigned(~dotThrow);
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
#if IS_WIN
		flag = 0;
		std::string cmd_line(::GetCommandLineA());
		if (cmd_line.find("--debug") != std::string::npos)
		{
			flag = 1;
		}
#else
		flag = 0;
		std::ifstream cls("/proc/self/cmdline");
		std::string cmd_line((std::istreambuf_iterator<char>(cls)), std::istreambuf_iterator<char>());
		if (cmd_line.find("--debug") != std::string::npos)
		{
			flag = 1;
		}
#endif
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

		case QtDebugMsg: // NOLINT(bugprone-branch-clone)
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
			//break;
	}
}

#endif

} // namespace sf
