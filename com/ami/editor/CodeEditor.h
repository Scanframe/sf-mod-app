#pragma once

#include <misc/qt/Editor.h>
#include <ami/iface/PlainTextEditMdi.h>
#include "CodeEditor.h"

namespace sf
{

class CodeEditor :public Editor, public PlainTextEditMdi
{
	public:
		CodeEditor(QWidget* parent);

		bool canClose() override;
};

}
