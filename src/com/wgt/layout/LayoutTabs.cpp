#include "LayoutTabs.h"
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <gii/qt/LayoutData.h>
#include <gii/qt/VariableWidgetBasePrivate.h>
#include <misc/gen/ScriptEngine.h>
#include <misc/gen/dbgutils.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

struct LayoutTabs::Private : QObject
	, VariableWidgetBase::PrivateBase
{
		struct TabWidget : QTabWidget
		{
				explicit TabWidget(QWidget* parent)
					: QTabWidget(parent)
				{}

				// Fix for keeping the same size after changing the tab index.
				[[nodiscard]] QSize sizeHint() const override
				{
					return _fixedSizeHint.isValid() ? _fixedSizeHint : QTabWidget::sizeHint();
				}

				// Fix for keeping the same size after changing the tab index.
				[[nodiscard]] QSize minimumSizeHint() const override
				{
					return _fixedSizeHint.isValid() ? _fixedSizeHint : QTabWidget::minimumSizeHint();
				}

				// Holds the size hint when layouts have been loaded.
				QSize _fixedSizeHint;
		};

		QStringList _tabsConfig;
		LayoutTabs* _w{nullptr};
		QVBoxLayout* _layout{nullptr};
		TabWidget* _tabWidget{nullptr};

		static LayoutTabs::Private* cast(PrivateBase* data)
		{
			return static_cast<LayoutTabs::Private*>(data);// NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
		}

		explicit Private(LayoutTabs* widget)
			: _w(widget)
		{
			_w->_p = this;
			_tabWidget = new TabWidget(_w);
			// Name the widget for ease of reference.
			_tabWidget->setObjectName("tabWidget");
			// Format using Horizontal layout.
			_layout = new QVBoxLayout(_w);
			// Clear margins for now.
			_layout->setContentsMargins(0, 0, 0, 0);
			// Add the tab-widget.
			_layout->addWidget(_tabWidget);
			// Allows setting tab order in designer.
			_w->setFocusPolicy(Qt::StrongFocus);
			// Then when getting focus pass it to the child line edit widget.
			_w->setFocusProxy(_tabWidget);
			// Only link when not in design mode.
			if (!ObjectExtension::inDesigner())
			{
				_variable.setHandler(this);
			}
			connect(_tabWidget, &QTabWidget::currentChanged, [&](int index) {
				_variable.setCur(Value(index), true);
			});
		}

		~Private() override
		{
			// Clear the handler when destructing.
			_variable.setHandler(nullptr);
		}

		void recreateTabs();

		void variableEventHandler(
			EEvent event,
			const Variable& call_var,
			Variable& link_var,
			bool same_inst
		) override
		{
			SF_COND_RTTI_NOTIFY(isDebug(), DO_DEFAULT, Variable::getEventName(event));
			switch (event)
			{
				default:
					break;

				case veLinked:
				case veIdChanged:
				case veValueChange:
					// When the variable is to be linked the variable readonly state matters.
					_w->applyReadOnly(_readOnly || (link_var.getDesiredId() ? link_var.isReadOnly() : false));
					_tabWidget->setCurrentIndex(static_cast<int>(link_var.getCur().getInteger()));
					break;

				case veFlagsChange:
					_w->applyReadOnly(_readOnly || link_var.isReadOnly());
					break;
			}
		}

		void updateValue(bool skip)
		{
			_variable.setCur(Value(_tabWidget->currentIndex()));
			// Update the control with the recently set value.
			if (!skip)
			{
				variableEventHandler(Variable::veValueChange, _variable, _variable, true);
			}
		}

		void onClicked(bool /* checked*/)
		{
			updateValue(false);
		}
};

LayoutTabs::LayoutTabs(QWidget* parent)
	: VariableWidgetBase(parent, this)
{
	// Is assigning itself to '_p' member.
	new LayoutTabs::Private(this);
}

void LayoutTabs::applyReadOnly(bool yn)
{
	auto p = Private::cast(_p);
	// Set focus policy on the container.
	setFocusPolicy(yn ? Qt::NoFocus : Qt::StrongFocus);
	auto tw = p->_tabWidget;
	// Only make readonly when the desired id has been set.
	tw->tabBar()->setEnabled(!yn || !p->_variable.getDesiredId());
	// Make the style for readonly have effect on the widget.
	tw->style()->polish(tw);
}

bool LayoutTabs::isRequiredProperty(const QString& name)
{
	return true;
}

QStringList LayoutTabs::getTabsConfig() const
{
	return Private::cast(_p)->_tabsConfig;
}

void LayoutTabs::setTabsConfig(const QStringList& sl)
{
	auto p = Private::cast(_p);
	if (p->_tabsConfig != sl)
	{
		p->_tabsConfig = sl;
		p->recreateTabs();
	}
}

void LayoutTabs::Private::recreateTabs()
{
	// Grows to the
	QRect rcCombined;
	// Delete the current tabs in reversed order.
	for (int i = _tabWidget->count(); i > 0;)
	{
		delete _tabWidget->widget(--i);
	}
	// Reset this member so the size is invalid.
	_tabWidget->_fixedSizeHint = {};
	// Add the tab passing an empty widget and empty tab name.
	_tabWidget->addTab(new QWidget(), "");
	// Get the size of an empty tab.
	QSize szFirst = _tabWidget->sizeHint();
	// Delete it after retrieving the size.
	delete _tabWidget->widget(0);
	// Get the needed parent layout widget to retrieve the directory.
	auto plw = ObjectExtension::inDesigner() ? nullptr : LayoutData::from(_w);
	// Iterate through the string list.
	for (auto& s: _tabsConfig)
	{
		// Split the string into fields.
		auto fields = s.split('|');
		// When other than 3 fields ignore the line.
		if (fields.size() != 3)
		{
			continue;
		}
		// This happens only when in a designer.
		if (plw)
		{
			// Form the filename using and create a file object for reading.
			QFile file(plw->getDirectory().absoluteFilePath(fields[2] + '.' + LayoutData::getFileSuffix()));
			// Check if the file exists.
			if (!file.open(QFile::ReadOnly | QFile::Text))
			{
				QMessageBox mb(_w);
				mb.setIcon(QMessageBox::QMessageBox::Warning);
				mb.setWindowTitle(tr("Open file failed!"));
				mb.setText(tr("Could open file '%1'\nin directory\n'%2'").arg(file.fileName().arg(plw->getDirectory().path())));
				mb.exec();
				return;
			}
			// TODO: Work-around using a dummy widget for https://bugreports.qt.io/browse/QTBUG-96693
			// Create the parent container widget without a parent and initialized the directory and id-offset.
			QScopedPointer cw(new QWidget);
			cw->setObjectName("dummy_layout_container");
			auto ldc = new LayoutData(cw.get());
			ldc->setDirectory(QFileInfo(file).absoluteDir());
			ldc->setIdOffset(fields.at(1).toULongLong(nullptr, 0));
			// Create widget from the ui-file.
			auto tab = FormBuilderLoad(&file, cw.get());
			// When loading was not successful.
			if (!tab)
			{
				QMessageBox mb(_w);
				mb.setIcon(QMessageBox::Warning);
				mb.setWindowTitle(tr("UI loading failed!"));
				mb.setText(tr("Could not load UI file '%1'").arg(file.fileName()));
				mb.exec();
				return;
			}
			else
			{
				// Grow to the largest geometry of all layouts.
				rcCombined |= tab->geometry();
				// Assign a new parent for the layout-data instance.
				ldc->setParent(tab);
				// Add the tab passing the widget and name.
				_tabWidget->addTab(tab, fields.at(0));
			}
		}
		else
		{
			// Create dummy pages for in the designer.
			auto tab = new QLabel(_tabWidget);
			tab->setAlignment(Qt::AlignCenter);
			tab->setText(QString("File '%1' at offset %2").arg(fields.at(2)).arg(fields.at(1)));
			_tabWidget->addTab(tab, fields.at(0));
		}
	}
	// Make the minimum size hint fixed to fix a sizing problem when the tab index changes.
	if (!_tabWidget->_fixedSizeHint.isValid() && _tabWidget->count() && plw)
	{
		_tabWidget->_fixedSizeHint = rcCombined.size() + szFirst;
	}
}

SF_IMPL_PROP_GSP(QTabWidget::TabPosition, LayoutTabs, TabPosition, LayoutTabs::Private::cast(_p)->_tabWidget, tabPosition)

SF_IMPL_PROP_GSP(QTabWidget::TabShape, LayoutTabs, TabShape, LayoutTabs::Private::cast(_p)->_tabWidget, tabShape)

SF_IMPL_PROP_GSP(int, LayoutTabs, CurrentIndex, LayoutTabs::Private::cast(_p)->_tabWidget, currentIndex)

}// namespace sf
