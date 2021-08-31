#pragma once

#include <misc/gen/ScriptObject.h>
#include <QDirIterator>

// TODO: This class is not linked yet and available for scripts to use.

namespace sf
{

class StringsObject :public ScriptObject
{
	public:
		// Constructor.
		explicit FileFindObject(Parameters&)
			:ScriptObject("Strings")
		{
		}

		[[nodiscard]] const IdInfo* getInfo(const std::string& name) const override;

		bool getSetValue(const IdInfo* info, Value* value, Value::vector_type* params, bool flag_set) override;

		void destroyObject(bool& should_delete) override
		{
			should_delete = false;
		}

		QScopedPointer<QDirIterator> _dirIterator;

		static IdInfo _objectInfo[];
};

}
