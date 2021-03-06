/**
 * General interface to controls.
 * This class contains the name, unit and value of a variable in the system.
 * It also contains the default, maximum, minimum, current and round values.
 * These values are stored in a reference class.
 * It has several triggers on changes to fields of a reference instance
*/

#pragma once

#include "VariableBase.h"

#define NULL_REF(r) (*((r*)NULL))

namespace sf
{

/**
 * Class referenced by the TVariable class which carries the actual data.
 * This type is never directly used.
 */
struct TVariableReference :public TVariableTypes
{
	/**
	 * Constructor when global is true the reference is added to the global list.
	 * @param global
	 */
	explicit TVariableReference(bool global);

	/**
	 * Destructor.
	 */
	~TVariableReference() override;

	/**
	 * Copies the members to this instance except for the List and Global members.
	 * @param ref
	 */
	void Copy(const TVariableReference& ref);

	/** Holds the flag about the global status of this reference.*/
	bool Global{false};
	/** Holds the exported flag for local applications.*/
	bool Exported{false};
	/** Holds the valid status of this reference.*/
	bool Valid{false};
	/** In case of a global instance this value is a unique ID.*/
	id_type Id{0};
	/** Holds the flags at creation.*/
	flags_type Flags{0};
	/** Holds the current flags.*/
	flags_type CurFlags{0};
	/** Holds the internal type of the variable.*/
	TValue::EType Type{TValue::vitUNDEF};
	/** Name path separated by '|' characters.*/
	std::string Name;
	/** Describes the*/
	std::string Descr;
	/** Contains the SI unit or the string filter.*/
	std::string Unit;
	/** Current value.*/
	TValue CurVal;
	/** Default value.*/
	TValue DefVal;
	/** Maximum value.*/
	TValue MaxVal;
	/** Minimum value.*/
	TValue MinVal;
	/** Rounding value.*/
	TValue RndVal;
	/** Significant digits based on the round value.*/
	int SigDigits{0};
	/** Vector which holds all states of this instance.*/
	TStateArray StateArray;
	/** List of variables attached to this reference.*/
	TPtrVector List;
	/** Static this counter is increased if a local event is generated.<br>
	 * It is decreased when it returns from the event handler.*/
	int LocalActive;
	/** Conversion option.*/
	std::string CnvOption;
	/** Converted unit.
	 * String length of unit is used as a flag for conversion enabling.*/
	std::string CnvUnit;
	/** Converted current value.*/
	TValue CnvCurVal;
	/** Converted default value.*/
	TValue CnvDefVal;
	/** Converted maximum value.*/
	TValue CnvMaxVal;
	/** Converted minimum value.*/
	TValue CnvMinVal;
	/** Converted rounding value.*/
	TValue CnvRndVal;
	/** Significant digits after conversion.*/
	int CnvSigDigits{0};
	/** Multiplication value for convert calculation.*/
	TValue CnvMult;
	/** Offset value for convert calculation.*/
	TValue CnvOffset;

	friend class TVariableBase;

	friend class TVariable;
};

/**
 * @brief Class for creating and referencing global or local created parameters or settings called variables.<br>
 * This class allows linking of member functions of other classes to handle events generated.<br>
 * See the [Example](sf-gii-variable.html) on how to use this class and the 'TVariableLinkHandler' template class.<br>
 */
class TVariable :public TVariableBase
{
	public:
		// Default constructor for a default global variable.
		TVariable();

		// Copy constructor. Copies also the Desired ID data member.
		TVariable(const TVariable& v);

		// Creates an instance having a reference with 'id'.
		TVariable(id_type id, bool set_did);

		// Creates a variable according to the definition std::string passed to it.
		// When 'setdid' is true the 'DesiredID' data member is set to 'id'.
		explicit TVariable(const std::string& definition);

		explicit TVariable(const char*);

		// Same as above but the id from the string is used with the passed ID offset.
		TVariable(const std::string& definition, id_type id_ofs);

		// Creates a variable using the passed definition structure.
		explicit TVariable(const TDefinition* def);

		// Virtual destructor.
		~TVariable() override;

		// Creates a variable according to the definition std::string passed to it.
		bool Setup(const std::string& definition);

		// Same as above but the id from the string is used with the passed ID offset.
		bool Setup(const std::string& definition, id_type id_ofs);

		// Creates a variable using the definition structure.
		bool Setup(const TDefinition* def);

		// Sets reference to other variable. Only way for local variables.
		bool Setup(const TVariable& v);

		// Sets reference to other variable by id. The global list of references is
		// searched for the ID. Cannot be used for local variables.
		// When 'set_did' is true the 'DesiredID' data member is set to 'id'.
		bool Setup(id_type id, bool set_did = false);

		// Only for local non exported owning variables to set the ID after Setup
		// is called using a string. The ID must can not be zero.
		// Attached local variables are notify with a new ID event.
		// Returns true onm success.
		bool SetId(id_type id, bool skip_self = false);

		// returns if the variable is valid meaning if it
		[[nodiscard]] bool IsValid() const;

		// Return true is the current instance is converting float values.
		[[nodiscard]] bool IsConverted() const;

		// Enables or disables returns the value returned by ISConverted().
		bool SetConvert(bool enable);

		// Return true if the current attach reference is nummerical value.
		[[nodiscard]] bool IsNumber() const;

		// Make this instance owner of this variable
		void MakeOwner();

		// Return a pointer to the owner variable.
		TVariable& GetOwner();

		// check if this is the owner of this variable
		[[nodiscard]] bool IsOwner() const;

		// Returns true if the variable is global.
		[[nodiscard]] bool IsGlobal() const;

		// Sets the variable to be global. This can only happen if it attached
		// to Zero Variable or the current id is zero. Returns true on succes.
		bool SetGlobal(bool global);

		// Function to make an owning local variable appear globally or revert
		// the process by passing 'false'.
		// Returns true on success.
		bool SetExport(bool global);

		// Returns true if the variable is an exported local variable.
		[[nodiscard]] bool IsExported() const;

		//
		// Functions for controls in for property sheets.
		//
		// Returns true when temporary is used instead of current value.
		bool IsTemporary();

		// When true is passed a local temporary value is used instead of current.
		void SetTemporary(bool onoff);

		// When skipself is true this instance is skipped in the event process.
		// Returns true when the value was actually changed.
		bool ApplyTemporary(bool skipself = false);

		// Updates the temporary value with the real value.
		bool UpdateTemporary(bool skipself = false);

		// Returns true if the Temporary value is different as the real value.
		[[nodiscard]] bool TemporaryDifs() const;

		// Only one link is available at a time.
		// Sets a event handler for this variable, passing NULL wil disable the link
		void SetLink(TVariableLink* link);

		[[nodiscard]] TVariableLink* GetLink() const {return FLink;}

		// Special hooked handler for converting float variables globally.
		// Sets a event handeler for this variable, passing NULL wil disable the link
		void SetConvertLink(TVariableLink* link);

		[[nodiscard]] TVariableLink* GetConvertLink() const {return FConvertLink;}

		// Initiate event for all instances of this variables depending on
		// the event value. Returns the amount of effected variables by this call.
		unsigned VariableEvent(EEvent event, bool skipself = false);

		//
		//  Information manipulation functions
		//
		// Sets the desired id member variable to the current reference id.
		void SetDesiredId();

		// Sets the desired id member variable to the passed id.
		// If the passed 'id' is zero the automatic attachment mechanism is disabled.
		void SetDesiredId(id_type id);

		// Sets the data for this instance for user purposes.
		void SetData(uint32_t data);

		// Gets the data for this instance for user purposes set with SetData().
		[[nodiscard]] uint32_t GetData() const;

		// Both return true if there was a change and notify all variables
		// of the same variable id through an event.
		// When skip_self is 'true' this instance is skipped in the list.
		// Returns true when the value was actually changed.
		bool SetCur(const TValue& value, bool skip_self = false);

		// Making this function also accessible for const objects of this instance.
		[[nodiscard]] bool SetCur(const TValue& value, bool skipself = false) const;

		// Used in settings loading routines which use the owner to set a new value.
		// Checks if clients have write access and is global before applying
		// the new value. Returns true on success of making a change.
		[[nodiscard]] bool LoadCur(const TValue& value) const;

		// Increase current variable value by step increments also negative values
		// Returns true if this function made a difference.
		bool Increase(int steps, bool skip_self = false);

		// Unsets a flag or multiple flags of the attached TVariableReference.
		void UnsetFlag(int flag, bool skip_self = false);

		// Sets a flag or multiple flags on the reference
		void SetFlag(int flag, bool skip_self = false);

		// Clears and updates the flags, only for owner
		void UpdateFlags(flags_type flag, bool skip_self = false);

		// Returns the flags at the time the instance was created.
		[[nodiscard]] flags_type GetFlags() const;

		// Returns the current flags for this instance.
		[[nodiscard]] flags_type GetCurFlags() const;

		// Sets the global conversion values and signals the clients.
		// Only with owners this function has any effect.
		bool SetConvertValues(const std::string& unit, const TValue& mult,
			const TValue& ofs, int digits = INT_MAX);

		// Makes a call to the unit conversion interface and tries to get
		// conversion values from it. Returns true on succes of getting the values.
		// Only with owners this function has any effect.
		// When false passed the convert values are disabled.
		bool SetConvertValues(bool = true);

		// Returns the current attached ID.
		[[nodiscard]] id_type GetId() const;

		// Returns the DesiredId data member value.
		[[nodiscard]] id_type GetDesiredId() const;

		// Returns the variable name which is default the full variable path name.
		// When levels is 'n' and larger then zero the last 'n' levels are returned.
		// When levels is 'n' and smaller then zero the first 'n' levels are ommited.
		[[nodiscard]] std::string GetName(int levels = 0) const;

		// Returns the amount of levels of the full name path.
		[[nodiscard]] int GetNameLevelCount() const;

		// Returns variable unit
		[[nodiscard]] std::string GetUnit() const;

		// Returns variable unit of converted or non converted value.
		[[nodiscard]] std::string GetUnit(bool converted) const;

		// Returns the conversion option string.
		[[nodiscard]] std::string GetConvertOption() const;

		// Returns the type of the string type instance.
		// The type is determined by the character in the unit field of the setup string.
		[[nodiscard]] EStringType GetStringType() const;

		// Return the unit for a specific string type.
		static const char* GetStringType(EStringType st);

		// Returns variable description
		[[nodiscard]] std::string GetDescr() const;

		// Flag related functions
		// Returns true checks if a certain flag is set or combination of flags are all set.
		[[nodiscard]] bool IsFlag(int flag) const;

		// Returns the current flags in a std::string form
		[[nodiscard]] std::string GetCurFlagsString() const;

		// Returns the original setup flags in a std::string form
		[[nodiscard]] std::string GetFlagsString() const;

		// Returns true if variable can not change its value.
		[[nodiscard]] bool IsReadOnly() const;

		// Gets the significant digits for this instance.
		[[nodiscard]] int GetSigDigits() const;

		// Gets the significant digits for this instance.
		[[nodiscard]] int GetSigDigits(bool converted) const;

		// Functions returning the variables, current, default, minimum,
		// maximum and rounding values. Are all converted when converted flag is true.
		[[nodiscard]] const TValue& GetDef() const;

		[[nodiscard]] const TValue& GetMin() const;

		[[nodiscard]] const TValue& GetMax() const;

		[[nodiscard]] const TValue& GetRnd() const;

		// Returns the temporary value when it is used.
		[[nodiscard]] const TValue& GetCur() const;

		// Used to get values converted or non converted independent of
		// the conversion flag.
		[[nodiscard]] const TValue& GetDef(bool converted) const;

		[[nodiscard]] const TValue& GetMin(bool converted) const;

		[[nodiscard]] const TValue& GetMax(bool converted) const;

		[[nodiscard]] const TValue& GetRnd(bool converted) const;

		// Does not return the temporary value when it is used.
		[[nodiscard]] const TValue& GetCur(bool converted) const;

		// Returns the complete state vector as a reference.
		[[nodiscard]] const TStateVector& GetStates() const;

		// Returns the usage count of this variable reference
		[[nodiscard]] unsigned GetUsageCount() const;

		// Returns the state count.
		[[nodiscard]] unsigned GetStateCount() const;

		// Returns the state index.
		[[nodiscard]] unsigned GetState(const TValue& v) const;

		// Return the state itself
		[[nodiscard]] std::string GetStateName(unsigned state) const;

		[[nodiscard]] const TValue& GetStateValue(unsigned state) const;

		// Returns enumerate value of the passed string.
		static TValue::EType GetType(const char* typestr);

		// Returns type string of given enumerate value.
		static const char* GetType(TValue::EType type);

		// Returns type enumerate of this variable instance.
		[[nodiscard]] TValue::EType GetType() const;

		// Returns the amount of variables having different ID's in the system.
		static unsigned GetVariableCount();

		// Returns the total amount of variable instances in the system.
		static unsigned GetInstanceCount();

		// Returns the 'n' item in the static variable list.
		// It returns always a pointer to the owner.
		static const TVariable* GetVariableListItem(unsigned p);

		// Returns variable with the given id on error it returns the zero variable.
		static const TVariable& GetVariableById(id_type id);

		// Finds variable in vector with passed ID. If not it returns the zero variable.
		static TVariable& GetVariableById(id_type id, TPtrVector& list);

		static const TVariable& GetVariableById(id_type id, const TPtrVector& list);

		// Assignment operator that attaches this instance to the same TVariableReference as 'v'.
		TVariable& operator=(const TVariable& v);

		// Comparison operator
		int operator==(const TVariable& v) const;

		// Returns current converted value in default formatted string or state.
		// When a temporary value is used that value is returned.
		[[nodiscard]] std::string GetCurString(bool states = true) const;

		// Gets the setup std::string for this variable
		[[nodiscard]] std::string GetSetupString() const;

		// Clips the passed value between min and max values.
		void ClipRound(TValue& value) const;

		// Returns the passed value converted to new units or back again.
		[[nodiscard]] TValue Convert(const TValue& value, bool to_org = false) const;

		// Writes id and current value and flags to the stream.
		bool WriteUpdate(std::ostream& os) const;

		// Read single current value and flags from stream.
		// If 'list' is other then a NULL_REF that list is used to seek the
		// according variable instead of the global list.
		static bool ReadUpdate(std::istream& is, bool skip_self = false, TPtrVector& list = NULL_REF(TPtrVector));

		// Writes id and current value to the stream.
		bool Write(std::ostream& os) const;

		// Read single current values from stream.
		// If 'list' is other then a NULL_REF that list is used to seek the
		// according variable instead of the global list.
		static bool Read(std::istream& is, bool skip_self = false, TPtrVector& list = NULL_REF(TPtrVector));

		// Reads multiple variable setup strings from stream separated by newline
		// characters. Returns true when no error occurred during the process.
		// On error returns false and line returns the error line in the stream.
		// If 'list' is other then a NULL_REF the created variables are added to that list
		static bool
		Create(std::istream& is, TPtrVector& list = NULL_REF(TPtrVector), bool global = true, int& errline = NULL_REF(int));

		// Converts a std::string with flag characters to an integer
		static int StringToFlags(const char* flags);

		// Returns the passed flags in a std::string form
		static std::string GetFlagsString(flags_type flags);

		// During events no instances should be deleted
		// The destructor will be called but the data still exists
		void operator delete(void*);

	protected:
		/**
		 * Creates global or local variable instance depending on the value passed.
		 * @param global
		 */
		explicit TVariable(bool global);

	private:
		/**
		 * Initializes the class members of TVariable
		 * @param global
		 */
		void InitMembers(bool global);

		/**
		 * Updates the original real version of this instance.
		 * @param value
		 * @param skip_self
		 * @return
		 */
		bool UpdateValue(const TValue& value, bool skip_self);

		bool UpdateTempValue(const TValue& value, bool skipself);

		/**
		 * Initiate event for all instances of this variable
		 * if params was set by SetLink this param is ignored
		 * @param event
		 * @param skipself
		 * @return
		 */
		unsigned LocalEvent(EEvent event, bool skipself = true);

		/**
		 * initiate event for all variable instances if param was set by SetLink this past param is used
		 * @param event
		 * @param skipself
		 * @return
		 */
		unsigned GlobalEvent(EEvent event, bool skipself = true);

		/**
		 * Function which checks the link before calling it.
		 * This is the only function which calls the Link handler directly.
		 * @param ev
		 * @param caller
		 */
		void NotifyVariable(EEvent ev, const TVariable& caller);

		/**
		 * Get a reference by ID if not exist
		 * @param id
		 * @return
		 */
		static TVariableReference* GetVarRefById(id_type  id);

		/**
		 * Removes the passed link from any variables.
		 * @param link
		 */
		static void RemoveLink(TVariableLink* link);

		/**
		 * Private function to attach variable references.
		 * @param ref
		 * @return
		 */
		bool AttachRef(TVariableReference* ref);

		/**
		 * Attaches all instances that have the desired id set as this one.
		 * @return
		 */
		unsigned AttachDesired();

		/**
		 * Holds the data to the reference caring the common data.
		 */
		TVariableReference* FRef{nullptr};
		/**
		 * Holds the instance data for user purposes set with SetData() and got with GetData().
		 */
		uint32_t FData{0};
		/**
		 * If flag is true the float values must be converted if possible.
		 */
		bool FConverted{false};
		/**
		 * Hold when this pointer is non null this value is changed instead of the current.
		 */
		TValue* FTemporary{nullptr};
		/**
		 * Pointer to object with VariableEventHandler function.
		 */
		TVariableLink* FLink{nullptr};
		/**
		 * If this data member is non-zero it will be automatically re-attached to
		 * the reference when it is created.
		 */
		id_type FDesiredId{0};
		/**
		 * This instance is a global variable and is default 'true' for TVariable.
		 */
		bool FGlobal{false};
		/**
		 * Holds the link to the global conversion handler.
		 */
		static TVariableLink* FConvertLink;

		friend class TVariableLink;

		friend class TVariableReference;

		friend class TLocalVariable;
};

inline
TVariable::TVariable(const TVariable& v)
{
	InitMembers(v.IsGlobal());
	Setup(v);
}

inline
TVariable::TVariable(const TDefinition* def)
{
	InitMembers(true);
	Setup(def);
}

inline
TVariable::TVariable(TVariable::id_type id, bool set_did)
{
	InitMembers(true);
	Setup(id, set_did);
}

inline
TVariable::TVariable(const std::string& definition)
{
	InitMembers(true);
	Setup(definition, 0);
}

inline
TVariable::TVariable(const char* definition)
{
	InitMembers(true);
	Setup(definition, 0);
}

inline
TVariable::TVariable(const std::string& definition, TVariable::id_type id_ofs)
{
	InitMembers(true);
	Setup(definition, id_ofs);
}

inline
bool TVariable::Setup(const TVariable& v)
{
	auto* p = &v;
	SetDesiredId(p ? v.FDesiredId : 0);
	return AttachRef(p ? v.FRef : nullptr);
}

inline
bool TVariable::Setup(TVariable::id_type id, bool set_did)
{
	if (set_did)
	{
		SetDesiredId(id);
	}
	// When non global the reference members are copied.
	return AttachRef(GetVarRefById(id));
}

inline
bool TVariable::Setup(const std::string& definition)
{
	return Setup(definition, 0L);
}

inline
TVariable& TVariable::operator=(const TVariable& v)
{
	if (this != &v)
		Setup(v);
	return *this;
}

inline
TVariable::id_type TVariable::GetDesiredId() const
{
	return FDesiredId;
}

inline
void TVariable::SetDesiredId()
{
	SetDesiredId(FRef->Id);
}

inline
bool TVariable::SetCur(const TValue& value, bool skipself) const
{
	// Cast this pointer to non-const class.
	return ((TVariable*) this)->SetCur(value, skipself);
}

inline
TVariable& TVariable::GetOwner()
{
	return *FRef->List[0];
}

inline
bool TVariable::IsOwner() const
{
	return this == FRef->List[0];
}

inline
bool TVariable::IsValid() const
{
	return FRef && FRef->Valid;
}

inline
bool TVariable::IsGlobal() const
{
	return FGlobal;
}

inline
bool TVariable::IsNumber() const
{
	return FRef->Type == TValue::vitFLOAT || FRef->Type == TValue::vitINTEGER;
}

inline
TVariable::id_type TVariable::GetId() const
{
	return FRef->Id;
}

inline
bool TVariable::IsConverted() const
{
	return
		FConverted &&
			FRef->Type == TValue::vitFLOAT &&
			FRef->CnvUnit.length();
}

inline
bool TVariable::IsTemporary()
{
	return FTemporary != nullptr;
}

inline
std::string TVariable::GetDescr() const
{
	return FRef->Descr;
}

inline
bool TVariable::IsFlag(int flag) const
{
	return (FRef->CurFlags & flag) == flag;
}

inline
std::string TVariable::GetCurFlagsString() const
{
	return GetFlagsString(FRef->CurFlags);
}

inline
std::string TVariable::GetFlagsString() const
{
	return TVariable::GetFlagsString(FRef->Flags);
}

inline
TVariable::flags_type TVariable::GetFlags() const
{
	return FRef->Flags;
}

inline
TVariable::flags_type TVariable::GetCurFlags() const
{
	return FRef->CurFlags;
}

inline
const TValue& TVariable::GetDef() const
{
	return IsConverted() ? FRef->CnvDefVal : FRef->DefVal;
}

inline
const TValue& TVariable::GetMin() const
{
	return IsConverted() ? FRef->CnvMinVal : FRef->MinVal;
}

inline
const TValue& TVariable::GetMax() const
{
	return IsConverted() ? FRef->CnvMaxVal : FRef->MaxVal;
}

inline
const TValue& TVariable::GetRnd() const
{
	return IsConverted() ? FRef->CnvRndVal : FRef->RndVal;
}

inline
int TVariable::GetSigDigits() const
{
	return IsConverted() ? FRef->CnvSigDigits : FRef->SigDigits;
}

inline
const TValue& TVariable::GetDef(bool converted) const
{
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvDefVal : FRef->DefVal;
}

inline
const TValue& TVariable::GetMin(bool converted) const
{
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvMinVal : FRef->MinVal;
}

inline
const TValue& TVariable::GetMax(bool converted) const
{
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvMaxVal : FRef->MaxVal;
}

inline
const TValue& TVariable::GetRnd(bool converted) const
{
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvRndVal : FRef->RndVal;
}

inline
int TVariable::GetSigDigits(bool converted) const
{
	return (converted && FRef->CnvUnit.length()) ? FRef->CnvSigDigits : FRef->SigDigits;
}

inline
TValue::EType TVariable::GetType(const char* typestr)
{
	return TValue::GetType(typestr);
}

inline
const char* TVariable::GetType(TValue::EType type)
{
	return TValue::GetType(type);
}

inline
TValue::EType TVariable::GetType() const
{
	return FRef->Type;
}

inline
const TVariable& TVariable::GetVariableById(TVariable::id_type  id)
{
	return *(GetVarRefById(id)->List[0]);
}

inline
const TVariable& TVariable::GetVariableById(TVariable::id_type id, const TPtrVector& list)
{
	return TVariable::GetVariableById(id, (TPtrVector&) list);
}

inline
unsigned TVariable::GetUsageCount() const
{
	return FRef->List.Count();
}

inline
unsigned TVariable::GetStateCount() const
{
	return FRef->StateArray.Count();
}

inline
const TVariable::TStateVector& TVariable::GetStates() const
{
	return FRef->StateArray;
}

inline
void TVariable::SetData(uint32_t data)
{
	FData = data;
}

inline
uint32_t TVariable::GetData() const
{
	return FData;
}

inline
int TVariable::operator==(const TVariable& v) const
{
	return v.FRef == FRef;
}

/**
 * Class std::ostream operator
 */
_GII_FUNC  std::ostream& operator<<(std::ostream& os, const TVariable& v);

/**
 * Class std::istream operator
 */
_GII_FUNC std::istream& operator>>(std::istream& is, TVariable& v);

/**
 * Base class used for giving a TVariable instance access to a member functions
 * of a derived class. This class is not used directly and is used a base class
 * for the TVariableLinkHandler<T> template class.
 */
class _GII_CLASS TVariableLink :public TVariableTypes
{
	protected:

		/**
		 * Pure virtual function which must be implemented when used in a polymorphic setting.
		 */
		virtual void VariableEventHandler
			(
				EEvent /*event*/,
				const TVariable& /*callvar*/,
				TVariable&/*linkvar*/,
				bool /*sameinst*/
			) = 0;

		/**
		 * this destructor clears the link with variable instances
		 * so no errors occur when the link is destructed before
		 * the variable is.
		 */
		~TVariableLink() override
		{
			TVariable::RemoveLink(this);
		}

		friend class TVariableCommon;

		friend class TVariable;
};

/**
 * Template for linking pointers of member function to TVariable instances.
 * @tparam T
 */
template<class T>
class TVariableLinkHandler :public TVariableLink
{
	public:
		/**
		 * Required event handler type.
		 */
		typedef void (T::*TPmf)(EEvent, const TVariable&, TVariable&, bool);

		/**
		 * Constructor for assigning the pointer of the member function.
		 */
		inline
		TVariableLinkHandler(T* _this, TPmf pmf)
			:This(_this)
			 , Pmf(pmf) {}

		/**
		 * Prevent copying by not implementing copy constructor.
		 */
		TVariableLinkHandler(const TVariableLinkHandler&) = delete;

		/**
		 * Prevent copying by not implementing assignment operator.
		 */
		TVariableLinkHandler& operator=(const TVariableLink&) = delete;

	private:
		/**
		 * void pointer to member function.
		 */
		TPmf Pmf;
		/**
		 * void pointer to class instance.
		 */
		T* This;

		/**
		 * Call the member function through virtual overloaded function from the base class.
		 *
		 * @param event
		 * @param call_var
		 * @param link_var
		 * @param same_inst
		 */
		void VariableEventHandler
			(
				EEvent event,
				const TVariable& call_var,
				TVariable& link_var,
				bool same_inst
			) override
		{
			(This->*Pmf)(event, call_var, link_var, same_inst);
		}
};

}
