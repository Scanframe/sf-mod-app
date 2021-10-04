#include <QActionGroup>
#include <QMenu>
#include <QSettings>
#include <QWidget>
#include "PropertySheetDialog.h"
#include "ui_PropertySheetDialog.h"
#include "Resource.h"
#include "qt_utils.h"

namespace sf
{

struct PropertySheetDialog::Private :QObject
{
	PropertySheetDialog* _s;

	Ui::PropertySheetDialog* ui;

	QSettings* _settings;

	QString _selectedPage{};

	Private(QSettings* settings, PropertySheetDialog* sheet)
	:QObject(sheet), _s(sheet), ui(new Ui::PropertySheetDialog), _settings(settings)
	{
		ui->setupUi(_s);
		//
		auto* menu = new QMenu("View", _s);
		//
		ui->splitter->setStretchFactor(0, 0);
		ui->splitter->setStretchFactor(1, 1);
		// Assign svg icons to the buttons.
		for (auto& i: QList<QPair<QAbstractButton*, Resource::Icon >>
			{
				{ui->btnOkay, Resource::Icon::Okay},
				{ui->btnCancel, Resource::Icon::Cancel},
				{ui->btnApply, Resource::Icon::Check},
			})
		{
			i.first->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(i.second), QPalette::ButtonText));
		}
		// Restore the state of the dialog including the list view mode.
		stateSaveRestore(false, nullptr);
		// Create aN action group.
		auto ag = new QActionGroup(menu);
		ag->setExclusive(true);
		//
		struct
		{
			QListView::ViewMode mode;
			QAction* action;
		} modes[] =
			{
				{QListView::IconMode, new QAction(tr("Icons"), menu)},
				{QListView::ListMode, new QAction(tr("List"), menu)}
			};
		// Add and iterate through the created.
		for (auto m: modes)
		{
			ag->addAction(m.action)->setCheckable(true);
			menu->addAction(m.action);
			m.action->setData(m.mode);
			m.action->setChecked(ui->listWidget->viewMode() == m.mode);
		}
		// Apply some properties according the current view mode.
		applyViewMode(ui->listWidget->viewMode());
		// Handle a click in the menu.
		connect(ag, &QActionGroup::triggered, [&](QAction* action) -> void
		{
			ui->listWidget->setViewMode((QListView::ViewMode) action->data().toInt());
			applyViewMode(ui->listWidget->viewMode());
		});
		// Enable the context menu.
		ui->listWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		// Connect the context menu signal.
		connect(ui->listWidget, &QWidget::customContextMenuRequested, [&, menu](QPoint pos) -> void
			{
				menu->popup(ui->listWidget->viewport()->mapToGlobal(pos));
			}
		);
		// Connect the needed signals.
		connect(ui->btnCancel, &QPushButton::clicked, _s, &PropertySheetDialog::close);
		connect(ui->btnOkay, &QPushButton::clicked, this, &Private::applyClose);
		connect(ui->btnApply, &QPushButton::clicked, this, &Private::apply);
		connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &Private::activatePage);
	}

	~Private() override
	{
		delete ui;
	}

	[[nodiscard]] auto getPages() const
	{
		QList<PropertyPage*> ppl;
		for (int i = 0; i < ui->listWidget->count(); i++)
		{
			ppl.append(qvariant_cast<PropertyPage*>(ui->listWidget->item(i)->data(Qt::ItemDataRole::UserRole)));
		}
		return ppl;
	}

	void stateSaveRestore(bool save, PropertyPage* pp)
	{
		_settings->beginGroup(getObjectNamePath(_s).join('.').prepend("State."));
		if (!pp)
		{
			QString keyState("State");
			QString keySplitter("Splitter");
			QString keyViewMode("ViewMode");
			QString keyPage("Page");
			if (save)
			{
				_settings->setValue(keyState, _s->saveGeometry());
				_settings->setValue(keySplitter, ui->splitter->saveState());
				_settings->setValue(keyViewMode, ui->listWidget->viewMode());
				_settings->setValue(keyPage, _selectedPage);
			}
			else
			{
				_s->restoreGeometry(_settings->value(keyState).toByteArray());
				ui->splitter->restoreState(_settings->value(keySplitter).toByteArray());
				auto vm = ui->listWidget->viewMode();
				ui->listWidget->setViewMode((QListView::ViewMode) _settings->value(keyViewMode, (int) vm).toInt());
				applyViewMode(vm);
				_selectedPage = _settings->value(keyPage, _selectedPage).toString();
			}
			for(auto p: getPages())
			{
				auto name = p->objectName();
				if (!name.isEmpty())
				{
					_settings->beginGroup(name);
					p->stateSaveRestore(*_settings, save);
					_settings->endGroup();
				}
			}
		}
		else
		{
			auto name = pp->objectName();
			if (!name.isEmpty())
			{
				_settings->beginGroup(name);
				pp->stateSaveRestore(*_settings, save);
				_settings->endGroup();
			}
		}
		_settings->endGroup();
	}

	void applyViewMode(QListView::ViewMode vm) const
	{
		switch (vm)
		{
			case QListView::ListMode:
				ui->listWidget->setIconSize(QSize(20, 20));
				ui->listWidget->setGridSize(QSize());
				break;
			case QListView::IconMode:
				ui->listWidget->setIconSize(QSize(50, 50));
				ui->listWidget->setGridSize(QSize(100, 100));
				break;
		}
	}

	void setActivePage(PropertyPage* page) const
	{
		// Deactivate all pages.
		for (int i = 0; i < ui->listWidget->count(); i++)
		{
			auto item = ui->listWidget->item(i);
			if (qvariant_cast<PropertyPage*>(item->data(Qt::ItemDataRole::UserRole)) == page)
			{
				ui->listWidget->setCurrentItem(item);
			}
		}
	}


	void activatePage()
	{
		// Deactivate all pages.
		for (int i = 0; i < ui->listWidget->count(); i++)
		{
			qvariant_cast<PropertyPage*>(ui->listWidget->item(i)->data(Qt::ItemDataRole::UserRole))->setVisible(false);
		}
		auto items = ui->listWidget->selectedItems();
		if (items.count())
		{
			// Get the page from the list widget.
			auto page = qvariant_cast<PropertyPage*>(items.first()->data(Qt::ItemDataRole::UserRole));
			// Now activate the page by making it visible.
			page->setVisible(true);
			// Assign the object name of the selected page.
			_selectedPage = page->objectName();
		}
	}

	void applyClose() const
	{
		// Apply the changes when modified.
		// Is an additional check when 'Ok' button is not enabled or disabled when modified.
		if (_s->isSheetModified())
		{
			// When apply was successful.
			if (apply())
			{
				// Close the sheet.
				_s->close();
			}
			return;
		}
		// Close the sheet.
		_s->close();
	}

	[[nodiscard]] bool apply() const
	{
		QList<PropertyPage*> modified_pages;
		// Iterate through the pages and collect the modified pages.
		for (auto p: getPages())
		{
			if (p->isPageModified())
			{
				modified_pages.append(p);
				// If a page cannot be applied return false.
				if (!p->canApplyPage())
				{
					// Activate the first page which could not be applied.
					setActivePage(p);
					// Signal failure to apply.
					return false;
				}
			}
		}
		// Only apply the modified pages.
		for (auto p: modified_pages)
		{
			p->applyPage();
		}
		// Call after apply to allow resolving dependencies between property pages or saving settings.
		for (auto p: getPages())
		{
			p->afterPageApply(modified_pages.contains(p));
		}
		// Calling checkModified will enable or disable the buttons.
		_s->checkModified(nullptr);
		// Emit the modified signal when a page was applied.
		if (!modified_pages.isEmpty())
		{
			Q_EMIT _s->modified();
		}
		// Signal all was applied.
		return true;
	}
};

PropertySheetDialog::PropertySheetDialog(const QString& name, QSettings* settings, QWidget* parent)
	:QDialog(parent), _p(nullptr)
{
	// Object name must be set before private structure is created.
	setObjectName(name);
	_p = new Private(settings, this);
}

PropertySheetDialog::~PropertySheetDialog()
{
	_p->stateSaveRestore(true, nullptr);
}

void PropertySheetDialog::addPage(PropertyPage* page)
{
	// Add the page to the layout.
	_p->ui->pageLayout->addWidget(page/*, 1, Qt::AlignmentFlag::AlignCenter*/);
	// Make the page initially invisible.
	page->setVisible(false);
	// Update the control widgets before connecting signals.
	page->updatePage();
	// Connect the controls after the update not before.
	for (auto w: page->connectControls())
	{
		qDebug() << __FUNCTION__  << "Not connected: " << w->objectName();
	}
	// Add an item to the list view.
	auto item = new QListWidgetItem(_p->ui->listWidget);
	// Assign the page to the user data.
	item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(page));
	// By default, select the first page.
	// Make the item appear selected when the page is visible.
	if ((!_p->_selectedPage.isEmpty() && page->objectName() == _p->_selectedPage))
	{
		_p->ui->listWidget->setCurrentItem(item);
	}
	// Use the widgets window icon.
	item->setIcon(page->getPageIcon());
	// Set item name from the property page list name.
	item->setText(page->getPageName());
	// Set item tool tip from the property page list description.
	item->setToolTip(page->getPageDescription());
	// Restore state of this single page.
	_p->stateSaveRestore(false, page);
}

bool PropertySheetDialog::isSheetModified() const
{
	auto ppl = _p->getPages();
	return std::any_of(ppl.begin(), ppl.end(),[](const PropertyPage* p)
	{
		return p->isPageModified();
	});
}

void PropertySheetDialog::applySheet()
{
	(void)_p->apply();
}

void PropertySheetDialog::checkModified(QWidget* /*origin*/)
{
	// Check if one of the pages has been modified.
	auto modified = isSheetModified();
	// Enable or disable the apply buttons depending on the sheet change.
	_p->ui->btnApply->setEnabled(modified);
	//_p->ui->btnOkay->setEnabled(modified);
}

void PropertySheetDialog::showEvent(QShowEvent* event)
{
	QDialog::showEvent(event);
	// Calling checkModified will enable or disable the buttons.
	checkModified(nullptr);
}

int PropertySheetDialog::exec()
{
	if (_p->ui->listWidget->selectedItems().empty() && _p->ui->listWidget->count())
	{
		_p->ui->listWidget->setCurrentItem(_p->ui->listWidget->item(0));
	}
	return QDialog::exec();
}

}
