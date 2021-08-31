#include <misc/gen/ScriptObject.h>
#include "FileFindObject.h"

namespace sf
{

SF_REG_CLASS
(
	ScriptObject, ScriptObject::Parameters, Interface,
	FileFindObject, "FileFind", "File finder object."
)

// Speed index.
enum :int
{
	sidFirst,
	sidNext,
	sidPath,
	sidName,
	sidInfoLine,
	sidIsDir,
	sidIsFile,
	sidIsFound,
};

ScriptObject::IdInfo FileFindObject::_objectInfo[] =
	{
		{sidFirst, ScriptObject::idFunction, "First", 2, nullptr},
		{sidNext, ScriptObject::idFunction, "Next"},
		{sidPath, ScriptObject::idConstant, "Path"},
		{sidName, ScriptObject::idConstant, "Name"},
		{sidIsDir, ScriptObject::idConstant, "IsDir"},
		{sidIsFile, ScriptObject::idConstant, "IsFile"},
		{sidIsFound, ScriptObject::idConstant, "IsFound"},
		{sidInfoLine, ScriptObject::idFunction, "InfoLine", 1, nullptr},
	};

const ScriptObject::IdInfo* FileFindObject::getInfo(const std::string& name) const
{
	for (auto& i: _objectInfo)
	{
		if (i._name == name)
		{
			// Assign this pointer to info structure to reference owner of this member.
			i._data = const_cast<FileFindObject*>(this);
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

		case sidFirst:
		{
			_dirIterator.reset(new QDirIterator((*params)[0].getQString(), QDir::Filter::Files));
			//std::string attr = (*params)[1].getString();
			value->set(_dirIterator->hasNext() ? _dirIterator->next() : "");
			break;
		}

		case sidNext:
			value->set(_dirIterator->next());
			break;

		case sidPath:
			value->set(_dirIterator->filePath());
			break;

		case sidName:
			value->set(_dirIterator->fileName());
			break;

		case sidInfoLine:
		{
			auto fi = _dirIterator->fileInfo();
			value->set(QString("%1 (%2) (%3)").arg(fi.baseName()).arg(fi.group()).arg(fi.size()));
			break;
		}

			// TODO: Needs implementing.
		case sidIsDir:
		case sidIsFile:
		case sidIsFound:
			value->set(true);
			break;
	}
	//
	return true;
}

}
