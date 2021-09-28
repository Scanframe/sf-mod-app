#include <misc/qt/PropertySheetDialog.h>
#include <QTreeView>
#include <QFocusEvent>
#include <misc/qt/qt_utils.h>
#include <QUiLoader>
#include <QCoreApplication>
#include <misc/gen/ConfigLocation.h>
#include <misc/qt/Globals.h>
#include <misc/qt/ObjectPropertyModel.h>
#include "LayoutEditorAppModule.h"
#include "LayoutEditor.h"
#include "LayoutEditorPropertyPage.h"

namespace sf
{

LayoutEditorAppModule::LayoutEditorAppModule(const AppModuleInterface::Parameters& params)
	:AppModuleInterface(params)
	 , _settings(params._settings)
{
	addFileType(tr("UI Layout File"), "ui");
	//
	settingsReadWrite(false);
	// Create pre configured global UI loader instance.
	auto uiLoader = new QUiLoader(this);
	// Add the application directory as the plugin directory to find custom plugins.
	uiLoader->addPluginPath(QCoreApplication::applicationDirPath());
	// Not sure if this is needed at any time.
	uiLoader->setWorkingDirectory(QString::fromStdString(getConfigLocation()));
	// Make the created instance the current one.
	delete setGlobalUiLoader(uiLoader);
}

LayoutEditorAppModule::~LayoutEditorAppModule()
{
	delete setGlobalUiLoader(nullptr);
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
	connect(le, &LayoutEditor::objectSelected, [&](QObject* obj)
	{
		if (_hierarchyViewer)
		{
			_hierarchyViewer->selectObject(obj);
		}
	});
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

void LayoutEditorAppModule::initialize(InitializeStage stage)
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
		connect(_hierarchyViewer, &HierarchyViewer::objectSelectChange, [&](QObject* obj)
		{
			if (_tvProperties)
			{
				if (auto m = dynamic_cast<ObjectPropertyModel*>(_tvProperties->model()))
				{
					m->setTarget(obj);
					resizeColumnsToContents(_tvProperties);
				}
			}
		});
		dock->setWidget(_hierarchyViewer);
		rv.append(dock);
		// Add dock widget for the object properties.
		dock = new QDockWidget(tr("Object Properties"), parent);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		dock->setObjectName("layoutObjectProperties");
		_tvProperties = new QTreeView(dock);
		auto model = new ObjectPropertyModel();
		model->setDelegates(_tvProperties);
		connect(model, &ObjectPropertyModel::changed, _hierarchyViewer, &HierarchyViewer::documentModified);
		_tvProperties->setModel(model);
		_tvProperties->setAlternatingRowColors(true);
		dock->setWidget(_tvProperties);
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
