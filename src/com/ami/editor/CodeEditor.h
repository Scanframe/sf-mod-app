#pragma once
#include <ami/editor/CodeEditor.h>
#include <ami/iface/PlainTextEditMdi.h>
#include <misc/qt/Editor.h>

namespace sf
{

class CodeEditor
	: public Editor
	, public PlainTextEditMdi
{
	public:
		CodeEditor(QWidget* parent);

		bool canClose() override;
};

}// namespace sf
