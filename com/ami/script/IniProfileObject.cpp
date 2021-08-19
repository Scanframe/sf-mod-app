#include "IniProfileObject.h"
#include <misc/gen/ScriptObject.h>
#include <misc/gen/IniProfile.h>
#include <misc/qt/qt_utils.h>
#include <QCoreApplication>

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
	// TODO: This must be configured globally.
	// Set the default file.
	QFileInfo fi(QCoreApplication::applicationFilePath());
	fi.setFile(fi.absolutePath() + QDir::separator() + "config", fi.completeBaseName() + ".cfg");
	_iniProfile.setFilepath(fi.absoluteFilePath().toStdString());
	_iniProfile.setSection("Default");
}

#define SID_INI_PATH 1
#define SID_INI_SECTION 2
#define SID_INI_READ  3
#define SID_INI_WRITE 4
#define SID_INI_REDIRECT 5


ScriptObject::IdInfo IniProfileObject::_objectInfo[] =
	{
		{SID_INI_PATH, ScriptObject::idVariable, "Path", 0, nullptr},
		{SID_INI_SECTION, ScriptObject::idVariable, "Section", 0, nullptr},
		{SID_INI_READ, ScriptObject::idFunction, "Read", 2, nullptr},
		{SID_INI_WRITE, ScriptObject::idFunction, "Write", 2, nullptr},
		{SID_INI_REDIRECT, ScriptObject::idFunction, "Redirect", 1, nullptr},
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
	return ScriptObject::getInfoUnknown();
}

bool IniProfileObject::getSetValue(const ScriptObject::IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			return false;

		case SID_INI_SECTION:
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

		case SID_INI_READ:
		{
			std::string val;
			_iniProfile.getString((*params)[0].getString(), val, (*params)[1].getString());
			value->assign(val);
			break;
		}

		case SID_INI_WRITE:
		{
			value->set(_iniProfile.setString((*params)[0].getString(), (*params)[1].getString()));
			break;
		}

		case SID_INI_REDIRECT:
		{
			QFileInfo fi((*params)[0].getQString());
			if (fi.isRelative())
			{
				QFileInfo fia(QCoreApplication::applicationFilePath());
				// Set the instance to change the extension only.
				fi.setFile(fia.absolutePath() + QDir::separator() + "config", fi.filePath());
			}
			_iniProfile.setFilepath(fi.absoluteFilePath().toStdString());
			// Return the resolved path.
			value->set(_iniProfile.getFilepath());
			break;
		}

		case SID_INI_PATH:
		if (flag_set)
		{
			QFileInfo fi((*params)[0].getQString());
			if (fi.isRelative())
			{
				QFileInfo fia(QCoreApplication::applicationFilePath());
				// Set the instance to change the extension only.
				fia.setFile(fia.absolutePath() + QDir::separator() + "config", fi.filePath());
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
