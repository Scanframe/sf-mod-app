#include <misc/qt/PropertySheetDialog.h>
#include <QTreeView>
#include <QFocusEvent>
#include <misc/qt/qt_utils.h>
#include "LayoutEditorAppModule.h"
#include "LayoutEditor.h"
#include "LayoutEditorPropertyPage.h"
#include "ObjectHierarchyModel.h"

namespace sf
{

LayoutEditorAppModule::LayoutEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
{
	addFileType(tr("UI Layout File"), "ui");
	//
	settingsReadWrite(false);
}

QString LayoutEditorAppModule::getName() const
{
	return tr("Layout Editor");
}

QString LayoutEditorAppModule::getDescription() const
{
	return tr("UI layout editor.");
}

void LayoutEditorAppModule::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new LayoutEditorPropertyPage(*this, sheet));
}

MultiDocInterface* LayoutEditorAppModule::createWidget(QWidget* parent) const
{
	auto le = new LayoutEditor(getSettings(), parent);
	le->setReadOnly(_readOnly);
	return le;
}

QString LayoutEditorAppModule::getLibraryFilename() const
{
	return QString::fromStdString(getLibraryName());
}

QString LayoutEditorAppModule::getSvgIconResource() const
{
	return ":icon/svg/layout";
}

void LayoutEditorAppModule::settingsReadWrite(bool save)
{
	if (!_settings)
	{
		return;
	}
	_settings->beginGroup("AppModule.Layout");
	QString keyReadOnly("ReadOnly");
	if (!save)
	{
		_readOnly = _settings->value(keyReadOnly, _readOnly).toBool();
	}
	else
	{
		_settings->setValue(keyReadOnly, _readOnly);
	}
	_settings->endGroup();

}

void LayoutEditorAppModule::initialize(bool)
{
}

AppModuleInterface::DockWidgetList LayoutEditorAppModule::createDockingWidgets(QWidget* parent)
{
	AppModuleInterface::DockWidgetList rv;
	// Use tree view pointer as a sentry just in case this function is called twice.
	if (!_hierarchyViewer)
	{
		auto dock = new QDockWidget(tr("Layout Hierarchy"), parent);
		dock->setObjectName("layoutHierarchy");
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		_hierarchyViewer = new HierarchyViewer(dock);
		dock->setWidget(_hierarchyViewer);
		rv.append(dock);
	}
	return rv;
}

void LayoutEditorAppModule::documentActivated(MultiDocInterface* interface, bool yn) const
{
	if (_hierarchyViewer)
	{
		if (auto le = dynamic_cast<LayoutEditor*>(interface))
		{
			_hierarchyViewer->setEditor(le);
		}
	}
}

}
