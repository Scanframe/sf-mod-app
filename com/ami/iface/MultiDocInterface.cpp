#include <QPlainTextEdit>
#include <QMessageBox>
#include "MultiDocInterface.h"
#include "AppModuleInterface.h"

namespace sf
{

MultiDocInterface::MultiDocInterface()
{
	(void) this;
}

void MultiDocInterface::setConnections(QWidget* editor)
{
	if (auto pte = dynamic_cast<QPlainTextEdit*>(editor))
	{
		QObject::connect(pte, &QPlainTextEdit::textChanged, [&]() -> void {emit mdiSignals.textChanged();});
		QObject::connect(pte, &QPlainTextEdit::copyAvailable, [&](bool b) -> void {emit mdiSignals.copyAvailable(b);});
		QObject::connect(pte, &QPlainTextEdit::undoAvailable, [&](bool b) -> void {emit mdiSignals.undoAvailable(b);});
		QObject::connect(pte, &QPlainTextEdit::redoAvailable, [&](bool b) -> void {emit mdiSignals.redoAvailable(b);});
		QObject::connect(pte, &QPlainTextEdit::selectionChanged, [&]() -> void {emit mdiSignals.selectionChanged();});
		QObject::connect(pte, &QPlainTextEdit::cursorPositionChanged,
			[&]() -> void {emit mdiSignals.cursorPositionChanged();});
		QObject::connect(pte, &QPlainTextEdit::modificationChanged,
			[&](bool b) -> void {emit mdiSignals.modificationChanged(b);});
	}
	else if (auto te = dynamic_cast<QTextEdit*>(editor))
	{
		QObject::connect(te, &QTextEdit::textChanged, [&]() -> void {emit mdiSignals.textChanged();});
		QObject::connect(te, &QTextEdit::copyAvailable, [&](bool b) -> void {emit mdiSignals.copyAvailable(b);});
		QObject::connect(te, &QTextEdit::undoAvailable, [&](bool b) -> void {emit mdiSignals.undoAvailable(b);});
		QObject::connect(te, &QTextEdit::redoAvailable, [&](bool b) -> void {emit mdiSignals.redoAvailable(b);});
		QObject::connect(te, &QTextEdit::selectionChanged, [&]() -> void {emit mdiSignals.selectionChanged();});
		QObject::connect(te, &QTextEdit::cursorPositionChanged,
			[&]() -> void {emit mdiSignals.cursorPositionChanged();});
		QObject::connect(te, &QTextEdit::textChanged, [&]() -> void {emit mdiSignals.modificationChanged(true);});
	}
	_editor = editor;
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

void MultiDocInterface::develop()
{
	auto obj = dynamic_cast<QObject*>(this);
	if (obj)
	{
		qInfo() << QString("%1::%2() is not overridden.").arg(obj->metaObject()->className()).arg(__FUNCTION__);
	}
}

void MultiDocInterface::stateSaveRestore(bool save)
{
}

}
