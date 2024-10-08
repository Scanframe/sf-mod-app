#include "LayoutButton.h"
#include <QGuiApplication>
#include <QLayout>
#include <QMessageBox>
#include <QScreen>
#include <gii/qt/LayoutData.h>
#include <misc/qt/FormDialog.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>

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
			: _w(widget)
		{
			_w->_p = this;
		}
};

LayoutButton::LayoutButton(QWidget* parent)
	: QToolButton(parent)
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
	if (auto ld = LayoutData::from(this))
	{
		// From the filename.
		QString fn = _p->_layoutFile + '.' + LayoutData::getFileSuffix();
		// Create file device.
		QFile file(ld->getDirectory().absoluteFilePath(fn));
		// Check if the file exists.
		if (!file.open(QFile::ReadOnly | QFile::Text))
		{
			QMessageBox mb(this);
			mb.setIcon(QMessageBox::QMessageBox::Warning);
			mb.setWindowTitle(tr("Open file failed!"));
			mb.setText(tr("Could open file '%1' in directory '%2'").arg(fn).arg(ld->getDirectory().path()));
			mb.exec();
			return;
		}
		// Create the popup dialog as the parent for the layout.
		QScopedPointer dlg(new FormDialog(this, Qt::Popup));
		// Offset used for the ID's in GII widgets.
		dlg->setProperty("idOffset", getIdOffset());
		// TODO: Work-around using a dummy widget for https://bugreports.qt.io/browse/QTBUG-96693
		// Set no parent to work around the bug.
		QScopedPointer cw(new QWidget(nullptr));
		// Create data object for dependent widgets.
		auto ldc = new LayoutData(cw.get());
		ldc->setIdOffset(ld->getIdOffset() + getIdOffset());
		ldc->setDirectory(QFileInfo(file).absoluteDir());
		// Create widget from the ui-file.
		_p->_layoutContainer = FormBuilderLoad(&file, cw.get());
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
			// Set dialog the as the parent for the container.
			_p->_layoutContainer->setParent(dlg.get());
			// Make the container widget the parent for the layout data.
			ldc->setParent(_p->_layoutContainer);
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
	}
}

SF_IMPL_PROP_GRS(QString, LayoutButton, LayoutFile, _p->_layoutFile)

SF_IMPL_PROP_GS(Gii::IdType, LayoutButton, IdOffset, _p->_idOffset)

}// namespace sf