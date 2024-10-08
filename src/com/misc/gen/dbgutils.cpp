#include "target.h"

#if IS_GNU
	#include <cxxabi.h>
#endif

#include <array>
#include <csignal>
#include <cstring>
#include <fstream>
#include <mutex>
#include <regex>
#include <string>

#if IS_QT
	#include "qt/qt_utils.h"
#endif

#if IS_WIN
	#include <debugapi.h>
	#include <processenv.h>
#else
	#include <execinfo.h>
#endif

#include "TimeSpec.h"
#include "dbgutils.h"
#include "system.h"

namespace sf
{

namespace
{
std::mutex DbgMutex;
}

bool isDebuggerActive()
{
#if IS_WIN
	return IsDebuggerPresent();
#else
	static int dbg_attached = -1;
	if (-1 == dbg_attached)
	{
		dbg_attached = 1;
		// Call lambda function to handle.
		signal(SIGTRAP, [](int) {
			dbg_attached = 0;
			signal(SIGTRAP, SIG_DFL);
		});
	}
	return dbg_attached > 0;
#endif
}

void debugBreak()
{
	if (isDebuggerActive())
	{
#if IS_WIN
		::DebugBreak();
#else
		// Raise signal SIGTRAP when debugger present otherwise the process crashes.
		raise(SIGTRAP);
#endif
	}
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
	// When default is specified set the bit.
	if (type & dotDefault)
	{
		type |= DefaultDebugOutputType;
	}
	// If the debug string bit is enabled call 'OutputDebugString'.
	if (type & dotDebugString)
	{
		//::OutputDebugString((std::string(s) + "\n").c_str());
	}
	TimeSpec tm(getTimeRunning());
	// If the log bit is enabled write the line as is.
	if (type & dotStdLog)
	{
		std::clog << tm.toString() << ' ' << s << std::endl;
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
		std::cerr << tm.toString() << ' ' << s << std::endl;
	}
	if (type & dotStdOut)
	{
		std::cout << tm.toString() << ' ' << s << std::endl;
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
				(void) sentry;
				// Release lock to prevent lockup multiple threads.
				DbgMutex.unlock();
#if IS_QT
				// When not GUI application has been started skip this test.
				if (sf::isGuiApplication())
				{
					QMessageBox(QMessageBox::Warning, caption, text).exec();
				}
#endif
				sentry = false;
				(void) sentry;
			}
		}
	}
	// If the debug break bit is enabled call.
	if (type & dotDebugBreak)
	{
		// Raise signal SIGTRAP when debugger present otherwise the process crashes in Linux.
		debugBreak();
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

std::string executeShellCommand(const std::string& cmd)
{
	std::string result;
#if !IS_WIN
	std::array<char, 1024> buffer;
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
	// Check if the pipe was opened successfully.
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (!feof(pipe.get()))
	{
		if (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		{
			result.append(buffer.data());
		}
	}
#endif
	return result;
}

void printBacktrace()
{
#if !IS_WIN
	void* bt[128];
	int bt_size;
	char** bt_syms;
	int i;
	bt_size = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
	bt_syms = backtrace_symbols(bt, bt_size);
	std::regex re("^(.*)\\((.*)(\\+0x[0-9a-f]*)\\)\\s\\[(0x[0-9a-f]*)\\]$");
	std::regex re2("\\n$");
	for (i = 1; i < bt_size; i++)
	{
		std::string sym = bt_syms[i];
		std::smatch ms;
		if (std::regex_search(sym, ms, re))
		{
			auto r = executeShellCommand("addr2line -e " + ms[1].str() + " -Cifpsa " + ms[3].str());
			auto r2 = std::regex_replace(r, re2, "");
			std::cerr << r2 << std::endl;
		}
	}
	free(bt_syms);
#endif
}

bool isDebug()
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
			//break;
	}
}

#endif

}// namespace sf
