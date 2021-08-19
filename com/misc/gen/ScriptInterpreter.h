#pragma once

#include "ElapseTimer.h"
#include "TListener.h"
#include "ScriptEngine.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Script interpreter for running a loaded script.
 */
class _MISC_CLASS ScriptInterpreter :public ScriptEngine
{
	public:
		/**
		 * @brief Declare type for listening and at teh sme time a type for emitting
		 * the changes of the script like State and Error and execution pointer.
		 */
		typedef TListener<const ScriptInterpreter*> ChangeListener;

		/**
		 * @brief Default constructor
		 */
		ScriptInterpreter();

		/**
		 * @brief Virtual destructor
		 */
		~ScriptInterpreter() override;

		/**
		 * @brief Gets the emitter for change events.
		 */
		ChangeListener::emitter_type& getChangeEmitter()
		{
			return _emitterChange;
		}

		/**
		 * @brief Returns the name given by the user.
		 *
		 * Is used for referencing when debugging.
		 */
		[[nodiscard]] std::string getScriptName() const;

		/**
		 * @brief Sets the name given by the user.
		 *
		 * Is used for referencing when debugging.
		 */
		void setScriptName(const std::string& name);

		/**
		 * @brief Sets and enables debugging output stream.
		 */
		void setOutputStream(std::ostream* os);

		/**
		 * @brief Sate of the running script.
		 */
		enum EState
		{
			/** Script went into an error due to compiling or running. */
			esError = 0,
			/** Script is empty */
			esEmpty,
			/** Script is compiled and ready for initialization. */
			esCompiled,
			/** Script is running in step mode. (also internally used in run mode) */
			esRunning,
			/** Script is initialized and ready for running or just finished running. */
			esReady,
			/** When passed to #getStateName() the current state is used. */
			esCurrent
		};

		/**
		 * @brief Enumerate for indicating the mode of execution.
		 */
		enum EExecMode
		{
			/** Initialize script in continues mode.*/
			emInit,
			/** Run script in continuous mode. */
			emRun,
			/** Starts a stepping macro again. */
			emStart,
			/** Execute only one instruction and return. */
			emStep,
			/** To abort a running script. */
			emAbort,
		};

		/**
		 * @brief Position in the source with line and offset.
		 */
		struct CodePos
		{
			CodePos() = default;

			CodePos(pos_type line, pos_type offset)
				:_line(line), _offset(offset) {}

			pos_type _line{0};
			pos_type _offset{0};
		};

		/**
		 * @brief Executes a macro depending on the execution mode.
		 *
		 * @return State execution.
		 */
		EState Execute(EExecMode exec_mode = emRun);

		/**
		 * @brief Compiles the passed script text and outputs debug info to the stream.
		 *
		 * @param cmd Script to compile.
		 * @return True when successful.
		 */
		bool compile(const char* cmd);

		/**
		 * @brief Gets the execution state name from the passed state or the current one on state 'esCurrent'.
		 */
		[[nodiscard]] const char* getStateName(EState exec_state = esCurrent) const;

		/**
		 * Gets the current execution state.
		 */
		[[nodiscard]] EState getState() const;

		/**
		 * Gets the debug text.
		 */
		[[nodiscard]] std::string getDebugText() const;

		/**
		 * Returns the current error position where the compiler stopped.
		 */
		[[nodiscard]] CodePos getErrorPos() const;

		/**
		 * Gets the current instruction pointer.
		 */
		[[nodiscard]] ip_type getInstructionPtr() const;

		/**
		 * @brief Gets the error instruction pointer in case of an error.
		 */
		[[nodiscard]] ip_type getErrorInstructionPtr() const;

		/**
		 * @brief Gets the instruction text at pointer IP.
		 */
		[[nodiscard]] std::string getInstructionText(ip_type ip) const;

		/**
		 * @brief Calls the protected virtual clear function.
		 */
		void flush();

		/**
		 * Calls a label/function in the script.
		 * @param name Name of the label.
		 * @param step_mode When true in step mode
		 * @return True when label/function was found.
		 */
		bool callFunction(const std::string& name, bool step_mode);

		/**
		 * @brief Makes it possible to execute function/gosub.
		 */
		void callFunction(ip_type ip, bool step_mode);

		/**
		 * @brief Sets the maximum loop time for this instance
		 *
		 * @param usec Microseconds.
		 */
		void setMaxLoopTime(unsigned long usec);

		// Overloaded from base class.
		[[nodiscard]] strings getInfoNames() const override;

		/**
		 * Overloaded from base class to append more information on identifier/keywords.
		 *
		 * @param name Name of the identifier or keyword.
		 * @return Information when found else nullptr.
		 */
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		/**
		 * @brief Executable instruction as result of interpreting the script.
		 */
		struct _MISC_CLASS Instruction
		{
			/**
			 * @brief Interpreter available instructions.
			 */
			enum EInstr
			{
				eiGoto = -3,
				eiContinue = -2,
				eiBreak = -1,
				eiNone = 0,
				eiCalculate,
				eiTestJump,
				eiJump,
				eiCall,
				eiRetFunction,
				eiLastEntry
			};

			/**
			 * @brief Default constructor.
			 */
			Instruction() = default;

			/**
			 * @brief Initializing constructor.
			 */
			Instruction(EInstr instr, ip_type ip, const CodePos& pos, std::string script = std::string());

			/**
			 * @brief Operation instruction.
			 */
			EInstr _instr{eiNone};
			/**
			 * @brief Position in the source.
			 */
			CodePos _codePos{0, 0};
			/**
			 * @brief Partial script to calculate and to be used for a decision to jump or not.
			 */
			std::string _script{};
			/**
			 * @brief Absolute instruction pointer address for jump instruction.
			 */
			ip_type _absIp{-1};

			/**
			 * @brief Gets the mnemonic of the current instruction.
			 */
			[[nodiscard]] const char* getMnemonic() const;

			/**
			 * @brief Gets the jump ip from this instruction when applicable.
			 *
			 * @returns -1 when not applicable.
			 */
			[[nodiscard]] ip_type getJumpIp() const;
		};

		/**
		 * Forward definition of private type.
		 */
		struct VariableInfo;
		/**
		 * Forward definition of private type.
		 */
		struct LabelInfo;

		/**
		 * @brief Gets the instruction text at pointer IP.
		 */
		[[nodiscard]] const TVector<Instruction>& getInstructions() const {return _instructions;}

		/**
		 * @brief Gets the current variables declared in the script.
		 */
		[[nodiscard]] const TVector<VariableInfo*>& getVariables() const {return _variables;}

		/**
		 * @brief Gets a list of available identifiers.
		 *
		 * Used for syntax high lighting for example or code completion.
		 */
		[[nodiscard]] strings getIdentifiers(EIdentifier id) const override;

		/**
		 * @brief Get the step mode of the script.
		 * @return True when the script is in step mode.
		 */
		[[nodiscard]] bool isStepMode() const;

	protected:
		/**
		 * @brief Possible exit codes.
		 */
		enum EExitCode
		{
			/** Default normal stop at end of script */
			ecNormal = 0,
			/** Exit as result of an error in the script */
			ecError,
			/** Script exited by script function Exit() */
			ecScript,
			/** Script exited by Execute() member function */
			ecApplication,
		};

		/**
		 * @brief Keywords available for the script.
		 */
		enum EKeyWord
		{
			kwNone = 0,
			kwIf,
			kwWhile,
			kwElse,
			kwGoto,
			kwBreak,
			kwContinue,
			kwSubroutine,
			kwReturn,
			kwExternal,
			kwExtern,
		};

		/**
		 * @brief Overridden from #ScriptEngine class.
		 */
		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

		/**
		 * @brief Compile external supplied keyword.
		 *
		 * @return On error it returns 'false'.
		 */
		virtual bool compileAdditionalStatement(const IdInfo* info, const std::string& source);

		/**
		 * @brief Function is called at the exit of the script.
		 *
		 * @param exitcode Exit code provided.
		 * @param value
		 */
		virtual void exitFunction(EExitCode exitcode, const Value& value);

		/**
		 * @brief Clears compiled data can be overloaded by derived classes.
		 */
		virtual void clear();

		/**
		 * @brief Links the labels of the calls in the script.
		 */
		virtual void linkInstruction();

		/**
		 * @brief Sets the execution state.
		 */
		void setState(EState exec_state);

		/**
		 * @brief Gets the ip from the label name.
		 *
		 * @return On failure -1.
		 */
		ip_type getLabelIp(const std::string& name);

		/**
		 * @brief Timer to prevent endless loops.
		 *
		 * Must be reset by functions calling external functions.
		 */
		ElapseTimer _loopTimer;
		/**
		 * @brief Output stream to write debug information to.
		 */
		std::ostream* _outputStream{nullptr};

	private:

		/**
		 * @brief Gets the external source. First non white character is also delimiting character.
		 */
		void getExternalSource(std::string& source);

		/**
		 * @brief Adds instruction to the list.
		 */
		ip_type addInstruction(Instruction::EInstr instr, ip_type ip, pos_type pos, const std::string& script = std::string());

		/**
		 * @brief Makes a jump instruction of the passed instruction at index 'jmp_ip'.
		 */
		void setJumpInstruction(ip_type ip, ip_type jmp_ip = -1);

		/**
		 * @brief Sets break jump.
		 */
		void setBreakInstruction(ip_type start_ip, ip_type jmp_ip);

		/**
		 * @brief Sets continue jump.
		 */
		void setContinueInstruction(ip_type start_ip, ip_type jmp_ip);

		/**
		 * @brief Moves member Pos as much as needed to skip white space.
		 */
		void skipWhite();

		/**
		 * @brief Sets the read position to pos and adjust the line count in the process.
		 */
		void setPosition(pos_type pos);

		/**
		 * @brief Gets a name from the script.
		 */
		bool getName(std::string& name);

		/**
		 * @brief Compiles one scope.
		 *
		 * @return True on success.
		 */
		bool doCompile();

		/**
		 * @brief Compiles keywords when found in DoCompile().
		 */
		bool compileKeyword(const IdInfo* Info, pos_type pos);

		/**
		 * @brief Process one instruction.
		 */
		void doStep();

		/**
		 * @brief Executes an instruction.
		 */
		void doExecute();

		/**
		 * @brief Checks if next keyword is the one passed
		 *
		 * @param keyword
		 * @return When true 'Pos' is moved beyond the keyword
		 */
		bool isKeyWord(EKeyWord keyword);

		/**
		 * @brief Gets the text of the passed keyword.
		 *
		 * @param keyword
		 * @return Keyword as text.
		 */
		const char* getKeyWordText(EKeyWord keyword);

		/**
		 * @brief Gets the script for current location.
		 *
		 * @param script
		 * @param ending
		 * @return True on success.
		 */
		bool getScript(std::string& script, char ending);

		/**
		 * @brief ???
		 */
		void getVarScript(std::string& script);

		/**
		 * @brief Gets the profile path for possible settings.
		 */
		std::string getProfilePath();

		/**
		 * @brief Contains execute state.
		 *
		 * Use the Get and Set methods to change it's value.
		 */
		const EState _currentState{esEmpty};
		/**
		 * @brief Contains previous execute state.
		 */
		EState _prevState{esEmpty};
		/**
		 * @brief Accumulator for arithmetic calculations.
		 */
		Value _accumulator{};
		/**
		 * @brief Pointer to macro source.
		 */
		const char* _command;
		/**
		 * @brief Position in text during compiling and in case of an error.
		 */
		pos_type _codePos{0};
		/**
		 * @brief Line in text during compiling and in case of an error.
		 */
		pos_type _codeLine{0};
		/**
		 * @brief Flag that blocks reentry of this class.
		 */
		bool _flagSentry{false};
		/**
		 * @brief Flag vor determining mode of the script.
		 */
		bool _flagStepMode{false};
		/**
		 * @brief Contains compiled instructions.
		 */
		TVector<Instruction> _instructions{};
		/**
		 * @brief Current compile and execute instruction index pointer.
		 */
		ip_type _currentInstructionPtr{-1};
		/**
		 * @brief Holds the error instruction index in case of an error.
		 */
		ip_type _errorInstructionPtr{-1};
		/**
		 * @brief Instruction index to main function. When main does not exists it is -1.
		 */
		ip_type _mainFunction{-1};

		/**
		 * @brief Stack for the instruction pointer.
		 */
		struct _MISC_CLASS StackEntry
		{
			explicit StackEntry(ip_type ip)
			{
				_ip = ip;
			}

			/**
			 * Instruction pointer to return to.
			 */
			ip_type _ip{0};
		};

		/**
		 * @brief Holds the stack.
		 */
		TVector<StackEntry> _stack;
		/**
		 * @brief Contains variables declared the script.
		 */
		TVector<VariableInfo*> _variables;
		/**
		 * @brief Holds the label entries of the compiled script.
		 */
		TVector<LabelInfo*> _labels;
		/**
		 * @brief Timer for when sleep is called.
		 */
		ElapseTimer _sleepTimer;
		/**
		 * @brief Holds the name used for debugging purposes and is initially '<unknown>'.
		 */
		std::string _scriptName;
		/**
		 * @brief Holds the trace status where 0 = Off, 1 = _outputStream.
		 */
		Value::int_type _trace{0};
		/**
		 * @brief Holds a static list of identifier info.
		 */
		static IdInfo _info[];
		/**
		 * @brief Contains static names of instruction names.
		 */
		static const char* _instructionNames[];
		/**
		 * @brief Contains static execute states names.
		 */
		static const char* _stateNames[];
		/**
		 * @brief Contains static exit code names
		 */
		static const char* _exitCodeNames[];

		/**
		 * @brief Holds emitter for change events.
		 */
		ChangeListener::emitter_type _emitterChange;
};

}
