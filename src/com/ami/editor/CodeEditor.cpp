#include "CodeEditor.h"

namespace sf
{

CodeEditor::CodeEditor(QWidget* parent)
	:Editor(parent)
	 , PlainTextEditMdi(this)
{
}

bool CodeEditor::canClose()
{
	return PlainTextEditMdi::canClose();
}

}