#include <QMessageBox>
#include <QCoreApplication>
#include "LayoutButton.h"
#include "LayoutWidget.h"
#include "FormBuilder.h"
#include "FormDialog.h"

namespace sf
{

LayoutButton::LayoutButton(QWidget* parent)
	:QToolButton(parent)
	 , ObjectExtension(this)
	 ,_layoutContainer(nullptr)
{
	auto action = new QAction(this);
	connect(action, &QAction::triggered, this, &LayoutButton::openLayout);
	setDefaultAction(action);
}

bool LayoutButton::isRequiredProperty(const QString& name)
{
	return true;
}

void LayoutButton::addPropertyPages(PropertySheetDialog* sheet)
{
	ObjectExtension::addPropertyPages(sheet);
	// TODO: Needs implementing.
}

void LayoutButton::openLayout()
{
	auto lw = LayoutWidget::getLayoutWidgetOf(this);
	if (lw)
	{
		// From the filename.
		QString fn = _layoutFile + '.' + LayoutWidget::getSuffix();
		// Create file device.
		QFile file(lw->getDirectory().absoluteFilePath(fn));
		// Check if the file exists.
		if (!file.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox mb(this);
			mb.setIcon(QMessageBox::QMessageBox::Warning);
			mb.setWindowTitle(tr("Open file failed!"));
			mb.setText(tr("Could open file '%1'\nin directory\n'%2'").arg(fn).arg(lw->getDirectory().path()));
			mb.exec();
			return;
		}
		// Create out form builder to load a layout.
		FormBuilder builder;
		// Add the application directory as the plugin directory to find custom plugins.
		builder.addPluginPath(QCoreApplication::applicationDirPath());
		// Create widget from the loaded ui-file.
		// Do not set parent when loading because it will mess up the QLabel buddy resolving by name.
		_layoutContainer = builder.load(&file, nullptr);
		// Prepend the name of the object for styling.
		_layoutContainer->setObjectName(QString("%1Popup").arg(_layoutContainer->objectName()));
		// When loading was not successful.
		if (!_layoutContainer)
		{
			QMessageBox mb(this);
			mb.setIcon(QMessageBox::Warning);
			mb.setWindowTitle(tr("UI loading failed!"));
			mb.setText(tr("Could not load UI file '%1'").arg(file.size()));
			mb.exec();
			return;
		}
		auto dlg = new FormDialog(this, Qt::Popup);
		_layoutContainer->setParent(dlg);
		dlg->move(mapToGlobal(QPoint(0, size().height())));
		dlg->exec();
		delete dlg;
	}
}

}