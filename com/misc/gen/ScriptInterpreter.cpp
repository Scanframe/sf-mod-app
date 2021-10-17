#include "ScriptInterpreter.h"
#include "Exception.h"
#include "Sustain.h"
#include "IniProfile.h"
#if IS_QT
	#include <QApplication>
	#include <QWidget>
#include <utility>
#endif
#include "ScriptInterpreter_p.h"

#define MAX_IDENT_LENGTH 64

namespace sf
{

// Speed index defines
#define SID_COMPILER_START (-10000)
#define SID_VARS          SID_COMPILER_START
#define SID_LABELS       (SID_COMPILER_START - 1)
#define SID_EXIT         (SID_COMPILER_START - 2)
#define SID_PRINT        (SID_COMPILER_START - 3)
#define SID_WRITELOG     (SID_COMPILER_START - 4)
#define SID_BEEP         (SID_COMPILER_START - 5)
#define SID_CLOCK        (SID_COMPILER_START - 6)
#define SID_CLS          (SID_COMPILER_START - 7)
#define SID_SETTIMEOUT   (SID_COMPILER_START - 8)
#define SID_READPROFILE  (SID_COMPILER_START - 9)
#define SID_WRITEPROFILE (SID_COMPILER_START -10)
#define SID_SHUTDOWNSYS  (SID_COMPILER_START -11)
#define SID_EXITPROCESS  (SID_COMPILER_START -12)
#define SID_SHELLEXEC    (SID_COMPILER_START -13)
#define SID_GETENVIRON   (SID_COMPILER_START -14)
#define SID_GETCONFIGDIR (SID_COMPILER_START -15)
#define SID_SPEAK        (SID_COMPILER_START -16)
#define SID_SLEEP        (SID_COMPILER_START -17)
#define SID_RELINQUISH   (SID_COMPILER_START -18)
#define SID_FOCUSWINDOW  (SID_COMPILER_START -19)
#define SID_TRACE        (SID_COMPILER_START -20)
#define SID_GETUSERNAME  (SID_COMPILER_START -21)
#define SID_CREATE       (SID_COMPILER_START -22)
#define SID_TIME         (SID_COMPILER_START -23)
#define SID_DATE         (SID_COMPILER_START -24)

// Timeout value for script when not in stepping mode. 10 seconds for debugging purposes.
//#define SCRIPT_TIMEOUT 10000000
#define SCRIPT_TIMEOUT 1000000000

ScriptInterpreter::ScriptInterpreter()
	:ScriptEngine()
	 , _scriptName("<unknown>")
{
	_command = "";
	_currentInstructionPtr = -1;
	_errorInstructionPtr = -1;
	_mainFunction = -1;
	_prevState = esEmpty;
	_flagSentry = false;
	_flagStepMode = false;
	_loopTimer.set(SCRIPT_TIMEOUT);
}

ScriptInterpreter::~ScriptInterpreter()
{
	// Clear all compiled data from this class and possible derived classes.
	clear();
}

std::string ScriptInterpreter::getScriptName() const
{
	return _scriptName;
}

void ScriptInterpreter::setScriptName(const std::string& name)
{
	_scriptName = name;
}

void ScriptInterpreter::setOutputStream(std::ostream* os)
{
	_outputStream = os;
}

ScriptInterpreter::IdInfo ScriptInterpreter::_info[] =
	{
		// Functions
		{SID_EXIT, idFunction, "exit", 1, nullptr},
		{SID_PRINT, idFunction, "print", std::numeric_limits<int>::max(), nullptr},
		{SID_WRITELOG, idFunction, "writelog", std::numeric_limits<int>::max(), nullptr},
		{SID_BEEP, idFunction, "beep", 2, nullptr},
		{SID_CLOCK, idFunction, "clock", 1, nullptr},
		{SID_SPEAK, idFunction, "speak", 2, nullptr},
		{SID_CLS, idFunction, "cls", 0, nullptr},
		{SID_SLEEP, idFunction, "sleep", 1, nullptr},
		{SID_RELINQUISH, idFunction, "relinquish", 0, nullptr},
		{SID_SETTIMEOUT, idFunction, "settimeout", 1, nullptr},
		{SID_READPROFILE, idFunction, "readprofile", 3, nullptr},
		{SID_WRITEPROFILE, idFunction, "writeprofile", 3, nullptr},
		{SID_SHUTDOWNSYS, idFunction, "shutdownsystem", 0, nullptr},
		{SID_EXITPROCESS, idFunction, "exitprocess", 1, nullptr},
		{SID_SHELLEXEC, idFunction, "shellexec", 4, nullptr},
		{SID_GETENVIRON, idFunction, "getenv", 1, nullptr},
		{SID_GETUSERNAME, idFunction, "getusername", 0, nullptr},
		{SID_GETCONFIGDIR, idConstant, "configdir", 1, nullptr},
		{SID_FOCUSWINDOW, idFunction, "focuswindow", 1, nullptr},
		{SID_TRACE, idFunction, "trace", 1, nullptr},
		{SID_CREATE, idFunction, "Create", -1, nullptr},
		{SID_TIME, idFunction, "Time", 0, nullptr},
		{SID_DATE, idFunction, "Date", 0, nullptr},
		// Keywords. (do not need speed index)
		{0, idKeyword, "if", kwIf, nullptr},
		{0, idKeyword, "while", kwWhile, nullptr},
		{0, idKeyword, "else", kwElse, nullptr},
		{0, idKeyword, "goto", kwGoto, nullptr},
		{0, idKeyword, "break", kwBreak, nullptr},
		{0, idKeyword, "continue", kwContinue, nullptr},
		{0, idKeyword, "gosub", kwSubroutine, nullptr},
		{0, idKeyword, "return", kwReturn, nullptr},
		{0, idKeyword, "extern", kwExtern, nullptr},
	};

strings ScriptInterpreter::getInfoNames() const
{
	strings rv = ScriptEngine::getInfoNames();
	for (auto& i : _info)
	{
		rv.add(i._name);
	}
	return rv;
}

const ScriptInterpreter::IdInfo* ScriptInterpreter::getInfo(const std::string& name) const
{
	// Look into the static info list.
	for (auto& i: _info)
	{
		if (i._name == name)
		{
			return &i;
		}
	}
	// Look for a variable name.
	for (auto i: _variables)
	{
		if (i->_name == name)
		{
			return i;
		}
	}
	// Look for labels.
	for (auto i: _labels)
	{
		if (name == i->_name)
		{
			return i;
		}
	}
	// Call the inherited member function.
	return ScriptEngine::getInfo(name);
}

const char* ScriptInterpreter::getKeyWordText(EKeyWord keyword)
{
	for (auto& i: _info)
	{
		if (i._id == (EIdentifier) keyword)
		{
			return i._name;
		}
	}
	return "<keyword?>";
}

const char* ScriptInterpreter::_stateNames[] =
	{
		"Error",
		"Empty",
		"Compiled",
		"Running",
		"Ready",
		nullptr
	};

const char* ScriptInterpreter::_instructionNames[] =
	{
		"NONE",
		"CALC",
		"ZJMP",
		"JMP ",
		"CALL",
		"RETF",
		nullptr
	};

ScriptInterpreter::Instruction::Instruction(Instruction::EInstr instr, ip_type ip, const CodePos& pos, std::string  script)
	:_script(std::move(script))
	 , _instr(instr)
	 , _absIp(ip)
	 , _codePos(pos)
{
}

const char* ScriptInterpreter::Instruction::getMnemonic() const
{
	return _instructionNames[_instr % Instruction::eiLastEntry];
}

ScriptObject::ip_type ScriptInterpreter::Instruction::getJumpIp() const
{
	if (_instr == eiJump || _instr == eiTestJump)
	{
		return _absIp;
	}
	return -1;
}

ScriptInterpreter::ip_type ScriptInterpreter::addInstruction(Instruction::EInstr instr, ip_type ip, pos_type pos, const std::string& script)
{
	CodePos cp(_codeLine, _codePos);
	_instructions.add(Instruction(instr, (ip < 0) ? (ip_type) _instructions.size() + 1 : ip, cp, script));
	_currentInstructionPtr = (ip_type)(_instructions.size() - 1);
	return _currentInstructionPtr;
}

void ScriptInterpreter::setBreakInstruction(ip_type start_ip, ip_type jmp_ip)
{
	for (auto i = start_ip; i < _instructions.size(); i++)
	{
		if (_instructions[i]._instr == Instruction::eiBreak)
		{
			setJumpInstruction(i, jmp_ip);
		}
	}
}

void ScriptInterpreter::setContinueInstruction(ip_type start_ip, ip_type jmp_ip)
{
	for (auto i = start_ip; i < _instructions.size(); i++)
	{
		if (_instructions[i]._instr == Instruction::eiContinue)
		{
			setJumpInstruction(i, jmp_ip);
		}
	}
}

ScriptInterpreter::ip_type ScriptInterpreter::getLabelIp(const std::string& name)
{
	// Before searching check the length of the name.
	if (name.length())
	{
		for (unsigned i = 0; i < _labels.size(); i++)
		{
			if (name == _labels[i]->_name)
			{
				return _labels[i]->instructionPtr();
			}
		}
	}
	// On not label found return -1.
	return -1;
}

void ScriptInterpreter::linkInstruction()
{
	// Link all call instructions.
	for (unsigned i = 0; i < _instructions.size(); i++)
	{
		// Call instruction found.
		if (_instructions[i]._instr == Instruction::eiCall)
		{
			auto ip = getLabelIp(_instructions[i]._script);
			// Check if the label is found.
			if (ip == -1)
			{
				setError(aeLabelNotFound, _instructions[i]._script);
			}
			else
			{
				// Assign the instruction index/pointer.
				_instructions[i]._absIp = ip;
			}
		}
			// Go to instruction found
		else if (_instructions[i]._instr == Instruction::eiGoto)
		{
			auto ip = getLabelIp(_instructions[i]._script);
			// Check if the label is found.
			if (ip == -1)
			{
				setError(aeLabelNotFound, _instructions[i]._script);
			}
			else
			{
				// Assign the instruction index/pointer.
				setJumpInstruction(i, ip);
			}
		}
	}
	// Link main function pointer.
	_mainFunction = getLabelIp("main");
}

void ScriptInterpreter::setJumpInstruction(ip_type ip, ip_type jmp_ip)
{
	Instruction::EInstr instr = _instructions[ip]._instr;
	if
		((instr == Instruction::eiTestJump || instr == Instruction::eiJump ||
		instr == Instruction::eiGoto || instr == Instruction::eiBreak ||
		instr == Instruction::eiContinue) &&
		ip != jmp_ip)
	{
		// jmp_ip negative jump to next ipp => ip+1
		_instructions[ip]._absIp = (jmp_ip < 0) ? (ip + 1) : jmp_ip;
		// Replace pseudo instruction with jump.
		if (_instructions[ip]._instr < Instruction::eiNone)
		{
			_instructions[ip]._instr = Instruction::eiJump;
		}
	}
	else
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Error in " << __FUNCTION__);
	}
}

const char* ScriptInterpreter::_exitCodeNames[] =
	{
		"Normal",
		"Error",
		"Script",
		"Application",
		nullptr
	};

void ScriptInterpreter::exitFunction(EExitCode exitcode, const Value& value)
{
	(void) exitcode;
	(void) value;
	SF_COND_RTTI_NOTIFY(exitcode == ecError, DO_DEFAULT, getDebugText());
}

bool ScriptInterpreter::getSetValue(const IdInfo* info, Value* result, Value::vector_type* params, bool flag_set)
{
	if (!info)
	{
		throw Exception("GetSetValue() 'info' is NULL");
	}
	// Check if the amount of parameters is correct.
	if (info->_id == idFunction && !params)
	{
		return setError(aeCompilerImplementationError, "getSetValue() 'params' is NULL");
	}
	// If the base class handled the request do not process further.
	if (ScriptEngine::getSetValue(info, result, params, flag_set))
	{
		return true;
	}
	// Check if this request is for us
	if (info->_index <= SID_COMPILER_START)
	{
		// Select action to take on speed index.
		switch (info->_index)
		{
			case SID_VARS:
				if (flag_set)
				{
					// Try to cast the pointer to a object info class pointer.
					ScriptObject* curobj = castToObject(*(Value*) info->_data);
					ScriptObject* newobj = castToObject(*result);
					// Are there any objects in this assignment present.
					// And if a change of object pointers.
					if ((curobj || newobj) && curobj != newobj)
					{
						// When the reference count of the current value is decremented
						// to zero and the object is not this compiler instance the
						// object must be destroyed.
						if (curobj && --curobj->_refCount == 0)
						{
							bool should_delete = false;
							// Call virtual overloaded function to remove the object.
							curobj->destroyObject(should_delete);
							// Check if the object should be deleted.
							if (should_delete)
							{
								delete curobj;
							}
						}
						// Increment the refcount of the new object.
						if (newobj)
						{
							newobj->_refCount++;
						}
					}
					// Assign the value to the variable. Not changing the type of the variable.
					((Value*) info->_data)->assign(*result);
				}
				else
				{
					result->set(*(Value*) info->_data);
				}
				break;

			case SID_LABELS:
				if (!flag_set)
				{
					result->set((Value::int_type) info->_data);
				}
				break;

			case SID_EXIT:
				exitFunction(ecScript, (*params)[0]);
				setState(esReady);
				result->set(0);
				// Reset instruction pointer.
				_currentInstructionPtr = -1;
				break;

			case SID_PRINT:
			{
				std::string s;
				unsigned count = params->size();
				for (unsigned i = 0; i < count; i++)
				{
					const Value& value((*params)[i]);
					ScriptObject* obj = castToObject(value);
					if (obj)
					{
						s += stringf("%s(:%p)", obj->_typeName, obj);
					}
					else
					{
						s += value.getString();
					}
					if (i < count - 1)
					{
						s += " ";
					}
				}
				std::cout << s;
				break;
			}

			case SID_WRITELOG:
			{
				std::string s;
				for (uint i = 0; i < params->size(); i++)
				{
					s += (*params)[i].getString();
					if (i < params->size() - 1)
					{
						s += " ";
					}
				}
				std::ostream& os(_outputStream ? *_outputStream : std::clog);
				os << _scriptName << ": " << s;
				result->set(0);
				break;
			}

			case SID_BEEP:
				// TODO: Beep should be implemented.
				//Beep((*params)[0].getInteger(), (*params)[1].getInteger());
				break;

			case SID_CLOCK:
				result->set((*params)[0].getFloat() - TimeSpec(getTime()).toDouble());
				if ((*params)[0].isZero())
				{
					*result *= Value(-1.0);
				}
				break;

			case SID_SPEAK:
				// TODO: Speak should be implemented.
				/*
				"Syntax:\n"
				"  Speak(append, text)\n\n"
				"Uses the speach API to speak the passed words. "
				"When 'append' is zero current spoken text is aborted "
				"otherwise it is appended to the current spoken text."
				*/
				//result->set(textToSpeech((*params)[0].getString(), (*params)[1].getInteger()));
				result->set(0);
				break;

			case SID_SLEEP:
			{
				_sleepTimer.set(TimeSpec((*params)[0].getFloat()));
				// When the script is in step mode the caller should handle the sleep time.
				if (!_flagStepMode)
				{
					// Wait until the time elapses.
					while (!_sleepTimer)
					{
						// TODO: Should call event process from QT.
						qWarning() << "Not implemented yet!";
					}
				}
				result->set(0);
				break;
			}

			case SID_RELINQUISH:
#if IS_QT
				// Process Qt events for 50 milliseconds.
				QApplication::processEvents(QEventLoop::AllEvents, 50);
#endif
				// Iterate through all functions in the sustain table.
				SustainBase::callSustain();
				result->set(0);
				break;

			case SID_CLS:
			{
				// TODO: Clear the console here.
				result->set(0);
				break;
			}

			case SID_SETTIMEOUT:
			{
				_loopTimer.set((*params)[0].getInteger());
				result->set(0);
				break;
			}

			case SID_WRITEPROFILE:
			case SID_READPROFILE:
			{
				auto path = getProfilePath();
				if (!path.empty())
				{
					// Create a profile for both functions.
					IniProfile prof
						(
							(*params)[0].getString(), // Section name.
							getProfilePath()  // Application redirected profile.
						);
					// Read from the profile.
					if (info->_index == SID_READPROFILE)
					{
						std::string val;
						prof.getString
							(
								(*params)[1].getString(), // Key name
								val,                             // Return value
								(*params)[2].getString()  // Default string
							);
						result->set(val);
					}
					else // Write to the profile.
					{
						prof.setString
							(
								(*params)[1].getString(), // Key name
								(*params)[2].getString()  // String value
							);
					}
				}
				break;
			}

			case SID_SHUTDOWNSYS:
			{
				bool retval = false;

#if IS_WIN && false
				retval = ExitWindowsEx
				(
					EWX_SHUTDOWN | 0x00000010 /*EWX_FORCEIFHUNG*/,  // shutdown operation
					0 // SHTDN_REASON_FLAG_USER_DEFINED // shutdown reason (WinXP only)
				);
				RTTI_NOTIFY(DO_DEFAULT, "Shutting down Windows...");
				/*
				retval = InitiateSystemShutdownEx
				(
					NULL,                           // computer name
					"Shutting down the System",     // message to display
					10,                             // length of time to display
					true,                           // force closed option
					false,                          // reboot option
					0,//SHTDN_REASON_FLAG_USER_DEFINED  // shutdown reason (WinXP only)
				);
				*/
#endif
				result->set(retval);
				break;
			}

			case SID_EXITPROCESS:
#if IS_QT
				QApplication::exit((int) (*params)[0].getInteger());
#else
				exit((*params)[0].getInteger());
#endif
				result->set(true);
				break;

			case SID_SHELLEXEC:
			{
#if IS_WIN && false
				std::string operation = (*params)[0].GetString();
				std::string filename = (*params)[1].GetString();
				std::string parameters = (*params)[2].GetString();
				std::string directory = (*params)[3].GetString();
				HINSTANCE hinst = ShellExecute
				(
				 ::GetDesktopWindow(),                            // handle to parent window
				 operation.length() ? operation.c_str() : NULL,    // pointer to string that specifies operation to perform
				 filename.length() ? filename.c_str() : NULL,      // pointer to filename or folder name string
				 parameters.length() ? parameters.c_str() : NULL,  // pointer to string that specifies executable-file parameters
				 directory.length() ? directory.c_str() : NULL,    // pointer to string that specifies default directory
				 SW_SHOWNORMAL                                    // whether file is shown when opened
				);
				// If the function fails, the return value is an error value that is
				// less than or equal to 32.
				result->Set((int)hinst <= 32);
#else
				result->set(false);
#endif
				// Example:
				//  shellexec("", "RUNDLL.EXE", "user.exe,exitWindows", "")
				break;
			}

			case SID_FOCUSWINDOW:
			{
#if IS_QT
				// TODO: Needs system to get main window if activeWindow() does not do the trick.
				QWidget* window = QApplication::activeWindow();
				if (window)
				{
					// This will remove minimized status and restore window with keeping maximized/normal state.
					window->setWindowState(window->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
					// This will activate the window.
					window->activateWindow();
					result->set(1);
				}
				else
				{
					result->set(0);
				}
#else
				result->set(0);
#endif
				break;
			}

			case SID_TRACE:
			{
				_trace = (*params)[0].getInteger();
				result->set(_trace);
				break;
			}

			case SID_GETENVIRON:
				result->set(getenv((*params)[0].getString().c_str()));
				break;

			case SID_GETUSERNAME:
			{
#if IS_QT
				auto name = qgetenv("USER");
				if (name.isEmpty())
				{
					name = qgetenv("USERNAME");
				}
				result->set(name.toStdString());
#endif
				break;
			}

			case SID_CREATE:
			{
				auto type_name = (*params)[0].getString();
				// Remove the type name from the passed arguments.
				params->detachAt(0);
				// Find script object by the passed type name.
				auto obj = ScriptObject::Interface().create(type_name, ScriptObject::Parameters(params, this));
				// Return the pointer to the derived class.
				result->set(Value::vitCustom, &obj, sizeof(&obj));
				//
				if (!obj)
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Script object with name '" << type_name << "' does not exist!");
				}
				break;
			}

			case SID_TIME:
			{
				char buf[80];
				time_t now = time(nullptr);
				struct tm local = *localtime(&now);
				strftime(buf, sizeof(buf), "%Y-%m-%d", &local);
				result->set(buf);
				break;
			}

			case SID_DATE:
			{
				char buf[80];
				time_t now = time(nullptr);
				struct tm local = *localtime(&now);
				strftime(buf, sizeof(buf), "%X", &local);
				result->set(buf);
				break;
			}

			case SID_GETCONFIGDIR:
				// TODO: Get profile path must some how be implemented.
				//result->set(GetFileDirAndDrive(GetProfilePath()));
				break;

			default:
				return setError(aeCompilerImplementationError, "getSetValue() not implemented");
		}
		// Signal that the call is processed here.
		return true;
	}
	// Not processed.
	return false;
}

bool ScriptInterpreter::isKeyWord(EKeyWord keyword)
{
	std::string name;
	auto pos = _codePos;
	skipWhite();
	getName(name);
	const IdInfo* info = getInfo(name);
	// ParamCount contains the keyword enumerate value.
	if (idKeyword == info->_id && info->_paramCount == keyword)
	{
		// passed key word was found
		return true;
	}
	// Restore read position to previous.
	setPosition(pos);
	return false;
}

bool ScriptInterpreter::getName(std::string& name)
{
	name.clear();
	pos_type p = 0;
	while (_command[_codePos] && (isAlpha(_command[_codePos + p]) || isalnum(_command[_codePos + p])))
	{
		p++;
		if (p > MAX_IDENT_LENGTH)
		{
			return setError(aeMaxIdentifierLength, name);
		}
	}
	name.append(_command, _codePos, p);
	_codePos += p;
	return true;
}

void ScriptInterpreter::setPosition(pos_type pos)
{
	for (; _codePos < pos; _codePos++)
	{
		if (_command[_codePos] == '\n')
		{
			_codeLine++;
		}
	}
	for (; _codePos > pos; _codePos--)
	{
		if (_command[_codePos] == '\n')
		{
			_codeLine--;
		}
	}
}

void ScriptInterpreter::skipWhite()
{
	while (_command[_codePos] && strchr("/\r\n\t ", _command[_codePos]))
	{
		// Look for comment '//'.
		if (_command[_codePos] == '/' && _command[_codePos + 1] == '/')
		{
			while (_command[_codePos] && _command[_codePos] != '\n')
			{
				_codePos++;
			}
		}
		// Look for comment '/*' end '*/'.
		if (_command[_codePos] == '/' && _command[_codePos + 1] == '*')
		{
			while (_command[_codePos] && _command[_codePos + 1])
			{
				if (_command[_codePos] == '\n')
				{
					_codeLine++;
				}
				if (_command[_codePos] == '*' && _command[_codePos + 1] == '/')
				{
					_codePos++;
					break;
				}
				_codePos++;
			}
		}
		//
		if (_command[_codePos] == '\n')
		{
			_codeLine++;
		}
		_codePos++;
	}
}

bool ScriptInterpreter::getScript(std::string& script, char ending)
{
	script.clear();
	int lth = -1;
	skipWhite();
	int paren_count = 0;
	bool stop = false;
	bool between_quotes = false;
	//
	while (getError() == aeSuccess && !stop)
	{
		lth++;
		auto pos = _codePos + lth;
		// Check if this is the last character in the script.
		if (_command[pos] == 0)
		{
			return setError(aeUnexpectedEnd, "end-of-script");
		}
		// Do nothing when between quotes.
		if (between_quotes)
		{
			// When a slash character sequence is detected step over it.
			if (_command[pos] == '\\' && _command[pos + 1] != 0)
			{
				// Always skip next character when
				lth++;
				continue;
			}
			//
			if (_command[pos] == '"')
			{
				// Enable normal processing again.
				between_quotes = false;
			}
		}
		else
		{
			switch (_command[pos])
			{
				case '"':
					// Goto into double quote operation.
					between_quotes = true;
					break;

				case '(':
					// Needs (extra) parenthesis to close.
					paren_count++;
					break;

				case ')':
					// Check if this is the ending parenthesis.
					if (paren_count == 0 && ')' == ending)
					{
						stop = true;
					}
					else
					{
						if (paren_count)
						{
							paren_count--;
						}
						else
						{
							setPosition(pos);
							return setError(aeUnexpectedCharacter, ")");
						}
					}
					break;

				case ',':
					break;

				case ';':
					if (!paren_count && ';' == ending)
					{
						stop = true;
					}
					break;
			}
		}
	}
	// When no error occurred.
	if (stop)
	{
		// Assign script.
		script.append(_command, _codePos, lth);
		// Adjust read position
		_codePos += lth;
		//
		return true;
	}
	//
	return false;
}

void ScriptInterpreter::getVarScript(std::string& script)
{
	script.clear();
	skipWhite();

	if (isAlpha(_command[_codePos]) || _command[_codePos++] == '=')
	{
		// Save the position.
		auto pos = _codePos;
		int sc = 1;
		while (sc && _command[_codePos])
		{
			skipWhite();
			//
			if (_command[_codePos] == ';')
			{
				sc--;
				if (sc)
				{
					setError(aeExpectedRightParenthesis, ")");
					return;
				}
			}
			else
			{
				if (_command[_codePos] == ')')
				{
					sc--;
				}
				if (_command[_codePos] == '(')
				{
					sc++;
				}
				_codePos++;
			}
		}
		// Assign script string.
		script.append(_command, pos, _codePos - pos);
	}
}

void ScriptInterpreter::getExternalSource(std::string& source)
{
	// Save position at process point for errors.
	// auto pos = Pos;
	// Remove the white.
	skipWhite();
	{
		char delim;
		// As long as new delimiters are entered source will be read.
		do
		{ // Get first non white character.
			delim = _command[_codePos];
			if (isalnum(delim))
			{
				delim = ';';
			}
			else
			{
				// Move beyond the delimiter.
				_codePos++;
			}
			// Read the source until the delimiter.
			while (_command[_codePos] && _command[_codePos] != delim)
			{
				source.append(_command, _codePos, 1);
				// old version only wroks in bc5
				//source.append(Cmd[Pos]);
				_codePos++;
			}
			// Move beyond the delimiter.
			if (_command[_codePos] == delim && delim != ';')
			{
				_codePos++;
			}
			// Eat the white in between delimiters.
			skipWhite();
		}
		while (_command[_codePos] && _command[_codePos] != ';');
	}
	// If the end of the script source has been entered report an error.
	if (!_command[_codePos])
	{
		setError(aeUnexpectedEnd, "end-of-script");
		return;
	}
	// After all source has been read check if it has been delimited by
	// the script delimiter ';'.
	if (_command[_codePos] != ';')
	{
		setError(aeExpectedDelimiter, ";");
		return;
	}

	// Move beyond the delimiter.
	_codePos++;
}

bool ScriptInterpreter::doCompile() // NOLINT(misc-no-recursion)
{
	skipWhite();

	if (_command[_codePos] == '\0')
	{
		return true;
	}

	if (_command[_codePos] == ';')
	{
		_codePos++;
		return true;
	}

	// enter new scoop
	if (_command[_codePos] == '{')
	{
		_codePos++;
		while (!getError() && _command[_codePos] != '}' && _command[_codePos])
		{
			if (!doCompile())
			{
				return false;
			}
			skipWhite();
		}
		if (_command[_codePos] == '}')
		{
			_codePos++;
		}
		return true;
	}
	//
	if (_command[_codePos] == ':')
	{
		_codePos++;
		skipWhite();
		std::string name;
		getName(name);
		if (_command[_codePos] != ';')
		{
			return setError(aeExpectedDelimiter, ";");
		}
		// Check if the identifier already exists.
		if (getInfo(name)->_id != idUnknown)
		{
			return setError(aeMultipleDeclaration, name);
		}
		// Add a label entry for this instruction entry.
		_labels.add(new LabelInfo(name.c_str(), SID_LABELS, (int) _instructions.size()));
		// Signal success.
		return true;
	}
	// Compile keywords
	if (isAlpha(_command[_codePos]))
	{
		skipWhite();
		auto pos = _codePos;
		std::string name;
		getName(name);
		if (getError())
		{
			return false;
		}
		// SF_RTTI_NOTIFY(DO_DEFAULT, "Identifier: " << name);
		const IdInfo* info = getInfo(name);
		switch (info->_id)
		{

			case idUnknown:
				setError(aeUnknownIdentifier, name);
				break;

			case idKeyword:
			{
				skipWhite();
				// When an error occurs return immediately.
				compileKeyword(info, pos);
				return true;
			}

			case idFunction:
			{
				setPosition(pos);
				std::string script;
				//GetVarScript(script);
				if (!getScript(script, ';'))
				{
					return false;
				}
				// Call of function is made in the TCalcScript.
				addInstruction(Instruction::eiCalculate, -1, pos, script);
				if (getError())
				{
					return true;
				}
				skipWhite();
				if (_command[_codePos] != ';')
				{
					return setError(aeExpectedDelimiter, ";");
				}
				_codePos++;
				break;
			}

			case idTypedef:
			{
				// The params count contains the type enumerate from TValue.
				auto type = (Value::EType) info->_paramCount;
				skipWhite();
				// Update position for when running into next statement.
				pos = _codePos;
				if (!getName(name))
				{
					return false;
				}
				if (getInfo(name)->_id != idUnknown)
				{
					return setError(aeMultipleDeclaration, name);
				}
				// Create TVar having a speed index which is the index in the list.
				auto var = new VariableInfo(name.c_str(), type, SID_VARS);
				_variables.add(var);
				// Assign the info pointer to the new create derived info pointer var
				// when running into next case statement.
				info = var;
				skipWhite();
				// Is an assignment command following.
				if (_command[_codePos] != '=')
				{
					break;
				}
			} // run into next case statement

			case idVariable:
			case idConstant:
				skipWhite();
				{
					// Restore the read pointer to start of the object name.
					setPosition(pos);
					// Read the script.
					std::string script;
					if (!getScript(script, ';'))
					{
						return false;
					}
					else
					{ // Check for the wanted delimiter.
						if (_command[_codePos] != ';')
						{
							return setError(aeExpectedDelimiter, ";");
						}
						// Skip the delimiter.
						_codePos++;
						// Add the calculation instruction using the object code.
						addInstruction(Instruction::eiCalculate, -1, pos, script);
					}
					break;
				}

			default:
				// Set error position before keyword
				setPosition(pos);
				return setError(aeUnexpectedIdentifier, name);
		} // end switch GetIdentifierInfo()
		//
		skipWhite();
		//
		return true;
	}
	//
	return setError(aeUnexpectedCharacter, std::string(1, _command[_codePos]));
}

bool ScriptInterpreter::compileKeyword(const IdInfo* Info, pos_type pos) // NOLINT(misc-no-recursion)
{
	// ParamCount is in case of the keyword identifier the type of keyword.
	auto kw = (EKeyWord) Info->_paramCount;
	switch (kw)
	{
		case kwBreak:
			addInstruction(Instruction::eiBreak, -1, pos, "BREAK");
			break;

		case kwContinue:
			addInstruction(Instruction::eiContinue, -1, pos, "CONTINUE");
			break;

		case kwSubroutine:
		{
			// No break just run into next instruction.
			std::string name;
			getName(name);
			addInstruction(Instruction::eiCall, -1, pos, name);
			skipWhite();
			// Check for the delimiter.
			if (_command[_codePos] == ';')
			{
				_codePos++;
			}
			else
			{
				return setError(aeExpectedDelimiter, ";");
			}
		}
			break;

		case kwGoto:
		{
			std::string name;
			getName(name);
			addInstruction(Instruction::eiGoto, -1, pos, name);
			skipWhite();
			// Check for the delimiter.
			if (_command[_codePos] == ';')
			{
				_codePos++;
			}
			else
			{
				return setError(aeExpectedDelimiter, ";");
			}
		}
			break;

		case kwReturn:
		{
			addInstruction(Instruction::eiRetFunction, -1, _codeLine);
			skipWhite();
			// Check for the delimiter.
			if (_command[_codePos] == ';')
			{
				_codePos++;
			}
			else
			{
				return setError(aeExpectedDelimiter, ";");
			}
		}
			break;

		case kwIf:
		{
			std::string script;
			if (_command[_codePos] != '(')
			{
				return setError(aeExpectedLeftParenthesis, "(");
			}
			// Skip parenthesis character.
			_codePos++;
			// Get script between parentheses.
			if (getScript(script, ')'))
			{
				// Skip parenthesis character.
				_codePos++;
			}
			else
			{
				return false;
			}
			//
			auto jmp_f = addInstruction(Instruction::eiTestJump, -1, pos, script);
			// Compile true scope.
			if (!doCompile())
			{
				return false;
			}
			// Set first jump ip to second jump instruction
			setJumpInstruction(jmp_f, _currentInstructionPtr + 1);
			// Add instruction
			auto jmp_e = addInstruction(Instruction::eiJump, -1, pos);
			// Check if next word is key word 'ELSE'
			if (isKeyWord(kwElse))
			{
				// Compile else scope and set jump instructions
				doCompile();
				setJumpInstruction(jmp_f, jmp_e + 1);
				setJumpInstruction(jmp_e, _currentInstructionPtr + 1);
			}
			if (getError())
			{
				return false;
			}
		}
			break;

		case kwWhile:
		{
			std::string script;
			if (_command[_codePos] != '(')
			{
				return setError(aeExpectedLeftParenthesis, "(");
			}
			// Skip parenthesis character.
			_codePos++;
			// Get script between parentheses.
			if (getScript(script, ')'))
			{
				// Skip parenthesis character.
				_codePos++;
			}
			else
			{
				return false;
			}
			// Test of while loop
			// jump while scope if non zero
			auto ipt = addInstruction(Instruction::eiTestJump, -1, pos, script);
			// compile while scope
			doCompile();
			// return if an error occurred
			if (getError())
			{
				return false;
			}
			// jump back to test of while scope
			addInstruction(Instruction::eiJump, ipt, pos);
			// set test jump of while beyond scope
			setJumpInstruction(ipt, _currentInstructionPtr + 1);
			// set break jumps if any
			setBreakInstruction(ipt, _currentInstructionPtr + 1);
			// set break jumps if any
			setContinueInstruction(ipt, ipt);
		}
			break;

		case kwExternal:
		{
			skipWhite();
			std::string source;
			getExternalSource(source);
			if (!getError())
			{
				if (!compileAdditionalStatement(Info, source))
				{
					return setError(aeExternalKeyword, source);
				}
			}
		}
			break;

		default:
			return setError(aeUnexpectedKeyword, getKeyWordText(kw));
	}// switch
	return true;
}

bool ScriptInterpreter::compileAdditionalStatement(const IdInfo* info, const std::string& source)
{
	return true;
}

void ScriptInterpreter::clear()
{
	// Set state to not compiled so no execution takes place.
	setState(esEmpty);
	// Null code text character pointer.
	_command = nullptr;
	// Set read pointer to zero.
	_codePos = 0;
	// Set Line counter to zero.
	_codeLine = 0;
	// Set instruction pointer to Zero.
	_currentInstructionPtr = 0;
	// Clear the step mode.
	_flagStepMode = false;
	// Clear the variables and flush vector.
	for (unsigned i = 0; i < _variables.size(); i++)
	{
		ScriptObject* so = castToObject(_variables[i]->_value);
		if (so)
		{
			if (so->_refCount == 1)
			{
				bool should_delete = false;
				so->destroyObject(should_delete);
				// Check if the object may be deleted.
				if (should_delete)
				{
					delete so;
				}
			}
			else
			{
				so->_refCount--;
			}
		}
		//
		delete _variables[i];
	}
	// Clear the vars list.
	_variables.clear();
	// Clear the labels and flush vector.
	for (unsigned i = 0; i < _labels.size(); i++)
	{
		delete _labels[i];
	}
	// Clear the pointers list.
	_labels.clear();
	// Clear the IP stack.
	_stack.clear();
	// Clear instructions and flush vector.
	_instructions.clear();
	// Init error value to 'success' to start compilation,
	setError(aeSuccess);
}

bool ScriptInterpreter::compile(const char* cmd)
{
	// Cannot compile while running macro.
	if (_currentState == esRunning)
	{
		return false;
	}
	// Clears the error, stacks and compiled instruction and sets the esEMPTY state.
	clear();
	// Assign member variable with command std::string pointer.
	_command = cmd;
	// Do the actual compilation.
	while (!getError())
	{
		doCompile();
		skipWhite();
		if (_command[_codePos] == '\0')
		{
			// Link all goto statements.
			linkInstruction();
			// When output stream is enabled.
			if (_outputStream)
			{
				// Show all declared script variables.
				for (auto& i: _variables)
				{
					*_outputStream << "VARIABLES: " << i->_name << " " << i->_value << '\n';
				}
				// Clear the labels and flush vector.
				for (auto& i: _labels)
				{
					*_outputStream << "LABELS: " << i->_name << " " << i->instructionPtr() << '\n';
				}
			}
			// Set execution state to ready
			setState((getError() == aeSuccess) ? esCompiled : esEmpty);
			// Print code for debug purposes
			if (_currentState == esCompiled)
			{
				if (_outputStream)
				{
					for (ip_type ip = 0; ip < _instructions.size(); ip++)
					{
						int instr = _instructions[ip]._instr;
						instr = (instr > 0) ? instr % Instruction::eiLastEntry : 0;
						*_outputStream << stringf("%3i: %s %2i: %s\n", ip, _instructionNames[instr],
							_instructions[ip]._absIp, _instructions[ip]._script.c_str());
					}
				}
			}
			// Set the instruction pointer to -1 to start with.
			_currentInstructionPtr = -1;
			// Return true if no error occurred
			return getError() == aeSuccess;
		}
	}
	// Signal failure.
	return false;
}

bool ScriptInterpreter::callFunction(const std::string& name, bool step_mode)
{
	auto ip = getLabelIp(name);
	// Before searching check the length of the name.
	if (ip != -1)
	{
		// Make the script start calling the label/function.
		callFunction(ip, step_mode);
		// Stop iterating here.
		return true;
	}
	SF_RTTI_NOTIFY(DO_DEFAULT, "Function or label '" << name << "' not present in script!");
	// Label was not found.
	return false;
}

void ScriptInterpreter::callFunction(ip_type ip, bool step_mode)
{
	// Only possible when compiled.
	if (_currentState != esEmpty && _currentState != esError && ip != -1)
	{
		// Push the current instruction pointer on the stack
		_stack.push_back(StackEntry(_currentInstructionPtr));
		// Jump to gosub label.
		_currentInstructionPtr = ip;
		// Must function be called or only started.
		if (step_mode)
		{
			// Set state to running.
			if (_currentState == esReady)
			{
				setState(esRunning);
			}
		}
		else
		{
			// Execute the function in run mode(none stepping).
			if (_currentState == esReady || _currentState == esRunning || _currentState == esCompiled)
			{
				// Save the previous state to be able to restore it.
				EState prev_state = _currentState;
				// Set the execution state to running so compiler is behaving correctly.
				setState(esRunning);
				// Get the previous stack count for breaking the loop.
				unsigned stack_pos = _stack.size() - 1;
				// Timer to prevent endless loops.
				_loopTimer.reset();
				// Execute instructions as long as no error occurs and the script does not finish.
				while (_currentState != esError && _currentState != esReady)
				{
					// Execute one instruction.
					doStep();
					// Stop the loop when stack is back to the original state.
					if (_stack.size() <= stack_pos)
					{ // Set the state back to the previous value.
						setState(prev_state);
						// Break the while loop.
						break;
					}
					// Set the error state when the timer elapses.
					if (_loopTimer)
					{
						// Execution state to error.
						setState(esError);
						// Set the error value for the script.
						setError(aeScriptTimeout, stringf("%.3lf s", _loopTimer.getElapseTime().toDouble()));
						// Report debug information.
						SF_RTTI_NOTIFY(DO_DEFAULT, "Script function Timed Out!");
						// Break the loop.
						break;
					}
				}
			}
		}
	}
}

void ScriptInterpreter::doExecute()
{
	// When an error occurred do not process at all.
	if (_currentState == esError)
	{
		return;
	}
	// Check if main has finished.
	if (_currentInstructionPtr < 0)
	{
		return;
	}
	// When tracing is enabled.
	if (_trace)
	{
		auto s = std::string("Script '") + getScriptName() + "': " + getInstructionText(_currentInstructionPtr) + "\n";
		// Write trace test to the log file.
		if (_outputStream)
		{
			*_outputStream << s;
		}
	}
	// Assign the instruction pointer with new instruction.
	Instruction* instr = &_instructions[_currentInstructionPtr];
	// process the new instruction.
	switch (instr->_instr)
	{
		case Instruction::eiNone:
			_currentInstructionPtr++;
			break;

		case Instruction::eiCalculate:
		case Instruction::eiTestJump:
		{
			// Store temporarily the IP stack size.
			auto count = _stack.size();
			// Calculate the script and return the value in the Accu member.
			bool ok = calculate(instr->_script, _accumulator);
			// Break of here when the stack was cleared by an Abort or so.
			if (count > _stack.size())
			{
				return;
			}
			// Get reference to the real stack pointer to increment.
			auto& ip(count == _stack.size() ? _currentInstructionPtr : _stack[count - (_currentState == esRunning ? 0 : 1)]._ip);
			// Check if the stack has been incremented.
			if (!ok)
			{
				// Make sure that the correct instruction pointer is reported at the error reporting.
				_currentInstructionPtr = ip;
				// Set the error.
				setState(esError);
				break;
			}
			// Check if a jump must be made.
			if (instr->_instr == Instruction::eiTestJump && _accumulator.isZero())
			{
				ip = instr->_absIp;
			}
			else
			{
				// Do not increment when set to -1.
				if (ip != -1)
				{
					ip++;
				}
			}
			break;
		}

		case Instruction::eiJump:
			_currentInstructionPtr = instr->_absIp;
			break;

		case Instruction::eiCall:
		{
			// Push the next instruction pointer on the stack
			_stack.push_back(StackEntry(_currentInstructionPtr + 1));
			// SF_RTTI_NOTIFY(DO_DEFAULT, << "Pushing IP: " << (InstrPtr+1));
			// Jump to function or label.
			_currentInstructionPtr = instr->_absIp;
			break;
		}

		case Instruction::eiRetFunction:
		{
			auto count = _stack.size();
			if (count)
			{
				_currentInstructionPtr = _stack[count - 1]._ip;
				//_stack.detachAt(count - 1);
				_stack.pop_back();
				// SF_RTTI_NOTIFY(DO_DEFAULT, "Popping IP: " << InstrPtr);
			}
			else
			{
				// When a ip value must be popped and there are none to pop
				// an error is generated.
				setError(aeIpStack, "Unexpected end of stack.");
				setState(esError);
			}
			break;
		}

		default:
		case Instruction::eiLastEntry:
			setState(esError);
			break;
	}
}

void ScriptInterpreter::doStep()
{
	// Execute an instruction if the state is esRUNNING
	if (_currentState == esRunning)// && State != esERROR)
	{
		if (_currentInstructionPtr < _instructions.size())
		{
			doExecute();
			if (_currentState == esError)
			{
				exitFunction(ecError, Value(ecError));
			}
		}
		else
		{
			exitFunction(ecNormal, Value(ecNormal));
			setState(esReady);
			// Reset the instruction pointer
			_currentInstructionPtr = -1;
		}
	}
}

ScriptInterpreter::EState ScriptInterpreter::Execute(EExecMode exec_mode)
{
	// If sentry flag is set this function cannot be entered
	if (_flagSentry && exec_mode != emAbort)
	{
		// When a dialog is executing the TScriptLink instance will call
		// this function and the sentry will block this.
		// But it is used to reset the loop timer.
		if (exec_mode == emStep)
		{
			_loopTimer.reset();
		}
		//
		return _currentState;
	}
	// Set sentry flag to disable reentrancy.
	_flagSentry = true;
	// Check for valid states
	switch (exec_mode)
	{
		// Start script for running in step mode.
		case emStart:
			if (_currentState == esReady)
			{
				// Set instruction to  where it should start depending on an existing main function.
				if (_mainFunction)
				{
					// Push end of stack -1 so when main function returns it sets the ready status.
					_stack.push_back(StackEntry(-1));
					_currentInstructionPtr = _mainFunction;
				}
				else
				{
					_currentInstructionPtr = 0;
				}
				setState(esRunning);
				_flagStepMode = true;
			}
			else
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Can only Start when in state READY!");
			}
			break;

			// Return after one instruction only.
		case emStep:
			// Check if the compiler is in step mode.
			if (_currentState == esRunning && _flagStepMode)
			{
				// Check if a sleep was issued.
				if (_sleepTimer.isEnabled())
				{ // Check if the time has timed out already.
					if (_sleepTimer)
					{
						// Disable the timer so it can be enabled next time.
						_sleepTimer.disable();
					}
				}
				else
				{
					doStep();
				}
			}
			else
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Can only Step when in state RUNNING after Start!");
			}
			break;

			// Initializes the script.
		case emInit:
			if (_currentState == esCompiled)
			{
				// Check the existence of the main function or label.
				if (_mainFunction >= 0)
				{
					// Call the script from the start of the instruction list to initialize it.
					callFunction(0, false);
				}
				// When no error occurred set the ready state.
				if (_currentState != esError)
				{
					setState(esReady);
				}
			}
			else
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Can only Initialize when state COMPILED!");
			}
			break;

			// Run script completely.
		case emRun:
			if (_currentState == esReady)
			{
				// Set the instruction pointer.
				_currentInstructionPtr = -1;
				// Call the script from the start in non step mode.
				// The start is determined by the existence of the main function or label.
				callFunction((_mainFunction >= 0) ? _mainFunction : 0, false);
				//
				switch (_currentState)
				{
					case esRunning:
						exitFunction(ecNormal, Value(ecNormal));
						setState(esReady);
						// Reset the instruction pointer
						_currentInstructionPtr = -1;
						break;

					case esError:
						exitFunction(ecError, Value(ecError));
						break;

					default:
						// needs no handling because 'ExitFunction' was probably called by script
						break;
				}
			}
			else
			{
				SF_RTTI_NOTIFY(DO_DEFAULT, "Can only Run when state is READY!");
			}
			break;

			// Aborts a running script.
		case emAbort:
			if (_currentState == esRunning)
			{
				exitFunction(ecApplication, Value(ecApplication));
				setState(esReady);
				// Reset the instruction pointer.
				_currentInstructionPtr = -1;
				// Flush the IP stack.
				_stack.clear();
			}
			break;
	}
	// Set the state to error when an error occurred somewhere.
	if (getError() != aeSuccess)
	{
		setState(esError);
	}
	// Reset sentry flag to enable entrance.
	_flagSentry = false;
	// Notify listeners.
	_emitterChange.callListeners(this);
	// Return the current state.
	return _currentState;
}

void ScriptInterpreter::setState(EState exec_state)
{
	_prevState = _currentState;
	// const cast so direct accessing of member generates a compile error.
	*(EState*) &_currentState = exec_state;
	// When going into error.
	if (_currentState == esError && _prevState != esError)
	{
		// Set the error instruction pointer too.
		_errorInstructionPtr = _currentInstructionPtr;
	}
	// Clear the error instruction pointer when the error is turned off.
	if (_currentState != esError)
	{
		_errorInstructionPtr = -1;
	}
	// Notify listeners.
	_emitterChange.callListeners(this);
}

ScriptInterpreter::CodePos ScriptInterpreter::getErrorPos() const
{
	if (_currentState == esEmpty)
	{
		return {_codeLine, 0};
	}
	else
	{
		CodePos pos;
		if (_errorInstructionPtr > 0)
		{ // Get the position of the instruction in the code.
			pos = _instructions[_errorInstructionPtr]._codePos;
			// Add offset of the error.
			pos._offset += ScriptEngine::getPos();
		}
		return pos;
	}
}

std::string ScriptInterpreter::getDebugText() const
{
	std::string retval =
		"Script: " + getScriptName() + "\n" +
			"State: " + getStateName(_currentState) + "\n" +
			"Error: '" + getErrorReason() + "' " + getErrorText(getError());
	//
	if (_currentState == esEmpty)
	{
		retval += stringf("\nLocation: Line %i, Pos %i", _codeLine + 1, _codePos);
	}
	else if (_errorInstructionPtr < _instructions.size() && _errorInstructionPtr >= 0)
	{
		retval += stringf("\nLocation: Instr %i %s at Line %i at Pos %i",
			_currentInstructionPtr, _instructions[_errorInstructionPtr]._script.c_str(), getErrorPos()._line,
			getErrorPos()._offset);
	}
	//
	return retval;
}

std::string ScriptInterpreter::getInstructionText(ip_type ip) const
{
	if (ip >= 0 && ip < _instructions.size())
	{
		return stringf("%3i: %10s %2i \"%s\"", ip,
			_instructionNames[_instructions[ip]._instr % Instruction::eiLastEntry],
			_instructions[ip]._absIp,
			_instructions[ip]._script.c_str());
	}
	return {70, ' '};
}

std::string ScriptInterpreter::getProfilePath()
{
	return {};
}

const char* ScriptInterpreter::getStateName(EState exec_state) const
{
	// When esCurrent or a larger value is passed return the current state instead.
	return _stateNames[(exec_state >= esCurrent) ? _currentState : exec_state];
}

ScriptInterpreter::EState ScriptInterpreter::getState() const
{
	return _currentState;
}

ScriptInterpreter::ip_type ScriptInterpreter::getInstructionPtr() const
{
	return _currentInstructionPtr;
}

ScriptInterpreter::ip_type ScriptInterpreter::getErrorInstructionPtr() const
{
	return _errorInstructionPtr;
}

void ScriptInterpreter::flush()
{
	clear();
}

void ScriptInterpreter::setMaxLoopTime(unsigned long usec)
{
	_loopTimer.set(usec);
}

bool ScriptInterpreter::isStepMode() const
{
	return _flagStepMode;
}

strings ScriptInterpreter::getIdentifiers(EIdentifier id) const
{
	auto rv = ScriptEngine::getIdentifiers(id);
	for (auto& i: _info)
	{
		if (i._id == id)
		{
			rv.add(i._name);
		}
	}
	return rv;
}

}
