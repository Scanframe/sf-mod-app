/**
 * General interface to controls.
 * This class contains the name, unit and value of a variable in the system.
 * It also contains the default, maximum, minimum, current and round values.
 * These values are stored in a reference class.
 * It has several triggers on changes to fields of a reference instance
*/

#pragma once

#include "VariableHandler.h"

namespace sf
{

/**
 * @brief Class for creating and referencing global or local created parameters or settings called variables.<br>
 * This class allows linking of member functions of other classes to handle events generated.<br>
 * See the [Example](sf-gii-variable.html) on how to use this class and the 'TVariableHandler' template class.<br>
 */
class _GII_CLASS Variable :public InformationBase, public VariableTypes
{
	public:
		/**
		 * Must be called to initialize the statics which this class depends on.
		 */
		static void initialize() noexcept;
		/**
		 * Called when terminating the application.
		 */
		static void deinitialize() noexcept;

		/**
		 * Default constructor for a default global variable.
		 */
		Variable();

		/**
		 * Copy constructor.
		 * Copies also the Desired ID data member.
		 */
		Variable(const Variable& v);

		/**
		 * Creates an instance having a reference with 'id'.
		 * @param id
		 * @param set_did
		 */
		Variable(id_type id, bool set_did);

		/**
		 * Creates a variable according to the definition std::string passed to it.
		 * @param definition
		 */
		explicit Variable(const std::string& definition);

		/**
		 * same as above but the id from the string is used with the passed id offset.
		 * @param definition
		 * @param id_ofs
		 */
		Variable(const std::string& definition, id_type id_ofs);

		/**
		 * creates a variable using the passed definition structure.
		 * @param def
		 */
		explicit Variable(const Definition* def);

		/**
		 * Virtual destructor.
		 */
		~Variable() override;

		/**
		 * creates a variable according to the definition std::string passed to it.
		 * @param definition
		 * @return
		 */
		bool setup(const std::string& definition);

		/**
		 * same as above but the id from the string is used with the passed id offset.
		 * @param definition
		 * @param id_ofs
		 * @return
		 */
		bool setup(const std::string& definition, id_type id_ofs);

		/**
		 * creates a variable using the definition structure.
		 * @param definition
		 * @return
		 */
		bool setup(const Definition* definition);

		/**
		 * sets reference to other variable. only way for local variables.
		 * @param v
		 * @return
		 */
		bool setup(const Variable& v);

		/**
		 * Sets reference to other variable by id.
		 * The global list of references is searched for the ID.
		 * Cannot be used for local variables.
		 * When 'set_did' is true the 'DesiredID' data member is Set to 'id'.
		 * @param id
		 * @param set_did
		 * @return
		 */
		bool setup(id_type id, bool set_did = false);

		/**
		 * only for local non exported owning variables to Set the id after setup
		 * is called using a string. The ID must can not be zero.
		 * Attached local variables are notify with a new ID event.
		 * @param id
		 * @param skip_self
		 * @return True onm success.
		 */
		bool setId(id_type id, bool skip_self = false);

		/**
		 * returns true if the variable is valid meaning if it has an id assigned to it.
		 * @return
		 */
		[[nodiscard]] bool isValid() const;

		/**
		 * Return true is the current instance is converting float values.
		 * @return
		 */
		[[nodiscard]] bool isConverted() const;

		/**
		 * Enables or disables returns the value returned by ISConverted().
		 * @param enable
		 * @return
		 */
		bool setConvert(bool enable);

		/**
		 * return true if the current attach reference is a number value.
		 * @return
		 */
		[[nodiscard]] bool isNumber() const;

		/**
		 * Makes this instance owner of this variable.
		 */
		void makeOwner();

		/**
		 * Return a reference to the owner variable.
		 * @return
		 */
		Variable& getOwner();

		/**
		 * Checks if this is the owner of this variable
		 * @return
		 */
		[[nodiscard]] bool isOwner() const;

		/**
		 * returns true if the variable is global.
		 * @return
		 */
		[[nodiscard]] bool isGlobal() const;

		/**
		 * Sets the variable to be global. this can only happen if it attached
		 * to Zero Variable or the current id is zero. Returns true on success.
		 * @param global
		 * @return
		 */
		bool setGlobal(bool global);

		/**
		 * Function to make an owning local variable appear globally or revert the process by passing 'false'.
		 * @param global
		 * @return True on success.
		 */
		bool setExport(bool global);

		/**
		 * Returns true if the variable is an exported local variable.
		 * @return
		 */
		[[nodiscard]] bool isExported() const;

		/**
		 * Returns true when temporary is used instead of current value.
		 * @return
		 */
		bool isTemporary();

		/**
		 * When true is passed a local temporary value is used instead of current.
		 * @param on_off
		 */
		void setTemporary(bool on_off);

		/**
		 * When skip_self is true this instance is skipped in the event process.
		 * @param skip_self
		 * @return True when the value was actually changed.
		 */
		bool applyTemporary(bool skip_self = false);

		/**
		 * Updates the temporary value with the real value.
		 * @param skip_self
		 * @return
		 */
		bool updateTemporary(bool skip_self = false);

		/**
		 * Returns true if the Temporary value is different as the real value.
		 * @return
		 */
		[[nodiscard]] bool temporaryDiffs() const;

		/**
		 * Only one link is available at a time.
		 * Sets a event handler for this variable, passing NULL wil disable the link
		 * @param handler
		 */
		void setHandler(VariableHandler* handler);

		/**
		 * Returns the link Set by setHandler().
		 * @return Link instance pointer
		 */
		[[nodiscard]] VariableHandler* getHandler() const;

		/**
		 * Special hooked handler for converting float variables globally.
		 * Sets a event handler for this variable, passing NULL wil disable the link
		 * @param link
		 */
		void setConvertHandler(VariableHandler* link);

		/**
		 * Returns the link to the global conversion handler.
		 * @return
		 */
		[[nodiscard]] VariableHandler* getConvertHandler() const;

		/**
		 * Initiate event for all instances of this variables depending on the event value.
		 * Returns the amount of effected variables by this call.
		 * @param event
		 * @param skip_self
		 * @return
		 */
		Variable::size_type emitEvent(EEvent event, bool skip_self = false);

		/**
		 * Sets the desired id member variable to the current reference id.
		 */
		void setDesiredId();

		/**
		 * Sets the desired id member variable to the passed id.
		 * If the passed 'id' is zero the automatic attachment mechanism is disabled.
		 * @param id
		 */
		void setDesiredId(id_type id);

		/**
		 * Sets the data for this instance for user purposes.
		 * @param data
		 */
		void setData(uint64_t data);

		/**
		 * Gets the data for this instance for user purposes Set with setData().
		 * @return
		 */
		[[nodiscard]] uint64_t getData() const;

		/**
		 * Both return true if there was a change and notify all variables of the same variable id through an event.
		 * When skip_self is 'true' this instance is skipped in the list.
		 * Returns true when the value was actually changed.
		 * @param value
		 * @param skip_self
		 * @return
		 */
		bool setCur(const Value& value, bool skip_self = false);

		/**
		 * Making this function also accessible for const objects of this instance.
		 * @param value
		 * @param skip_self
		 * @return
		 */
		[[nodiscard]] bool setCur(const Value& value, bool skip_self = false) const;

		/**
		 * Used in settings loading routines which use the owner to Set a new value.
		 * Checks if clients have write access and is global before applying the new value.
		 * @param value
		 * @return True on success of making a change.
		 */
		[[nodiscard]] bool loadCur(const Value& value) const;

		/**
		 * Increase current variable value by step increments also negative values
		 * @param steps Amount of increments (>0) or decrements (<0)
		 * @param skip_self Skip event on this instance.
		 * @return True if this function made a difference.
		 */
		bool increase(int steps, bool skip_self = false);

		/**
		 * Unsets a flag or multiple flags of the attached VariableReference.
		 * @param flag
		 * @param skip_self
		 */
		void unsetFlag(int flag, bool skip_self = false);

		/**
		 * Sets a flag or multiple flags on the reference.
		 * @param flag
		 * @param skip_self
		 */
		void setFlag(int flag, bool skip_self = false);

		/**
		 * Clears and updates the flags, only for owner
		 * @param flag
		 * @param skip_self
		 */
		void updateFlags(flags_type flag, bool skip_self = false);

		/**
		 * Returns the flags at the time the instance was created.
		 * @return Set of flag bits.
		 */
		[[nodiscard]] flags_type getFlags() const;

		/**
		 * Returns the current flags for this instance.
		 * @return
		 */
		[[nodiscard]] flags_type getCurFlags() const;

		/**
		 * Sets the global conversion values and signals the clients.
		 * Only with owners this function has any effect.
		 * @param unit
		 * @param multiplier
		 * @param offset
		 * @param digits
		 * @return
		 */
		bool setConvertValues(const std::string& unit, const Value& multiplier,
			const Value& offset, int digits = std::numeric_limits<int>::max());

		/**
		 * Makes a call to the unit conversion interface and tries to get conversion values from it.
		 * Only with owners this function has any effect.
		 * When false passed the convert values are disabled.
		 * @return True on success of getting the values.
		 */
		bool setConvertValues(bool = true);

		/**
		 * Returns the current attached ID.
		 * @return
		 */
		[[nodiscard]] id_type getId() const;

		/**
		 * Returns the DesiredId data member value.
		 * @return
		 */
		[[nodiscard]] id_type getDesiredId() const;

		/**
		 * Returns the variable name which is default the full variable path name.
		 * When levels is 'n' and larger then zero the last 'n' levels are returned.
		 * When levels is 'n' and smaller then zero the first 'n' levels are omitted.
		 * @param levels
		 * @return
		 */
		[[nodiscard]] std::string getName(int levels = 0) const;

		/**
		 * Returns the amount of levels of the full name path.
		 * @return
		 */
		[[nodiscard]] int getNameLevelCount() const;

		/**
		 * Returns variable unit
		 * @return
		 */
		[[nodiscard]] std::string getUnit() const;

		/**
		 * Returns variable unit of converted or non converted value.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] std::string getUnit(bool converted) const;

		/**
		 * Returns the conversion option string.
		 * @return
		 */
		[[nodiscard]] std::string getConvertOption() const;

		/**
		 * The type is determined by the character in the unit field of the setup string.
		 * @return Enumerate value
		 */
		[[nodiscard]] EStringType getStringType() const;

		/**
		 * Return the unit for a specific string type.
		 * @param str
		 * @return
		 */
		static const char* getStringType(EStringType str);

		/**
		 * Returns the purpose description of the variable.
		 * @return String
		 */
		[[nodiscard]] std::string getDescription() const;

		/**
		 * Returns true checks if a certain flag is set or combination of flags are all Set.
		 * @param flag
		 * @return
		 */
		[[nodiscard]] bool isFlag(int flag) const;

		/**
		 * Returns the current flags in a std::string form
		 * @return
		 */
		[[nodiscard]] std::string getCurFlagsString() const;

		/**
		 * Returns the original setup flags in a std::string form
		 * @return
		 */
		[[nodiscard]] std::string getFlagsString() const;

		/**
		 * Returns true if variable can not change its value.
		 * @return
		 */
		[[nodiscard]] bool isReadOnly() const;

		/**
		 * Gets the significant digits for this instance.
		 * @return
		 */
		[[nodiscard]] int getSigDigits() const;

		/**
		 * Gets the significant digits for this instance.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] int getSigDigits(bool converted) const;

		/**
		 * Return the variable default value.
		 * Converted when converted flag is true.
		 * @return
		 */
		[[nodiscard]] const Value& getDef() const;

		/**
		 * Return the variable Minimum value.
		 * Converted when converted flag is true.
		 * @return
		 */
		[[nodiscard]] const Value& getMin() const;

		/**
		 * Return the variable Maximum value.
		 * Converted when converted flag is true.
		 * @return
		 */
		[[nodiscard]] const Value& getMax() const;

		/**
		 * Return the variable Rounding value.
		 * Converted when converted flag is true.
		 * @return
		 */
		[[nodiscard]] const Value& getRnd() const;

		/**
		 * Return the variable current value.
		 * Converted when converted flag is true.
		 * @return Temporary value when it is used.
		 */
		[[nodiscard]] const Value& getCur() const;

		/**
		 * Gets the default value converted or not
		 * converted independent of the conversion flag.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] const Value& getDef(bool converted) const;

		/**
		 * Gets the Minimum value converted or not
		 * converted independent of the conversion flag.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] const Value& getMin(bool converted) const;

		/**
		 * Gets the Maximum value converted or not
		 * converted independent of the conversion flag.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] const Value& getMax(bool converted) const;

		/**
		 * Gets the Rounding value converted or not
		 * converted independent of the conversion flag.
		 * @param converted
		 * @return
		 */
		[[nodiscard]] const Value& getRnd(bool converted) const;

		/**
		 * Gets the current value converted or not
		 * converted independent of the conversion flag.
		 * @param converted
		 * @return Does not return the temporary value when it is used.
		 */
		//
		[[nodiscard]] const Value& getCur(bool converted) const;

		/**
		 * Returns the complete state vector as a reference.
		 * @return
		 */
		[[nodiscard]] const StateVector& getStates() const;

		/**
		 * Returns the usage count of this variable reference
		 * @return
		 */
		[[nodiscard]] size_type getUsageCount() const;

		/**
		 * Returns the state count.
		 * @return
		 */
		[[nodiscard]] size_type getStateCount() const;

		/**
		 * Returns the state index.
		 * @param v
		 * @return
		 */
		[[nodiscard]] size_type getState(const Value& v) const;

		/**
		 * Return the name of the passed state index.
		 * @param state
		 * @return
		 */
		[[nodiscard]] std::string getStateName(size_type state) const;

		/**
		 * Return the value of the passed state index.
		 * @param state
		 * @return
		 */
		[[nodiscard]] const Value& getStateValue(size_type state) const;

		/**
		 * Returns enumerate value of the passed string.
		 * @param type
		 * @return
		 */
		static Value::EType getType(const char* type);

		/**
		 * Returns type string of given enumerate value.
		 * @param type
		 * @return
		 */
		static const char* getType(Value::EType type);

		/**
		 * Returns type enumerate of this variable instance.
		 * @see Value::EType
		 * @return
		 */
		[[nodiscard]] Value::EType getType() const;

		/**
		 * Returns the amount of variables having different ID's in the system.
		 * @return
		 */
		static size_type getVariableCount();

		/**
		 * Returns the total amount of variable instances in the system.
		 * @return
		 */
		static size_type getInstanceCount();

		/**
		 * Returns the 'n' item in the static variable list.
		 * It returns always a pointer to the owner.
		 * @param p
		 * @return
		 */
		static const Variable* getVariableListItem(size_type p);

		/**
		 * Returns variable with the given id.
		 * When not found it returns the zero variable.
		 * @param id
		 * @return
		 */
		static const Variable& getVariableById(id_type id);

		/**
		 * Returns variable with the given id.
		 * When not found it returns the zero variable.
		 * @param id
		 * @param list
		 * @return
		 */
		static Variable& getVariableById(id_type id, Vector& list);

		/**
		 * Returns variable with the given id.
		 * When not found it returns the zero variable.
		 * @param id
		 * @param list
		 * @return
		 */
		static const Variable& getVariableById(id_type id, const Vector& list);

		/**
		 * Assignment operator that attaches this instance to the same VariableReference as 'v'.
		 * @param v
		 * @return
		 */
		Variable& operator=(const Variable& v);

		/**
		 * Comparison operator
		 * @param v
		 * @return
		 */
		int operator==(const Variable& v) const;

		/**
		 * Returns current converted value in default formatted string or state.
		 * When a temporary value is used that value is returned.
		 * @param states
		 * @return
		 */
		[[nodiscard]] std::string getCurString(bool states = true) const;

		/**
		 * Gets the setup std::string for this variable
		 * @return
		 */
		[[nodiscard]] std::string getSetupString() const;

		/**
		 * Clips the passed value between min and max values.
		 * @param value
		 */
		void clipRound(Value& value) const;

		/**
		 * Returns the passed value converted to new units or back again.
		 * @param value
		 * @param to_org
		 * @return
		 */
		[[nodiscard]] Value convert(const Value& value, bool to_org = false) const;

		/**
		 * Writes id and current value and flags to the stream.
		 * @param os
		 * @return
		 */
		bool writeUpdate(std::ostream& os) const;

		/**
		 * Read single current value and flags from stream.
		 * If 'list' is other then the default that list is used to seek the according variable instead of the global list.
		 * @param is
		 * @param skip_self
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
		 * Read single current values from stream.
		 * If 'list' is other then a NULL_REF that list is used to seek the according variable instead of the global list.
		 * @param is
		 * @param skip_self
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
		static int toFlags(const char* flags);

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
		 * The destructor will be called but the data still exists
		 */
		void operator delete(void*); // NOLINT(misc-new-delete-overloads)

	protected:
		/**
		 * Creates global or local variable instance depending on the value passed.
		 * @param global
		 */
		explicit Variable(bool global);

	private:
		/**
		 * Zero variable constructor.
		 */
		explicit Variable(void*, void*);

		/**
		 * Updates the original real version of this instance.
		 * @param value
		 * @param skip_self
		 * @return True when changed.
		 */
		bool updateValue(const Value& value, bool skip_self);

		/**
		 * Updates the temporary value of this instance.
		 * @return True when changed.
		 */
		bool updateTempValue(const Value& value, bool skip_self);

		/**
		 * Initiate event for all instances of this variable
		 * if params was Set by setHandler this param is ignored
		 * @param event
		 * @param skip_self
		 * @return Amount of events sent.
		 */
		size_type emitLocalEvent(EEvent event, bool skip_self = true);

		/**
		 * Initiate event for all variable instances if param was Set by setHandler this past param is used
		 * @param event
		 * @param skip_self
		 * @return Amount of events sent.
		 */
		size_type emitGlobalEvent(EEvent event, bool skip_self = true);

		/**
		 * Function which checks the link before calling it.
		 * This is the only function which calls the Link handler directly.
		 * @param ev
		 * @param caller
		 */
		void emitEvent(EEvent ev, const Variable& caller);

		/**
		 * Get a reference by ID if not exist
		 * @param id
		 * @return Reference from passed id.
		 */
		static VariableReference* getReferenceById(id_type id);

		/**
		 * Removes the passed link from any variables.
		 * @param handler
		 */
		static void removeHandler(VariableHandler* handler);

		/**
		 * Private function to attach variable references.
		 * @param ref
		 * @return True when successful.
		 */
		bool attachRef(VariableReference* ref);

		/**
		 * Attaches all instances that have the desired id Set as this one.
		 * @return Amount of events sent.
		 */
		size_type attachDesired();

		/**
		 * Holds the data to the reference caring the common data.
		 */
		VariableReference* _reference{nullptr};
		/**
		 * Holds the instance data for user purposes Set with setData() and got with getData().
		 */
		uint64_t _data{0};
		/**
		 * If flag is true the float values must be converted if possible.
		 */
		bool _converted{false};
		/**
		 * Hold when this pointer is non null this value is changed instead of the current.
		 */
		Value* _temporary{nullptr};
		/**
		 * Pointer to object with VariableEventHandler function.
		 */
		VariableHandler* _handler{nullptr};
		/**
		 * If this data member is non-zero it will be automatically re-attached to
		 * the reference when it is created.
		 */
		id_type _desiredId{0};
		/**
		 * This instance is a global variable and is default 'true' for Variable.
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
Variable::Variable(const Definition* def)
{
	_global = true;
	setup(def);
}

inline
Variable::Variable(Variable::id_type id, bool set_did)
{
	_global = true;
	setup(id, set_did);
}

inline
Variable::Variable(const std::string& definition)
{
	_global = true;
	setup(definition, 0);
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
	auto* p = &v;
	setDesiredId(p ? v._desiredId : 0);
	return attachRef(p ? v._reference : nullptr);
}

inline
bool Variable::setup(Variable::id_type id, bool set_did)
{
	if (set_did)
	{
		setDesiredId(id);
	}
	// When non global the reference members are copied.
	return attachRef(getReferenceById(id));
}

inline
bool Variable::setup(const std::string& definition)
{
	return setup(definition, 0L);
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
const Variable& Variable::getVariableById(Variable::id_type id, const Vector& list)
{
	return Variable::getVariableById(id, (Vector&) list);
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
_GII_FUNC  std::ostream& operator<<(std::ostream& os, const Variable& v);

/**
 * Class std::istream operator
 */
_GII_FUNC std::istream& operator>>(std::istream& is, Variable& v);

/**
 * Class std::ostream operator
 */
_GII_FUNC  std::ostream& operator<<(std::ostream& os, const Variable::StateVector& v);

}
