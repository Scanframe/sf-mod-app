#include "InformationScript.h"

namespace sf
{

// Speed index defines
#define SID_GII_SCRIPT_START 0
#define SID_VARS            (SID_GII_SCRIPT_START +  0)
#define SID_TIME            (SID_GII_SCRIPT_START +  1)
#define SID_DATE            (SID_GII_SCRIPT_START +  2)
#define SID_ON_CHANGE       (SID_GII_SCRIPT_START +  3)
#define SID_VAR_IMPORT      (SID_GII_SCRIPT_START +  4)
#define SID_VAR_EXPORT      (SID_GII_SCRIPT_START +  5)
#define SID_SETFLAGS        (SID_GII_SCRIPT_START +  6)
#define SID_SETFLAG         (SID_GII_SCRIPT_START +  7)
#define SID_UNSETFLAG       (SID_GII_SCRIPT_START +  8)
#define SID_FUNCTIONENTRY   (SID_GII_SCRIPT_START +  9)
#define SID_CREATEVAR       (SID_GII_SCRIPT_START + 10)
#define SID_VARIABLE        (SID_GII_SCRIPT_START + 11)
#define SID_RESULTDATA      (SID_GII_SCRIPT_START + 12)
#define SID_VARSCRIPT_STOP  (SID_GII_SCRIPT_START + 13)

// Var param object members.
#define SID_VAR_SETUP       1
#define SID_VAR_ID          2
#define SID_VAR_NAME        3
#define SID_VAR_UNIT        4
#define SID_VAR_CUR         5
#define SID_VAR_DEF         6
#define SID_VAR_RND         7
#define SID_VAR_MIN         8
#define SID_VAR_MAX         9
#define SID_VAR_FLAGS      10
#define SID_VAR_ISFLAGS    11
#define SID_VAR_SETFLAGS   12
#define SID_VAR_UNSETFLAGS 13
#define SID_VAR_CURSTR     14
// Variable script object
#define SID_VAR_ON_VALUE   50
#define SID_VAR_ON_FLAGS   51
#define SID_VAR_ON_ID      52
#define SID_VAR_SKIPEVENT  53

// ResultData object members.
#define SID_RES_ID          1
#define SID_RES_NAME        2
#define SID_RES_FLAGS       3
#define SID_RES_OFFSET      4
#define SID_RES_RANGE       5
#define SID_RES_DATA        6
#define SID_RES_BLOCKCOUNT  7
#define SID_RES_REQUEST     8
// Events
#define SID_RES_ON_ID       50
#define SID_RES_ON_ACCESS   51
#define SID_RES_ON_CLEAR    52
#define SID_RES_ON_GOTRANGE 53

/**
 * Script object for exporting GII parameters to a script object.
 */
class _GII_CLASS TVarParamScriptObject :public ScriptObject, public InformationTypes
{
	public:
		// Constructor for export.
		TVarParamScriptObject
			(
				ScriptObject* parent, // Object owning this object.
				const char* name,      // Name in the script to access the parameter.
				const Variable* var,  // Attached variable.
				int sid = 0            // Speed index.
			);

		// Virtual destructor as base class.
		~TVarParamScriptObject() override;

		/**
		 * Gets the info of this parameter.
		 */
		[[nodiscard]] const IdInfo& Info() const
		{
			return FInfo;
		}

	protected:
		/**
		 * Constructor.
		 */
		TVarParamScriptObject(ScriptObject* parent, const char* type_name, const char* name, bool flagowner);

		/**
		 * Overloaded from base class.
		 */
		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		/**
		 * Overloaded from base class TScriptObject.
		 */
		void destroyObject(bool& should_delete) override
		{
			should_delete = false;
		}

		// Overloaded from base class.
		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flagset) override;

		// Holds the parent object.
		ScriptObject* FParent;
		// Variable containing the information of the parameter.
		const Variable* FVar;
		// Info structure which is filled in to return.
		IdInfo FInfo;
		// Skips event for this instance.
		bool FSkipEvent;
};

ScriptObject::IdInfo VarParamInfo[] =
	{
		{SID_VAR_SETUP, InformationScript::idFunction, "Setup", 1, nullptr},
		{SID_VAR_ID, InformationScript::idConstant, "Id", 0, nullptr},
		{SID_VAR_NAME, InformationScript::idFunction, "Name", 1, nullptr},
		{SID_VAR_UNIT, InformationScript::idConstant, "Unit", 0, nullptr},
		{SID_VAR_CUR, InformationScript::idVariable, "Cur", 0, nullptr},
		{SID_VAR_CURSTR, InformationScript::idConstant, "CurStr", 0, nullptr},
		{SID_VAR_RND, InformationScript::idConstant, "Rnd", 0, nullptr},
		{SID_VAR_DEF, InformationScript::idConstant, "Def", 0, nullptr},
		{SID_VAR_MIN, InformationScript::idConstant, "Min", 0, nullptr},
		{SID_VAR_MAX, InformationScript::idConstant, "Max", 0, nullptr},
		{SID_VAR_FLAGS, InformationScript::idVariable, "Flags", 0, nullptr},
		{SID_VAR_ISFLAGS, InformationScript::idFunction, "IsFlags", 1, nullptr},
		{SID_VAR_SETFLAGS, InformationScript::idFunction, "SetFlags", 1, nullptr},
		{SID_VAR_UNSETFLAGS, InformationScript::idFunction, "UnsetFlags", 1, nullptr},
	};

const ScriptObject::IdInfo* TVarParamScriptObject::getInfo(const std::string& name) const
{
	for (auto& i: VarParamInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<TVarParamScriptObject*>(this);
			return &i;
		}
	}
	return getInfoUnknown();
}

TVarParamScriptObject::TVarParamScriptObject(ScriptObject* parent, const char* name, const Variable* var, int sid)
	:ScriptObject("VarParam")
	 , FParent(parent)
	 , FVar(var)
	 , FSkipEvent(false)
{
	// Fill in the info structure for the passed variable.
	FInfo._id = idConstant;
	FInfo._name = strdup(name);
	FInfo._data = this;
	FInfo._paramCount = 0;
	FInfo._index = sid;
}

TVarParamScriptObject::TVarParamScriptObject(ScriptObject* parent, const char* type_name, const char* name,
	bool flagowner)
	:ScriptObject(type_name)
	 , FParent(parent)
	 , FVar(nullptr)
	 , FSkipEvent(true)
{
	// Fill in the info structure for the passed variable.
	FInfo._id = idVariable;
	FInfo._name = strdup(name);
	FInfo._data = this;
	FInfo._paramCount = 0;
	FInfo._index = flagowner ? SID_VAR_EXPORT : SID_VAR_IMPORT;
}

TVarParamScriptObject::~TVarParamScriptObject()
{
	free((char*) FInfo._name);
}

bool TVarParamScriptObject::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flagset)
{
	switch (info->_index)
	{
		default:
			// Signal that the request was not handled.
			return false;

		case SID_VAR_SETUP:
		{
			if ((*params)[0].isNumber())
			{
				value->set(const_cast<Variable*>(FVar)->setup((*params)[0].getInteger()));
			}
			else
			{
				value->set(const_cast<Variable*>(FVar)->setup((*params)[0].getString()));
			}
		}
			break;

		case SID_VAR_ID:
		{
			if (flagset)
			{
				// Only allow setting the ID if the variable is owned.
				if (FVar->isGlobal())
				{
					const_cast<Variable*>(FVar)->setup(value->getInteger(), true);
				}
			}
			else
			{
				value->set((Value::int_type) FVar->getId());
			}
		}
			break;

		case SID_VAR_NAME:
		{
			if (!flagset)
			{
				value->set(FVar->getName((int) (*params)[0].getInteger()));
			}
		}
			break;

		case SID_VAR_UNIT:
		{
			if (!flagset)
			{
				value->set(FVar->getUnit());
			}
		}
			break;

		case SID_VAR_CUR:
		{
			if (flagset)
			{
				if (!FVar->setCur(*value, FSkipEvent))
				{
					SF_RTTI_NOTIFY(DO_DEFAULT, "Currrent value of '" << FVar->getName() << "' could not be set!");
				}
			}
			else
			{
				*value = FVar->getCur();
			}
		}
			break;

		case SID_VAR_CURSTR:
		{
			*value = FVar->getCurString();
		}
			break;

		case SID_VAR_DEF:
		{
			if (!flagset)
			{
				*value = FVar->getDef();
			}
		}
			break;

		case SID_VAR_RND:
		{
			if (!flagset)
			{
				*value = FVar->getRnd();
			}
		}
			break;

		case SID_VAR_MIN:
		{
			if (!flagset)
			{
				*value = FVar->getMin();
			}
		}
			break;

		case SID_VAR_MAX:
		{
			if (!flagset)
			{
				*value = FVar->getMax();
			}
		}
			break;

		case SID_VAR_FLAGS:
		{
			if (flagset)
			{
				// Only allow setting the ID if the variable is owned.
				if (FVar->isGlobal())
				{
					const_cast<Variable*>(FVar)->updateFlags(Variable::toFlags(value->getString()), FSkipEvent);
				}
			}
			else
			{
				*value = FVar->getCurFlagsString();
			}
		}
			break;

		case SID_VAR_ISFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			// Check if all passed flags are set.
			value->set((FVar->getCurFlags() & flags) == flags);
		}
			break;

		case SID_VAR_SETFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			flags |= FVar->getCurFlags();
			const_cast<Variable*>(FVar)->updateFlags(flags, FSkipEvent);
		}
			break;

		case SID_VAR_UNSETFLAGS:
		{
			auto flags = Variable::toFlags((*params)[0].getString());
			flags = FVar->getCurFlags() & ~flags;
			const_cast<Variable*>(FVar)->updateFlags(flags, FSkipEvent);
		}
			break;
	}
	// Signal that the request was handled.
	return true;
}

/**
 * Information object for importing and exporting TVariable.
 */
struct InformationScript::GiiVariableInfo :TVarParamScriptObject, Variable
{
	// Constructor for import.
	GiiVariableInfo(const char* name, long id);

	// Constructor for export.
	GiiVariableInfo(const char* name, const std::string& defin);

	// Overloaded from base class.
	[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

	// Overloaded from base class.
	bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flagset) override;

	// Instruction pointer to jump to on a change of value. 0 means disabled.
	ip_type OnValue{0};
	// Instruction pointer to jump to on a change of flags. 0 means disabled.
	ip_type OnFlags{0};
	// Instruction pointer to jump to on a change of ID. 0 means disabled.
	ip_type OnId{0};
	// Label called in back ward compatibility mode.
	std::string ChangeLabel;
};

InformationScript::GiiVariableInfo::GiiVariableInfo(const char* name, long id)
	:TVarParamScriptObject(this, "Variable", name, false), Variable(id, true)
{
	// Assign the pointer to the variable in the base class.
	FVar = this;
}

InformationScript::GiiVariableInfo::GiiVariableInfo(const char* name, const std::string& definition)
	:TVarParamScriptObject(this, "Variable", name, true), Variable(definition)
{
	// Assign the pointer to the variable in the base class.
	FVar = this;
}

static ScriptObject::IdInfo VarObjInfo[] =
	{
		{SID_VAR_ON_VALUE, InformationScript::idVariable, "OnValue", 0, nullptr},
		{SID_VAR_ON_FLAGS, InformationScript::idVariable, "OnFlags", 0, nullptr},
		{SID_VAR_ON_ID, InformationScript::idVariable, "OnId", 0, nullptr},
		{SID_VAR_SKIPEVENT, InformationScript::idVariable, "SkipEvent", 0, nullptr}
	};

const ScriptObject::IdInfo* InformationScript::GiiVariableInfo::getInfo(const std::string& name) const
{
	for (auto& i: VarObjInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<InformationScript::GiiVariableInfo*>(this);
			//
			return &i;
		}
	}
	//
	return TVarParamScriptObject::getInfo(name);
}

bool
InformationScript::GiiVariableInfo::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params,
	bool flagset)
{
	switch (info->_index)
	{
		case SID_VAR_ON_VALUE:
		{
			if (flagset)
			{
				OnValue = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnValue);
			}
		}
			break;

		case SID_VAR_ON_FLAGS:
		{
			if (flagset)
			{
				OnFlags = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnFlags);
			}
		}
			break;

		case SID_VAR_ON_ID:
		{
			if (flagset)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnId);
			}
		}
			break;

		case SID_VAR_SKIPEVENT:
		{
			if (flagset)
			{
				FSkipEvent = value->getInteger() > 0;
			}
			else
			{
				value->set(FSkipEvent);
			}
		}
			break;

		default:
			// Call the derived class if not part of this one.
			return TVarParamScriptObject::getSetValue(info, value, params, flagset);
	}
	//
	return true;
}

/**
 * Information object for importing and exporting TResultData.
 */
struct InformationScript::GiiResultDataInfo :ScriptObject, ResultData
{
	// Constructor for import.
	explicit GiiResultDataInfo(long id)
		:ScriptObject("ResultData")
		 , ResultData(id, true)
		 , OnId(0)
		 , OnClear(0)
		 , OnAccess(0)
		 , OnGotRange(0) {}

	// Instruction pointer to jump to on a change of ID. 0 means disabled.
	ip_type OnId;
	// Instruction pointer to jump to on a change of access range. 0 means disabled.
	ip_type OnAccess;
	// Instruction pointer to jump to on a clear event. 0 means disabled.
	ip_type OnClear;
	// Instruction pointer to jump to on a got data event. 0 means disabled.
	ip_type OnGotRange;

	// Overridden from base class TObjInfo.
	[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

	// Overridden from base class TObjInfo.
	bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flagset) override;

	/**
	 * Overridden from base class ScriptObject.
	 */
	void destroyObject(bool& should_delete) override
	{
		should_delete = false;
	}
};

static InformationScript::IdInfo ResObjInfo[] =
	{
		{SID_RES_ID, InformationScript::idConstant, "Id", 0, nullptr},
		{SID_RES_NAME, InformationScript::idFunction, "Name", 1, nullptr},
		{SID_RES_FLAGS, InformationScript::idConstant, "Flags", 0, nullptr},
		{SID_RES_OFFSET, InformationScript::idConstant, "Offset", 0, nullptr},
		{SID_RES_RANGE, InformationScript::idConstant, "Range", 0, nullptr},
		{SID_RES_DATA, InformationScript::idFunction, "Data", 1, nullptr},
		{SID_RES_REQUEST, InformationScript::idFunction, "Request", 2, nullptr},
		{SID_RES_BLOCKCOUNT, InformationScript::idVariable, "BlockCount", 0, nullptr},
		{SID_RES_ON_ID, InformationScript::idVariable, "OnId", 0, nullptr},
		{SID_RES_ON_ACCESS, InformationScript::idVariable, "OnAccess", 0, nullptr},
		{SID_RES_ON_CLEAR, InformationScript::idVariable, "OnClear", 0, nullptr},
		{SID_RES_ON_GOTRANGE, InformationScript::idVariable, "OnGotRange", 0, nullptr},
	};

const InformationScript::IdInfo* InformationScript::GiiResultDataInfo::getInfo(const std::string& name) const
{
	for (auto& i:ResObjInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<InformationScript::GiiResultDataInfo*>(this);
			//
			return &i;
		}
	}
	return getInfoUnknown();
}

bool InformationScript::GiiResultDataInfo::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params,
	bool flagset)
{
	switch (info->_index)
	{
		case SID_RES_ID:
		{
			if (flagset)
			{
				setup(value->getInteger(), true);
			}
			else
			{
				value->set((Value::int_type) getId());
			}
		}
			break;

		case SID_RES_NAME:
		{
			if (!flagset)
			{
				value->set(getName((int) (*params)[0].getInteger()));
			}
		}
			break;

		case SID_RES_OFFSET:
		{
			if (!flagset)
			{
				value->set((Value::int_type) getValueOffset());
			}
		}
			break;

		case SID_RES_RANGE:
		{
			if (!flagset)
			{
				value->set((Value::int_type) getValueRange());
			}
		}
			break;

		case SID_RES_DATA:
		{
			Value::int_type data = 0;
			// Check for a valid ID.
			if (getId() && getBlockSize() == 1 && getTypeSize() <= sizeof(Value::int_type))
			{
				Range::size_type index = (*params)[0].getInteger();
				// When negative take the last entered.
				if (index < 0)
				{
					index = getBlockCount() - 1;
				}
				if (index >= 0)
				{
					blockRead(index, 1, &data, false);
				}
			}
			value->set(getValue(&data));
		}
			break;

		case SID_RES_BLOCKCOUNT:
		{
			value->set((Value::int_type) getBlockCount());
		}
			break;

		case SID_RES_REQUEST:
		{
			Range::size_type ofs = (*params)[0].getInteger();
			Range::size_type sz = (*params)[1].getInteger();
			// When the size is negative the data till the end is requested.
			if (sz < 0)
			{
				sz = getBlockCount() - ofs;
			}
			//
			value->set(requestRange(ofs, sz));
		}
			break;

		case SID_RES_FLAGS:
		{
			if (!flagset)
			{
				value->set(getFlagsString());
			}
		}
			break;

		case SID_RES_ON_ID:
		{
			if (flagset)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnId);
			}
		}
			break;

		case SID_RES_ON_ACCESS:
		{
			if (flagset)
			{
				OnAccess = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnAccess);
			}
		}
			break;

		case SID_RES_ON_CLEAR:
		{
			if (flagset)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnClear);
			}
		}
			break;

		case SID_RES_ON_GOTRANGE:
		{
			if (flagset)
			{
				OnId = (ip_type) value->getInteger();
			}
			else
			{
				value->set(OnGotRange);
			}
		}
			break;

		default:
			return false;
	}
	return true;
}

InformationScript::InformationScript()
	:_varLink(this, &InformationScript::variableHandler)
	 , _resLink(this, &InformationScript::resultDataHandler) {}

InformationScript::~InformationScript()
{
	clear();
}

void InformationScript::clear()
{
	// Call overloaded function first.
	ScriptInterpreter::clear();
	// Remove all imports and exports.
	for (auto i: _variableInfoList)
	{
		delete i;
	}
	_variableInfoList.clear();
	// Remove all created results.
	for (auto i: _resultDataInfoList)
	{
		delete i;
	}
	_resultDataInfoList.clear();
}

void InformationScript::linkInstruction()
{
	for (auto i: _variableInfoList)
	{
		// If a label has been specified.
		if (i->ChangeLabel.length())
		{
			auto ip = getLabelIp(i->ChangeLabel);
			i->OnValue = ip;
			if (ip == -1)
			{
				if (_outputStream)
				{
					*_outputStream << "Label '" << i->ChangeLabel << "' NOT found!\n";
				}
				setError(aeLabelNotFound, i->ChangeLabel);
			}
		}
	}
	// Calling the base class function will disable GetLabelIp().
	ScriptInterpreter::linkInstruction();
}

InformationScript::IdInfo InformationScript::_infoList[] =
	{
		// Functions
		{SID_TIME, idFunction, "Time", 0, nullptr},
		{SID_DATE, idFunction, "Date", 0, nullptr},
		{SID_SETFLAGS, idFunction, "SetFlags", 2, nullptr},
		{SID_SETFLAG, idFunction, "SetFlag", 2, nullptr},
		{SID_UNSETFLAG, idFunction, "UnsetFlag", 2, nullptr},
		{SID_CREATEVAR, idFunction, "CreateVar", 2, nullptr},
		{SID_VARIABLE, idFunction, "Variable", 1, nullptr},
		{SID_RESULTDATA, idFunction, "ResultData", 1, nullptr},
		// Keywords. (do not need speed index)
		{SID_ON_CHANGE, idKeyword, "on_change", kwExternal, nullptr},
		// TODO: Import and export keywords are maybe obsolete because of the object keyword.
		{SID_VAR_IMPORT, idKeyword, "import", kwExternal, nullptr},
		{SID_VAR_EXPORT, idKeyword, "export", kwExternal, nullptr},
	};

std::string InformationScript::getInfoNames() const
{
	// Call base class function first.
	std::string s = ScriptInterpreter::getInfoNames();
	// Add now our own functions.
	for (auto& i: _infoList)
	{
		s += i._name;
		s += "\n";
	}
	// Add the object members functions.
	for (auto& i: VarParamInfo)
	{
		s += "<VarParam>.";
		s += i._name;
		s += "\n";
	}
	// Add the object members functions.
	for (auto& i: VarObjInfo)
	{
		s += "<Variable>.";
		s += i._name;
		s += "\n";
	}
	// Add the object members functions.
	for (auto& i: ResObjInfo)
	{
		s += "<ResultData>.";
		s += i._name;
		s += "\n";
	}
	return s;
}

const InformationScript::IdInfo* InformationScript::getInfo(const std::string& name) const
{
	for (auto& i: _infoList)
	{
		if (i._name == name)
		{
			return &i;
		}
	}
	// Find the import or exported variables.
	for (auto i: _variableInfoList)
	{
		if (i->Info()._name == name)
		{
			return &i->Info();
		}
	}
/*
	TODO: Global functions ?
	// Get the table entry for the function name if it exists.
	if (auto fe = TFunctionBaseTableEntry::getEntry(name))
	{
		IdInfo& info(*const_cast<IdInfo*>(&_infoFunction));
		info._index = SID_FUNCTIONENTRY;
		info._id = idFunction;
		info._name = fe->GetName().c_str();
		info._paramCount = fe->GetParamCount();
		info._data = fe;
		return &InfoFunctionTable;
	}
*/
	// Call the inherited method.
	return ScriptInterpreter::getInfo(name);
}

bool InformationScript::getSetValue(const InformationScript::IdInfo* info, Value* value, Value::vector_type* params,
	bool flag_set)
{
	// Check if the ID is in our range if not let the base class handle the request.
	if (info->_index<SID_GII_SCRIPT_START || info->_index>SID_VARSCRIPT_STOP)
	{
		return ScriptInterpreter::getSetValue(info, value, params, flag_set);
	}
	//
	switch (info->_index)
	{
		case SID_TIME:
		{
			char buf[80];
			time_t now = time(nullptr);
			struct tm local = *localtime(&now);
			strftime(buf, sizeof(buf), "%Y-%m-%d", &local);
			value->set(buf);
			break;
		}

		case SID_DATE:
		{
			char buf[80];
			time_t now = time(nullptr);
			struct tm local = *localtime(&now);
			strftime(buf, sizeof(buf), "%X", &local);
			value->set(buf);
			break;
		}

		case SID_SETFLAGS:
		{
			GiiVariableInfo* vi = _getVariableInfo((*params)[0].getString());
			if (vi)
			{
				vi->updateFlags(Variable::toFlags((*params)[1].getString()));
			}
			break;
		}

		case SID_SETFLAG:
		{
			auto vi = _getVariableInfo((*params)[0].getString());
			if (vi)
			{
				vi->setFlag(Variable::toFlags((*params)[1].getString()));
			}
			break;
		}

		case SID_UNSETFLAG:
		{
			auto vi = _getVariableInfo((*params)[0].getString());
			if (vi)
			{
				vi->unsetFlag(Variable::toFlags((*params)[1].getString()));
			}
			break;
		}

		case SID_CREATEVAR:
		{
			auto vi = _getVariableInfo((*params)[0].getString());
			if (vi)
			{
				std::string s = (*params)[1].getString();
				vi->setup(s);
			}
			break;
		}

		case SID_VARIABLE:
		{
			// First parameter is the local script name of the variable.
			// Second is the ID for imports
			GiiVariableInfo* vi = nullptr;
			int errors = 0;
			auto id = (*params)[0].getInteger(&errors);
			// If no conversion errors occurred the variable is an import one
			// because only the ID is specified.
			if (errors)
			{
				// Create export variable using passed setup string.
				vi = new GiiVariableInfo("", (*params)[0].getString());
			}
			else
			{
				// Create import variable using passed ID.
				vi = new GiiVariableInfo("", id);
			}
			// Add the item to the list.
			_variableInfoList.add(vi);
			// Hook this variable to the handler.
			vi->setHandler(&_varLink);
			// Return the pointer to the TObjectInfo derived class.
			value->set(Value::vitCustom, &vi, sizeof(&vi));
			break;
		}

		case SID_RESULTDATA:
		{
			// First parameter is the local script name of the variable.
			// Second is the ID for imports
			GiiResultDataInfo* ri = nullptr;
			// Create result client using passed ID.
			ri = new GiiResultDataInfo((*params)[0].getInteger());
			// Add the item to the list.
			_resultDataInfoList.add(ri);
			// Hook this result to the handler.
			ri->setHandler(&_resLink);
			// Return the pointer to the TObjectInfo derived class.
			value->set(Value::vitCustom, &ri, sizeof(&ri));
			break;
		}

		case SID_VAR_IMPORT:
		case SID_VAR_EXPORT:
		{
			auto vi = static_cast<GiiVariableInfo*>(info->_data);
			if (flag_set)
			{
				vi->setCur(*value);
			}
			else
			{
				*value = vi->getCur();
			}
			break;
		}

		case SID_FUNCTIONENTRY:
			if (info->_data)
			{
/*
				// Save the time left so a call to a function executing a dialog
				// does not result in an error.
				clock_t timeleft = LoopTimer.IsEnabled() ? LoopTimer.GetTimeLeft() : 0;
				TFunctionBaseTableEntry* entry = (TFunctionBaseTableEntry*)info->Data;
				value->SetType(Value::vitUNDEF);
				entry->Call(*params, *value);
				// Check if the returned value is a script object.
				TScriptObject* scrobj = CastToObject(*value);
				if (scrobj)
					// If so set the owner member of the object to this compiler.
					MakeOwner(scrobj);
				// Restore the time left before calling the external function.
				if (timeleft)
					LoopTimer.SetTimeLeft(timeleft);
*/
				break;
			}

		default:
			return setError(aeCompilerImplementationError, info->_name);

	}
	return true;
}

InformationScript::GiiVariableInfo* InformationScript::_getVariableInfo(const std::string& name)
{
	for (auto i: _variableInfoList)
	{
		// If the name has been found assign the label name to it.
		if (name == i->Info()._name)
		{
			return i;
		}
	}
	return nullptr;
}

bool InformationScript::compileAdditionalStatement(const InformationScript::IdInfo* info, const std::string& source)
{
	if (info->_index > SID_GII_SCRIPT_START)
	{
		switch (info->_index)
		{
			case SID_ON_CHANGE:
			{
				std::string name;
				auto pos = source.find(' ');
				if (pos != std::string::npos && source.length() > pos)
				{
					// Find the import or export that matches the name.
					for (auto i: _variableInfoList)
					{ // If the name has been found assign the label name to it.
						if (source.substr(0, pos) == i->Info()._name)
						{
							i->ChangeLabel = source.substr(pos + 1);
							// Link the variable to this class' event handler.
							i->setHandler(&_varLink);
						}
					}
				}
				break;
			}

			case SID_VAR_IMPORT:
			{
				auto pos = source.find(',');
				if (pos != std::string::npos && source.length() > pos)
				{
					// Get variable id from first part of the line.
					auto id = std::strtoll(source.substr(pos + 1).c_str(), nullptr, 0);
					auto vi = new GiiVariableInfo(source.substr(0, pos).c_str(), id);
					_variableInfoList.add(vi);
				}
				else
				{
					return false;
				}
				break;
			}

			case SID_VAR_EXPORT:
			{ // Has the setup string at least a size.
				if (source.length())
				{
					auto definition = source;
					// Find the first separator.
					auto pos = source.find(',');
					// If no separator was found it could be a variable declaration.
					if (pos == std::string::npos)
					{
						definition.append(",");
						pos = definition.length() - 1;
					}
					//
					auto vi = new GiiVariableInfo(source.substr(0, pos).c_str(), source.substr(pos + 1));
					//
					_variableInfoList.add(vi);
				}
				else
				{
					return false;
				}
				break;
			}
		}
	}
	return ScriptInterpreter::compileAdditionalStatement(info, source);
}

void InformationScript::variableHandler(Variable::EEvent event,
	const Variable& caller, Variable& link,	bool same_inst)
{
	(void) same_inst;
	if (event < Variable::veFirstLocal)
	{
		return;
	}
	// Do not allow events when not compiled.
	if (getState() == esError || getState() == esCompiled)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Event avoided because script was in Error or not Initialized!\n" << getDebugText());
		return;
	}
	//
	auto vi = dynamic_cast<GiiVariableInfo*>(&link);
	//
	switch (event)
	{
		case Variable::veValueChange:
			if (vi->OnValue > 0)
			{
				callFunction(vi->OnValue, isStepMode());
			}
			break;

		case Variable::veFlagsChange:
			if (vi->OnFlags > 0)
			{
				callFunction(vi->OnFlags, isStepMode());
			}
			break;

		case Variable::veIdChanged:
			if (vi->OnId > 0)
			{
				callFunction(vi->OnId, isStepMode());
			}
			break;

		default:
			break;
	}
}

void InformationScript::resultDataHandler(ResultData::EEvent event,
	const ResultData& caller, ResultData& link,	const Range& rng, bool same_inst)
{
	(void) same_inst;
	if (event < ResultData::reFirstLocal)
	{
		return;
	}
	// Do not allow events when not compiled.
	if (getState() == esError || getState() == esCompiled)
	{
		SF_RTTI_NOTIFY(DO_DEFAULT, "Event avoided because script was in Error or not Initialized!\n" << getDebugText());
		return;
	}
	//
	auto ri = (GiiResultDataInfo*) &link;
	//
	switch (event)
	{
		case ResultData::reAccessChange:
			if (ri->OnAccess > 0)
			{
				callFunction(ri->OnAccess, isStepMode());
			}
			break;

		case ResultData::reIdChanged:
			if (ri->OnId > 0)
			{
				callFunction(ri->OnId, isStepMode());
			}
			break;

		case ResultData::reClear:
			if (ri->OnClear > 0)
			{
				callFunction(ri->OnClear, isStepMode());
			}
			break;

		case ResultData::reGotRange:
			if (ri->OnClear > 0)
			{
				callFunction(ri->OnGotRange, isStepMode());
			}
			break;

		default:
			break;
	}
}

void InformationScript::exitFunction(EExitCode exitcode, const Value& value)
{
	ScriptInterpreter::exitFunction(exitcode, value);
}

}
