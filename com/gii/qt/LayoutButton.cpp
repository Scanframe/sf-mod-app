#include <QMessageBox>
#include <QLayout>
#include <QScreen>
#include <QCoreApplication>
#include <misc/qt/FormBuilder.h>
#include <misc/qt/FormDialog.h>
#include <QGuiApplication>
#include <misc/qt/qt_utils.h>
#include "LayoutButton.h"
#include "LayoutWidget.h"

namespace sf
{

struct LayoutButton::Private
{
	LayoutButton* _w;
	/**
	 * @brief Holds the relative file to the layout ui-file.
	 */
	QString _layoutFile;

	/**
	 * @brief Holds the relative file to the layout ui-file.
	 */
	Gii::IdType _idOffset{0};
	/**
	 * @brief Holds the widget containing layout.
	 */
	QWidget* _layoutContainer{nullptr};

	/**
	 * Constructor.
	 */
	explicit Private(LayoutButton* widget)
		:_w(widget)
	{
		_w->_p = this;
	}
};

LayoutButton::LayoutButton(QWidget* parent)
	:QToolButton(parent)
	 , ObjectExtension(this)
{
	new Private(this);
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
		QString fn = _p->_layoutFile + '.' + LayoutWidget::getSuffix();
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
		// Create the popup dialog as the parent for the layout.
		auto dlg = new FormDialog(this, Qt::Popup);
		// Offset used for the ID's in GII widgets.
		dlg->setProperty("idOffset", getIdOffset());
		// TODO: Work-around using a dummy widget for https://bugreports.qt.io/browse/QTBUG-96693
		// Set no parent to work around the bug.
		auto cw = new LayoutWidget(nullptr);
		// Offset used for the ID's in GII widgets.
		cw->setIdOffset(getIdOffset());
		// Create out form builder to load a layout.
		FormBuilder builder;
		// Add the application directory as the plugin directory to find custom plugins.
		builder.addPluginPath(QCoreApplication::applicationDirPath());
		// Create widget from the loaded ui-file.
		_p->_layoutContainer = builder.load(&file, cw);
		// When loading was not successful.
		if (!_p->_layoutContainer)
		{
			QMessageBox mb(this);
			mb.setIcon(QMessageBox::Warning);
			mb.setWindowTitle(tr("UI loading failed!"));
			mb.setText(tr("Could not load UI file '%1'").arg(file.size()));
			mb.exec();
			return;
		}
		else
		{
			_p->_layoutContainer->setParent(dlg);
			// Prepend the name of the object for optional styling.
			_p->_layoutContainer->setObjectName(QString("%1Popup").arg(_p->_layoutContainer->objectName()));
			// Position to actually move to.
			QPoint pos;
			// Position the popup dialog at a position
			switch (arrowType())
			{
				// For this put it centered.
				case Qt::NoArrow:
					pos = mapToGlobal(QPoint((size().width() - _p->_layoutContainer->size().width()) / 2, (size().height() - _p->_layoutContainer->size().height()) / 2));
					break;

				case Qt::DownArrow:
					pos = mapToGlobal(QPoint(0, size().height()));
					break;

				case Qt::UpArrow:
					pos = mapToGlobal(QPoint(0, -_p->_layoutContainer->size().height()));
					break;

				case Qt::RightArrow:
					pos = mapToGlobal(QPoint(size().width(), 0));
					break;

				case Qt::LeftArrow:
					pos = mapToGlobal(QPoint(-_p->_layoutContainer->size().width(), 0));
					break;
			}
			// Get the screen rectangle from the tool button.
			auto screenRect = QGuiApplication::screenAt(geometry().topLeft())->geometry();
			// The size and position of the dialog to be.
			QRect rc(pos, _p->_layoutContainer->size());
			// Check if the screen contains the dialog.
			if (!screenRect.contains(rc))
			{
				dlg->setGeometry(moveRectWithinRect(screenRect, rc));
			}
			else
			{
				dlg->move(pos);
			}
			dlg->exec();
		}
		delete dlg;
		delete cw;
	}
}

SF_IMPL_PROP_GRS(QString, LayoutButton, LayoutFile, _p->_layoutFile)

SF_IMPL_PROP_GS(Gii::IdType, LayoutButton, IdOffset, _p->_idOffset)

}