#include <QCoreApplication>
#include <misc/gen/ScriptObject.h>
#include <misc/qt/qt_utils.h>
#include <misc/gen/ConfigLocation.h>
#include "IniProfileObject.h"

namespace sf
{

SF_REG_CLASS
(
	ScriptObject, ScriptObject::Parameters, Interface,
	IniProfileObject, "IniFile", "Ini-file access object."
)

IniProfileObject::IniProfileObject(const Parameters& params)
	:ScriptObject("IniFile")
{
	QFileInfo fi(QCoreApplication::applicationFilePath());
	// Set the instance to change the extension only.
	fi.setFile(QString::fromStdString(getConfigLocation()), fi.completeBaseName() + ".ini");
	_iniProfile.setFilepath(fi.absoluteFilePath().toStdString());
	_iniProfile.setSection("Default");
}

// Speed index.
enum :int
{
	sidPath = 1,
	sidSection,
	sidRead,
	sidWrite,
};

ScriptObject::IdInfo IniProfileObject::_objectInfo[] = {
	{sidPath, ScriptObject::idVariable, "Path", 0, nullptr},
	{sidSection, ScriptObject::idVariable, "Section", 0, nullptr},
	{sidRead, ScriptObject::idFunction, "Load", 2, nullptr},
	{sidWrite, ScriptObject::idFunction, "Read", 2, nullptr},
};

const ScriptObject::IdInfo* IniProfileObject::getInfo(const std::string& name) const
{
	for (auto& i: _objectInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<IniProfileObject*>(this);
			//
			return &i;
		}
	}
	return getInfoUnknown();
}

bool IniProfileObject::getSetValue(const ScriptObject::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			return false;

		case sidSection:
		{
			if (flag_set)
			{
				_iniProfile.setSection(value->getString());
			}
			else
			{
				value->set(_iniProfile.getSection());
			}
			break;
		}

		case sidRead:
		{
			std::string val;
			_iniProfile.getString((*params)[0].getString(), val, (*params)[1].getString());
			value->assign(val);
			break;
		}

		case sidWrite:
		{
			value->set(_iniProfile.setString((*params)[0].getString(), (*params)[1].getString()));
			break;
		}

		case sidPath:
			if (flag_set)
			{
				QFileInfo fi(value->getQString());
				if (fi.isRelative())
				{
					// Set the instance to change the extension only.
					fi.setFile(QString::fromStdString(getConfigLocation()), fi.filePath());
				}
				_iniProfile.setFilepath(fi.absoluteFilePath().toStdString());
				// Return the resolved path.
				value->set(_iniProfile.getFilepath());
			}
			else
			{
				value->set(_iniProfile.getFilepath());
			}
			break;
	}
	//
	return true;
}

}
