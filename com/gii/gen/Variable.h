#pragma once

#include "VariableHandler.h"

namespace sf
{

/**
 * @brief Class for creating and referencing global or local created parameters or settings called variables.<br>
 * This class allows linking of member functions of other classes to handle events generated.<br>
 * See the [Example](sf-gii-Variable.html) on how to use this class and the 'TVariableHandler' template class.<br>
 */
class _GII_CLASS Variable :public InformationBase, public VariableTypes
{
	public:
		/**
		 * @brief Must be called to initialize the statics which this class depends on.
		 */
		static void initialize();

		/**
		 * @brief Called when terminating the application.
		 */
		static void deinitialize();

		/**
		 * @brief Default constructor for a default global variable.
		 */
		Variable();

		/**
		 * @brief Copy constructor. Copies also the Desired ID data member.
		 */
		Variable(const Variable& v);

		/**
		 * @brief Creates an instance having a reference with 'id'.
		 *
		 * @param id Identifying number.
		 * @param set_desired When true the desired id data member is set to the passed id as well.
		 */
		Variable(id_type id, bool set_desired);

		/**
		 * @brief Creates a variable according to the definition std::string passed to it.
		 *
		 * The "id" from the definition string is the passed id offset added to to form a final id.
		 * This used when instances are created from resource strings for multiple instances of the same module.
		 * @param definition Definition string.
		 * @param id_ofs Offset for the 'id'.
		 */
		explicit Variable(const std::string& definition, id_type id_ofs = 0);

		/**
		 * @brief Creates a variable using the passed definition structure.
		 *
		 * @param def Definition structure.
		 * @param id_ofs Offset for the 'id'.
		 */
		explicit Variable(const Definition& def, id_type id_ofs = 0);

		/**
		 * @brief Virtual destructor.
		 */
		~Variable() override;

		/**
		 * @brief Creates a variable according to the definition std::string passed to it.

		 * The passed identifier offset 'id_ofs' is added to the id in the string.
		 * This used when instances are created from resource strings for multiple instances of the same module.
		 * @param definition Comma separated definition string.
		 * @param id_ofs Offset for the 'id'.
		 * @return True on success.
		 */
		bool setup(const std::string& definition, id_type id_ofs = 0);

		/**
		 * @brief Creates a variable using the definition structure.
		 * @param definition Definition structure.
		 * @param id_ofs Offset for the 'id'.
		 * @return True on success.
		 */
		bool setup(const Definition& definition, Variable::id_type id_ofs = 0);

		/**
		 * @brief Sets reference to other variable.
		 * This is the only way for local variables.
		 * @param v
		 * @return
		 */
		bool setup(const Variable& v);

		/**
		 * @brief Sets reference to other by id.
		 *
		 * The global list of references is searched for the passed id and referenced when found.
		 * When found event #veIdChanged is emitted.
		 * When the desired id was set and changed the #veDesiredId event is emitted.
		 * Cannot be used for local variables.
		 * @param id Identifier.
		 * @param set_did When true the desired id is set to the passed 'id'.
		 * @return True when found and referenced.
		 */
		bool setup(id_type id, bool set_did = false);

		/**
		 * @brief Only for local non exported owning variables to set the id after setup is called using a string.
		 *
		 * The id can not be zero and attached local variables are notify with a #veNewId event.
		 * @param id Variable identifier
		 * @param skip_self When false the event to its own handler is skipped to be emitted.
		 * @return True on success.
		 */
		bool setId(id_type id, bool skip_self = false);

		/**
		 * @brief Checks validity of the attached reference and used after a setup call.
		 *
		 * @return True if the variable is valid.
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * @brief Returns true is the current instance is converting floating point values.
		 * @return True when converting.
		 */
		[[nodiscard]] bool isConverted() const;

		/**
		 * @brief Enables or disables conversion of floating point values.
		 *
		 * If a change was triggered a #veConverted event was emitted.
		 * @param enable To enable pass 'true'. to disable 'false'.
		 * @return Value returned by #isConverted().
		 */
		bool setConvert(bool enable);

		/**
		 * @brief Returns whether the value is a number or not.
		 *
		 * Result is true when the value type is either Value::vitFloat or Value::vitInteger.
		 * @return True if the current variable value is a number.
		 */
		[[nodiscard]] bool isNumber() const;

		/**
		 * @brief Makes this instance owner of this variable instance and emits the appropriate events.
		 *
		 * When the owner really changes instances the new owner handler receives
		 * a #veGetOwner event and the previous one #veLostOwner.
		 */
		void makeOwner();

		/**
		 * @brief Gets a reference to the owner/server instance of this variable.
		 *
		 * @return Owner reference.
		 */
		Variable& getOwner();

		/**
		 * @brief Returns if this is the owner of this variable.
		 * @return True when owner.
		 */
		[[nodiscard]] bool isOwner() const;

		/**
		 * @brief Returns true if this instance is a global variable or not.
		 *
		 * @return True when global.
		 */
		[[nodiscard]] bool isGlobal() const;

		/**
		 * @brief Sets the variable to be global.
		 *
		 * This can only happen if it attached to Zero Variable so when the id is zero.
		 * @param global True when global and False when local.
		 * @return True on success.
		 */
		bool setGlobal(bool global);

		/**
		 * @brief Makes an owning local variable appear globally.
		 *
		 * Useful when reading/creating instances from stored files.
		 * First as local instances and then making them global.
		 * @param global True to export as global and False to revert it.
		 * @return True on success.
		 */
		bool setExport(bool global);

		/**
		 * @brief Checks if the variable is an exported local variable.
		 *
		 * @return True if exported.
		 */
		[[nodiscard]] bool isExported() const;

		/**
		 * @brief Checks if a temporary is used instead of current actual value.
		 * @return True when temporary is enabled.
		 */
		bool isTemporary();

		/**
		 * @brief When true is passed a local temporary value is used instead of current.
		 *
		 * @param on_off
		 */
		void setTemporary(bool on_off);

		/**
		 * @brief Applies the temporary value of this instance.
		 *
		 * Used mainly in dialogs to apply the changed value to become the actual current one.
		 * The 'skip_self' flag is set to true by default because an event was already sent using #setCur().
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when the value was actually a change.
		 */
		bool applyTemporary(bool skip_self = true);

		/**
		 * @brief Updates the temporary value with the real value.
		 *
		 * Used mainly in dialogs to update the temporary value with real/actual value.
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when a change was caused.
		 */
		bool updateTemporary(bool skip_self = false);

		/**
		 * @brief Check if the temporary value is differs from the actual value.
		 *
		 * Used mainly in dialogs to enable an apply button.
		 * @return True when different.
		 */
		[[nodiscard]] bool isTemporaryDifferent() const;

		/**
		 * @brief Only one link is available at a time.
		 *
		 * Sets a event handler for this variable, passing NULL wil disable the link.
		 * @param handler
		 */
		void setHandler(VariableHandler* handler);

		/**
		 * @brief Returns the link Set by setHandler().
		 *
		 * @return Link instance pointer
		 */
		[[nodiscard]] VariableHandler* getHandler() const;

		/**
		 * @brief Special handler for converting float variables globally.
		 *
		 * Sets a event handler for this variable, passing NULL wil disable the handler
		 * @param handler Handler from conversion interface.
		 */
		static void setConvertHandler(VariableHandler* handler);

		/**
		 * @brief Returns the link to the global conversion handler.
		 */
		[[nodiscard]] VariableHandler* getConvertHandler() const;

		/**
		 * @brief Initiate event for all instances of this variables depending on the event value.
		 *
		 * Returns the amount of effected variables by this call.
		 * @param event
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return Amount of events emitted.
		 */
		Variable::size_type emitEvent(EEvent event, bool skip_self = false);

		/**
		 * @brief Sets the desired id member variable to the current attached reference id.
		 */
		void setDesiredId();

		/**
		 * @brief Sets the desired id member variable to the passed id.
		 *
		 * If the passed 'id' is zero the automatic attachment mechanism is disabled.
		 */
		void setDesiredId(id_type id);

		/**
		 * @brief Sets the data for this instance for user purposes.
		 * @param data Could be a pointer casted value.
		 */
		void setData(uint64_t data);

		/**
		 * @brief Gets the data for this instance for user purposes Set with setData().
		 * @return Could be a pointer casted value.
		 */
		[[nodiscard]] uint64_t getData() const;

		/**
		 * @brief Both return true if there was a change and notify all variables of the same variable id through an event.
		 *
		 * @param value Sets a new current value.
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when the value was actually changed.
		 */
		bool setCur(const Value& value, bool skip_self = false);

		/**
		 * @brief Same as #setCur() but for const instances objects of this instance.
		 *
		 * @param value Sets a new current value.
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when the value was actually changed.
		 */
		[[nodiscard]] bool setCur(const Value& value, bool skip_self = false) const;

		/**
		 * @brief Used in settings loading routines which use the owner to Set a new value.
		 *
		 * Checks if clients have write access and is global before applying the new value.
		 * @param value
		 * @return True on success of making a change.
		 */
		[[nodiscard]] bool loadCur(const Value& value) const;

		/**
		 * @brief Increase current variable value by step increments also negative values.
		 *
		 * Emits event #veValueChange when a change is flags occurred.
		 * @param steps Amount of increments (>0) or decrements (<0)
		 * @param skip_self Skip event on this instance.
		 * @return True if this function made a difference.
		 */
		bool increase(int steps, bool skip_self = false);

		/**
		 * @brief Unsets a flag or multiple flags of the attached VariableReference.
		 *
		 * Emits event #veValueChange when a change in flags occurred.
		 * @param flag Single or multiple values of #EFlag.
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when a change was made.
		 */
		bool unsetFlag(flags_type flag, bool skip_self = false);

		/**
		 * @brief Sets a flag or multiple flags on the reference.
		 *
		 * Only for an owner can call this function successful.
		 * Emits event #veFlagsChange when a change in flags occurred.
		 * @param flag Single or multiple values of #EFlag.
		 * @param skip_self Skip event on this instance.
		 * @return True when a change was made.
		 */
		bool setFlag(flags_type flag, bool skip_self = false);

		/**
		 * @brief Replaces all flags with the passed flags.
		 *
		 * Only for an owner can call this function successful.
		 * Emits event #veFlagsChange when a change in flags occurred.
		 * @param flags Single or multiple values of #EFlag.
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when a change was made.
		 */
		bool updateFlags(flags_type flags, bool skip_self = false);

		/**
		 * @brief Returns the flags at the time the instance was created.
		 *
		 * @return Set of #EFlag bits.
		 */
		[[nodiscard]] flags_type getFlags() const;

		/**
		 * @brief Returns the current flags for this instance.
		 *
		 * @return Set of #EFlag bits.
		 */
		[[nodiscard]] flags_type getCurFlags() const;

		/**
		 * @brief Sets the global conversion values and signals the clients.
		 *
		 * Only with owners a call is successful and effective.<br>
		 * The formula is: <b><i>converted = (current * multiplier) + offset</i></b>
		 * When successful event #veConverted emitted.
		 * @param unit New Unit string.
		 * @param multiplier Multiplier value.
		 * @param offset Offset value.
		 * @param digits Significant digits for the new unit.
		 * @return
		 */
		bool setConvertValues(const std::string& unit, const Value& multiplier, const Value& offset, int digits = std::numeric_limits<int>::max());

		/**
		 * @brief Enables or disables unit conversion.
		 *
		 * Only owners can make this call successfully.<br>
		 * Depending on the globally set handler using #setConvertHandler() this function calls the handler with event
		 * #veConvert which then should apply the conversion values using #setConvertValues() on the 'call_var' passed.<br>
		 * When the global conversion handler has not been set the
		 * Makes a call to the unit conversion interface and tries to get conversion values from it.
		 * <br>
		 * When the global conversion handler not set it uses the locally available conversion values.
		 * @param convert When true conversion is enabled and false disabled.
		 * @return True on success of getting the values.
		 */
		bool setConvertValues(bool convert = true);

		/**
		 * @brief Returns the current attached ID.
		 *
		 * @return Id of the current attached variable.
		 */
		[[nodiscard]] id_type getId() const;

		/**
		 * @brief Returns the desired id of this instance.
		 *
		 * @return Desired id.
		 */
		[[nodiscard]] id_type getDesiredId() const;

		/**
		 * @brief Gets the name or a part of the name of the attached variable.
		 *
		 * Returns the variable name which is default (level 0) the full variable path name.
		 * When levels is 'n' and larger then zero the last 'n' levels are returned.
		 * When levels is 'n' and smaller then zero the first 'n' levels are omitted.
		 * @param levels Amount of required levels.
		 * @return Full or part of name depending on level.
		 */
		[[nodiscard]] std::string getName(int levels = 0) const;

		/**
		 * @brief Returns the amount of levels of the full name path.
		 *
		 * @return Total name levels available.
		 */
		[[nodiscard]] int getNameLevelCount() const;

		/**
		 * @brief Returns variable unit
		 *
		 * @return Unit string.
		 */
		[[nodiscard]] std::string getUnit() const;

		/**
		 * @brief Returns variable unit of converted or non converted value.
		 *
		 * @param converted True when getting the converted value.
		 * @return Unit string.
		 */
		[[nodiscard]] std::string getUnit(bool converted) const;

		/**
		 * @deprecated
		 * @brief Returns the conversion option string.
		 *
		 * This string determines how the value is to be converted when exporting.
		 * @return Option string.
		 */
		[[nodiscard]] std::string getConvertOption() const;

		/**
		 * @brief The type is determined by the character in the unit field of the setup string.
		 *
		 * @return Enumerate value of #EStringType
		 */
		[[nodiscard]] EStringType getStringType() const;

		/**
		 * @brief Gets the name for passed string type.
		 *
		 * @param type Enumerate value.
		 * @return Name of the string type.
		 */
		static const char* getStringType(EStringType type);

		/**
		 * @brief Gets the purpose description of the attached variable.
		 *
		 * @return Description string.
		 */
		[[nodiscard]] std::string getDescription() const;

		/**
		 * @brief Returns if a flag or flags has been set.
		 *
		 * @param flag Single value or a multiple of #EFlag values.
		 * @return True when the flag(s) were set.
		 */
		[[nodiscard]] bool isFlag(int flag) const;

		/**
		 * @brief Returns the current flags in a std::string form.
		 *
		 * @return Flags as a string.
		 */
		[[nodiscard]] std::string getCurFlagsString() const;

		/**
		 * Returns the flags at setup in a std::string form.
		 *
		 * @return Flags as a string.
		 */
		[[nodiscard]] std::string getFlagsString() const;

		/**
		 * @brief Returns if variable is allowed to change its value.
		 *
		 * @return True when readonly.
		 */
		[[nodiscard]] bool isReadOnly() const;

		/**
		 * @brief Gets the significant digits for this instance.
		 * @return Amount of digits.
		 */
		[[nodiscard]] int getSigDigits() const;

		/**
		 * @brief Gets the significant digits for this instance converted or not converted.
		 *
		 * @param converted True when needing the converted amount.
		 * @return Amount of digits.
		 */
		[[nodiscard]] int getSigDigits(bool converted) const;

		/**
		 * @brief Return the variable default value.
		 *
		 * Is converted when converted flag is true.
		 * @return
		 */
		[[nodiscard]] const Value& getDef() const;

		/**
		 * @brief Return the variable Minimum value.
		 *
		 * Is converted when converted flag is true.
		 */
		[[nodiscard]] const Value& getMin() const;

		/**
		 * @brief Return the variable Maximum value.
		 *
		 * Is converted when converted flag is true.
		 */
		[[nodiscard]] const Value& getMax() const;

		/**
		 * @brief Return the variable Rounding value.
		 *
		 * Converted when converted flag is true.
		 */
		[[nodiscard]] const Value& getRnd() const;

		/**
		 * @brief Return the variable current value.
		 *
		 * Is converted when converted flag is true.
		 * @return Temporary value when it is used.
		 */
		[[nodiscard]] const Value& getCur() const;

		/**
		 * @brief Gets the default value converted or not
		 * Is converted independent of the conversion flag.
		 * @param converted True when getting the converted value.
		 */
		[[nodiscard]] const Value& getDef(bool converted) const;

		/**
		 * @brief Gets the Minimum value converted or not
		 * converted independent of the conversion flag.
		 * @param converted True when getting the converted value.
		 */
		[[nodiscard]] const Value& getMin(bool converted) const;

		/**
		 * @brief Gets the Maximum value converted or not
		 * converted independent of the conversion flag.
		 * @param converted True when getting the converted value.
		 * @return
		 */
		[[nodiscard]] const Value& getMax(bool converted) const;

		/**
		 * @brief Gets the Rounding value converted or not
		 * converted independent of the conversion flag.
		 * @param converted True when getting the converted value.
		 * @return
		 */
		[[nodiscard]] const Value& getRnd(bool converted) const;

		/**
		 * @brief Gets the current value converted or not
		 * converted independent of the conversion flag.
		 * @param converted True when getting the converted value.
		 * @return Does not return the temporary value when it is used.
		 */
		//
		[[nodiscard]] const Value& getCur(bool converted) const;

		/**
		 * @brief Returns the complete state vector as a reference.
		 */
		[[nodiscard]] const State::Vector& getStates() const;

		/**
		 * @brief Returns the usage count of this variable reference
		 * @return
		 */
		[[nodiscard]] size_type getUsageCount() const;

		/**
		 * @brief Returns the state count.
		 *
		 * @return Amount of states.
		 */
		[[nodiscard]] size_type getStateCount() const;

		/**
		 * @brief Returns the state index of passed value when it exist.
		 *
		 * @param v Value to lookup.
		 * @return Valid index and when it does not exist it returns #npos.
		 */
		[[nodiscard]] size_type getState(const Value& v) const;

		/**
		 * @brief Return the display name of the passed state's index.
		 *
		 * @param state Index of the state.
		 * @return Display name.
		 */
		[[nodiscard]] std::string getStateName(size_type state) const;

		/**
		 * @brief Return the value of the passed state index.
		 *
		 * @param state Index of the state.
		 * @return Value of the passed state index.
		 */
		[[nodiscard]] const Value& getStateValue(size_type state) const;

		/**
		 * @brief Returns enumerate value of the passed string.
		 *
		 * @param type
		 * @return
		 */
		static Value::EType getType(const char* type);

		/**
		 * @brief Returns type string of given enumerate value.
		 *
		 * @param type
		 * @return
		 */
		static const char* getType(Value::EType type);

		/**
		 * @brief Returns type enumerate of this variable instance.
		 */
		[[nodiscard]] Value::EType getType() const;

		/**
		 * @brief Gets the amount of variables having different ID's in the system.
		 *
		 * @return Amount of instances.
		 */
		static size_type getCount();

		/**
		 * @brief Gets the total amount of variable instances in the system.
		 *
		 * @param global_only When true, count only the global ones.
		 * @return Amount of instances.
		 */
		static size_type getInstanceCount(bool global_only = true);

		/**
		 * @brief Retrieves a list of instances available (global + exported).
		 *
		 * Useful in populating a selection dialog.
		 * @return Vector of instance pointers.
		 */
		static Vector getList();

		/**
		 * @brief Gets the name of the passed field enumerate #EField.
		 */
		static std::string getFieldName(int field);

		/**
		 * @brief Returns variable with the given id.
		 *
		 * When not found it returns the zero variable.
		 * @param id Variable id to seek.
		 * @return When not found always zero variable.
		 */
		static const Variable& getInstanceById(id_type id);

		/**
		 * @brief Returns variable with the given id.
		 *
		 * When not found it returns the zero variable.
		 * @param id Variable id to seek.
		 * @param list Vector to use for lookup.
		 * @return When not found always zero variable.
		 */
		static Variable& getInstanceById(id_type id, Vector& list);

		/**
		 * @brief Returns variable with the given id.
		 *
		 * When not found it returns the zero variable.
		 * @param id Variable id to seek.
		 * @param list Vector to use for lookup.
		 * @return When not found always zero variable.
		 */
		static const Variable& getInstanceById(id_type id, const Vector& list);

		/**
		 * @brief Assignment operator that attaches this instance to the same VariableReference as 'v'.
		 */
		Variable& operator=(const Variable& v);

		/**
		 * @brief Comparison operator.
		 */
		int operator==(const Variable& v) const;

		/**
		 * @brief Returns current converted value in default formatted string or state.
		 *
		 * When a temporary value is used that value is returned.
		 * @param states Determines if a state string name is to be used.
		 * @return String of the value.
		 */
		[[nodiscard]] std::string getCurString(bool states = true) const;

		/**
		 * @brief Gets the setup std::string for this variable.
		 */
		[[nodiscard]] std::string getSetupString() const;

		/**
		 * @brief Clips the passed value between min and max values.
		 *
		 * @param value Value to be clipped.
		 */
		void clipRound(Value& value) const;

		/**
		 * @brief Returns the passed value converted to new units or back again.
		 *
		 * @param value Value to be converted.
		 * @param to_org Direction of conversion.
		 * @return Converted value.
		 */
		[[nodiscard]] Value convert(const Value& value, bool to_org = false) const;

		/**
		 * @brief Writes id and current value and flags to the stream.
		 */
		bool writeUpdate(std::ostream& os) const;

		/**
		 * @brief load single current value and flags from stream.
		 *
		 * If 'list' is other then the default that list is used to seek the according variable instead of the global list.
		 * @param is
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @param list
		 * @return
		 */
		static bool readUpdate(std::istream& is, bool skip_self = false, Vector& list = null_ref<Vector>());

		/**
		 * Writes id and current value to the stream.
		 * @param os
		 * @return
		 */
		bool write(std::ostream& os) const;

		/**
		 * Fills a definition structure from a string.
		 * On failure the _valid field is set to false.
		 * @param str Definition string
		 * @return Definition structure
		 */
		static Definition getDefinition(const std::string& str);

		/**
		 * load single current values from stream.
		 * If 'list' is other then a NULL_REF that list is used to seek the according variable instead of the global list.
		 * @param is
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @param list
		 * @return
		 */
		static bool read(std::istream& is, bool skip_self = false, Vector& list = null_ref<Vector>());

		/**
		 * Reads multiple variable setup strings from stream separated by newline characters.
		 * Returns true when no error occurred during the process.
		 * On error returns false and line returns the error line in the stream.
		 * If 'list' is other then a NULL_REF the created variables are added to that list.
		 * @param is
		 * @param list
		 * @param global
		 * @param err_line
		 * @return
		 */
		static bool create(std::istream& is, Vector& list = null_ref<Vector>(),
			bool global = true, int& err_line = null_ref<int>());

		/**
		 * Converts a std::string with flag characters to an integer
		 * @param flags
		 * @return
		 */
		static Variable::flags_type toFlags(const std::string& flags);

		/**
		 * Returns the passed flags in a std::string form
		 * @param flags
		 * @return
		 */
		static std::string getFlagsString(flags_type flags);

		/**
		 * Returns the event name to make debugging easier.
		 */
		static const char* getEventName(EEvent event);

		/**
		 * During events no instances should be deleted.
		 * The destructor will be called but the data still exist
		 */
		void operator delete(void*); // NOLINT(misc-new-delete-overloads)

	protected:
		/**
		 * @brief Creates global or local variable instance depending on the value passed.
		 * @param global True when global, false when local.
		 */
		explicit Variable(bool global);

	private:
		/**
		 * @brief Zero variable constructor only.
		 */
		explicit Variable(void*, void*);

		/**
		 * @brief Updates the original non converted version of this instance.
		 *
		 * @param value
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return True when changed.
		 */
		bool updateValue(const Value& value, bool skip_self);

		/**
		 * @brief Updates the temporary value of this instance.
		 * @return True when changed.
		 */
		bool updateTempValue(const Value& value, bool skip_self);

		/**
		 * @brief Initiate event for all instances of this variable if params was Set by setHandler this param is ignored.
		 *
		 * @param event
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return Amount of events sent.
		 */
		size_type emitLocalEvent(EEvent event, bool skip_self = true);

		/**
		 * @brief Initiate event for all variable instances if param was Set by setHandler this past param is used
		 *
		 * @param event
		 * @param skip_self When 'true' this instance is skipped in emission of events.
		 * @return Amount of events sent.
		 */
		size_type emitGlobalEvent(EEvent event, bool skip_self = true);

		/**
		 * @brief Function which checks the link before calling it.
		 *
		 * This is the only function which calls the Link handler directly.
		 * @param ev
		 * @param caller
		 */
		void emitEvent(EEvent ev, const Variable& caller);

		/**
		 * @brief Get a reference by ID if not exist
		 *
		 * @param id
		 * @return Reference from passed id.
		 */
		static VariableReference* getReferenceById(id_type id);

		/**
		 * @brief Removes the passed link from any variables.
		 *
		 * @param handler
		 */
		static void removeHandler(VariableHandler* handler);

		/**
		 * @brief Private function to attach variable references.
		 *
		 * @param ref
		 * @return True when successful.
		 */
		bool attachRef(VariableReference* ref);

		/**
		 * @brief Attaches all instances that have the desired id Set as this one.
		 *
		 * @return Amount of events sent.
		 */
		size_type attachDesired();

		/**
		 * @brief Holds the data to the reference caring the common data.
		 */
		VariableReference* _reference{nullptr};
		/**
		 * @brief Holds the instance data for user purposes Set with setData() and got with getData().
		 */
		uint64_t _data{0};
		/**
		 * @brief When this flag is true the float values must be converted if possible.
		 */
		bool _converted{false};
		/**
		 * @brief Hold when this pointer is non null this value is changed instead of the current.
		 */
		Value* _temporary{nullptr};
		/**
		 * @brief Pointer to object with variableEventHandler function.
		 */
		VariableHandler* _handler{nullptr};
		/**
		 * @brief When this data member is non-zero it will be automatically re-attached to the reference when it is created.
		 */
		id_type _desiredId{0};
		/**
		 * @brief When true this instance is global and is by default set to true in the constructor.
		 */
		bool _global{false};

		friend class VariableHandler;

		friend class VariableReference;

		friend class VariableStatic;
};

inline
Variable::Variable(const Variable& v)
{
	_global = v.isGlobal();
	setup(v);
}

inline
Variable::Variable(const Definition& def, id_type id_ofs)
{
	_global = true;
	setup(def, id_ofs);
}

inline
Variable::Variable(Variable::id_type id, bool set_desired)
{
	_global = true;
	setup(id, set_desired);
}

inline
bool Variable::setup(const std::string& definition, Variable::id_type id_ofs)
{
	return setup(getDefinition(definition), id_ofs);
}

inline
Variable::Variable(const std::string& definition, Variable::id_type id_ofs)
{
	_global = true;
	setup(definition, id_ofs);
}

inline
bool Variable::setup(const Variable& v)
{
	setDesiredId(v._desiredId);
	return attachRef(v._reference);
}

inline
bool Variable::setup(Variable::id_type id, bool set_did)
{
	if (set_did)
	{
		setDesiredId(id);
	}
	// When non-global the reference members are copied.
	return attachRef(getReferenceById(id));
}

inline
Variable& Variable::operator=(const Variable& v)
{
	if (this != &v)
	{
		setup(v);
	}
	return *this;
}

inline
Variable::id_type Variable::getDesiredId() const
{
	return _desiredId;
}

inline
bool Variable::setCur(const Value& value, bool skip_self) const
{
	// Cast this pointer to non-const class.
	return const_cast<Variable*>(this)->setCur(value, skip_self);
}

inline
bool Variable::isGlobal() const
{
	return _global;
}

inline
Value::EType Variable::getType(const char* type)
{
	return Value::getType(type);
}

inline
const char* Variable::getType(Value::EType type)
{
	return Value::getType(type);
}

inline
const Variable& Variable::getInstanceById(Variable::id_type id, const Vector& list)
{
	return Variable::getInstanceById(id, (Vector&) list);
}

inline
void Variable::setData(uint64_t data)
{
	_data = data;
}

inline
uint64_t Variable::getData() const
{
	return _data;
}

inline
VariableHandler* Variable::getHandler() const
{
	return _handler;
}

inline
int Variable::operator==(const Variable& v) const
{
	return v._reference == _reference;
}

/**
 * Class std::ostream operator
 */
_GII_FUNC std::ostream& operator<<(std::ostream& os, const Variable& v);

/**
 * @brief Stream operator for the setup std::string.
 */
_GII_FUNC std::istream& operator>>(std::istream& is, Variable& v);

/**
 * @brief Stream operator for setting up this instance with a setup std::string.
 */
_GII_FUNC std::ostream& operator<<(std::ostream& os, const Variable::State::Vector& v);

}
