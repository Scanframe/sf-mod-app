#include <QPlainTextEdit>
#include <QMessageBox>
#include "MultiDocInterface.h"
#include "AppModuleInterface.h"

namespace sf
{

MultiDocInterface::MultiDocInterface(QWidget* editor)
	:_editor(editor)
{
	if (auto c_edit = dynamic_cast<QPlainTextEdit*>(editor))
	{
		QObject::connect(c_edit, &QPlainTextEdit::textChanged, [&]() -> void {emit mdiSignals.textChanged();});
		QObject::connect(c_edit, &QPlainTextEdit::copyAvailable, [&](bool b) -> void {emit mdiSignals.copyAvailable(b);});
		QObject::connect(c_edit, &QPlainTextEdit::undoAvailable, [&](bool b) -> void {emit mdiSignals.undoAvailable(b);});
		QObject::connect(c_edit, &QPlainTextEdit::redoAvailable, [&](bool b) -> void {emit mdiSignals.redoAvailable(b);});
		QObject::connect(c_edit, &QPlainTextEdit::selectionChanged, [&]() -> void {emit mdiSignals.selectionChanged();});
		QObject::connect(c_edit, &QPlainTextEdit::cursorPositionChanged,
			[&]() -> void {emit mdiSignals.cursorPositionChanged();});
		QObject::connect(c_edit, &QPlainTextEdit::modificationChanged,
			[&](bool b) -> void {emit mdiSignals.modificationChanged(b);});
	}
	else if (auto t_edit = dynamic_cast<QTextEdit*>(editor))
	{
		QObject::connect(t_edit, &QTextEdit::textChanged, [&]() -> void {emit mdiSignals.textChanged();});
		QObject::connect(t_edit, &QTextEdit::copyAvailable, [&](bool b) -> void {emit mdiSignals.copyAvailable(b);});
		QObject::connect(t_edit, &QTextEdit::undoAvailable, [&](bool b) -> void {emit mdiSignals.undoAvailable(b);});
		QObject::connect(t_edit, &QTextEdit::redoAvailable, [&](bool b) -> void {emit mdiSignals.redoAvailable(b);});
		QObject::connect(t_edit, &QTextEdit::selectionChanged, [&]() -> void {emit mdiSignals.selectionChanged();});
		QObject::connect(t_edit, &QTextEdit::cursorPositionChanged,
			[&]() -> void {emit mdiSignals.cursorPositionChanged();});
		QObject::connect(t_edit, &QTextEdit::textChanged, [&]() -> void {emit mdiSignals.modificationChanged(true);});
	}
}

bool MultiDocInterface::canClose() const
{
	if (!isModified())
	{
		return true;
	}
	auto ret = QMessageBox::warning(_editor, MultiDocInterfaceSignals::tr("MDI"),
		MultiDocInterfaceSignals::tr("'%1' has been modified.\nDo you want to save your changes?").arg(
			userFriendlyCurrentFile()),
		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	switch (ret)
	{
		case QMessageBox::Save:
			return const_cast<MultiDocInterface*>(this)->save();
		case QMessageBox::Cancel:
			return false;
		default:
			break;
	}
	return true;
}

QString MultiDocInterface::getFileTypeFilters() const
{
	return _module ? _module->getFileTypeFilters() :QString();
}

}
