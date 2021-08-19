#include "IniProfileObject.h"
#include <misc/gen/ScriptObject.h>
#include <misc/gen/IniProfile.h>

namespace sf
{

SF_REG_CLASS
(
	ScriptObject, ScriptObject::Parameters, Interface,
	FileFindObject, "FileFind", "File finder object."
)

// Scan entry objects speed index defines.
#define SID_FIRST 1
#define SID_NEXT  2
#define SID_PATH  3
#define SID_NAME  4
#define SID_INFOLINE  5
#define SID_ISDIR     6
#define SID_ISFILE    7
#define SID_ISFOUND   8

ScriptObject::IdInfo FileFindObject::_objectInfo[] =
	{
		{SID_FIRST, ScriptObject::idFunction, "First", 2, nullptr},
		{SID_NEXT, ScriptObject::idFunction, "Next", 0, nullptr},
		{SID_PATH, ScriptObject::idConstant, "Path", 0, nullptr},
		{SID_NAME, ScriptObject::idConstant, "Name", 0, nullptr},
		{SID_ISDIR, ScriptObject::idConstant, "IsDir", 0, nullptr},
		{SID_ISFILE, ScriptObject::idConstant, "IsFile", 0, nullptr},
		{SID_ISFOUND, ScriptObject::idConstant, "IsFound", 0, nullptr},
		{SID_INFOLINE, ScriptObject::idFunction, "InfoLine", 1, nullptr},
	};

const ScriptObject::IdInfo* FileFindObject::getInfo(const std::string& name) const
{
	for (auto& i: _objectInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<ScriptObject*>(this);
			//
			return &i;
		}
	}
	return ScriptObject::getInfoUnknown();
}

bool FileFindObject::getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set)
{
	switch (info->_index)
	{
		default:
			return false;

		case SID_FIRST:
		{
			_dirIterator.reset(new QDirIterator((*params)[0].getQString(), QDir::Filter::Files));
			//std::string attr = (*params)[1].getString();
			value->set(_dirIterator->hasNext() ? _dirIterator->next() : "");
			break;
		}

		case SID_NEXT:
			value->set(_dirIterator->next());
			break;

		case SID_PATH:
			value->set(_dirIterator->filePath());
			break;

		case SID_NAME:
			value->set(_dirIterator->fileName());
			break;

		case SID_INFOLINE:
		{
			auto fi = _dirIterator->fileInfo();
			value->set(QString("%1 (%2) (%3)").arg(fi.baseName()).arg(fi.group()).arg(fi.size()));
			break;
		}

			// TODO: Needs implementing.
		case SID_ISDIR:
		case SID_ISFILE:
		case SID_ISFOUND:
			value->set(true);
			break;
	}
	//
	return true;
}

}
