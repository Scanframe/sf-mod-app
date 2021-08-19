#include <misc/qt/Resource.h>
#include <gii/gen/UnitConversionServerEx.h>
#include "UnitConversionPropertyPage.h"
#include "ConversionListModel.h"
#include "FollowersListModel.h"
#include "ui_UnitConversionPropertyPage.h"

namespace sf
{

UnitConversionPropertyPage::UnitConversionPropertyPage(UnitConversionAppModule& ucm, QWidget* parent)
	:PropertyPage(parent), ui(new Ui::UnitConversionPropertyPage)
	 , _ucm(ucm)
{
	ui->setupUi(this);
	//
	for (auto us: {UnitConversionServerEx::usPassThrough, UnitConversionServerEx::usMetric, UnitConversionServerEx::usImperial})
	{
		ui->cbSystem->addItem(QString::fromStdString(UnitConversionServerEx::getUnitSystemName(us)), us);
	}
	// Create the actions for the buttons.
	for (auto& t: std::vector<std::tuple<QAbstractItemView*, QToolButton*, QAction*&, Resource::Icon, QString, QString>>
		{
			{ui->tvConversions, ui->tbEditConversion, _actionEditConversion, Resource::Edit, tr("Edit"), tr("Edit the selected conversion entry.")},
			{ui->tvConversions, ui->tbAddConversion, _actionAddConversion, Resource::Add, tr("Add"), tr("Add a conversion entry.")},
			{ui->tvConversions, ui->tbRemoveConversion, _actionRemoveConversion, Resource::Remove, tr("Remove"), tr("Remove the selected entry.")},

			{ui->tvFollowers, ui->tbEditFollower, _actionEditFollower, Resource::Edit, tr("Edit"), tr("Edit the selected entry.")},
			{ui->tvFollowers, ui->tbAddFollower, _actionAddFollower, Resource::Add, tr("Add"), tr("Add a master or follower entry.")},
			{ui->tvFollowers, ui->tbRemoveFollower, _actionRemoveFollower, Resource::Remove, tr("Remove"), tr("Remove the selected entry.")},
		})
	{
		std::get<2>(t) = new QAction(
			Resource::getSvgIcon(Resource::getSvgIconResource(std::get<3>(t)), QPalette::ButtonText),
			std::get<4>(t),
			this
		);
		std::get<2>(t)->setToolTip(std::get<5>(t));
		std::get<1>(t)->setDefaultAction(std::get<2>(t));
		//std::get<1>(t)->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
		std::get<0>(t)->addAction(std::get<2>(t));
	}
	// Function to get the selected item in an item view widget.
	auto getIndex = [&](QAbstractItemView* tv) -> QModelIndex
	{
		auto list = tv->selectionModel()->selectedIndexes();
		return list.isEmpty() ? QModelIndex() : list.first();
	};
	//
	_lmConversions = new ConversionListModel(&_ucm._ucs, this);
	_lmFollowers = new FollowersListModel(&_ucm._ucs, this);
	//
	ui->tvConversions->setModel(_lmConversions);
	ui->tvFollowers->setModel(_lmFollowers);
	//
	connect(ui->tvConversions, &QAbstractItemView::doubleClicked, [&](const QModelIndex& index)
	{
		_lmConversions->edit(index);
	});
	//
	connect(ui->tvFollowers, &QAbstractItemView::doubleClicked, [&](const QModelIndex& index)
	{
		_lmFollowers->edit(index);
	});
	//
	connect(_actionEditConversion, &QAction::triggered, [&](){_lmConversions->edit(getIndex(ui->tvConversions));});
	connect(_actionAddConversion, &QAction::triggered, [&](){_lmConversions->add();});
	connect(_actionRemoveConversion, &QAction::triggered, [&](){_lmConversions->remove(getIndex(ui->tvConversions));});
	//
	connect(_actionEditFollower, &QAction::triggered, [&](){_lmFollowers->edit(getIndex(ui->tvFollowers));});
	connect(_actionAddFollower, &QAction::triggered, [&](){_lmFollowers->add();});
	connect(_actionRemoveFollower, &QAction::triggered, [&](){_lmFollowers->remove(getIndex(ui->tvFollowers));});
}

UnitConversionPropertyPage::~UnitConversionPropertyPage()
{
	delete ui;
}

QString UnitConversionPropertyPage::getPageName() const
{
	return tr("Unit Conversion");
}

QString UnitConversionPropertyPage::getPageDescription() const
{
	return tr("Configure the unit conversion static and followers");
}

QIcon UnitConversionPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(":icon/svg/units", QPalette::ColorRole::Text);
}

void UnitConversionPropertyPage::updatePage()
{
	ui->cbAsk->setChecked(_ucm._ask);
	ui->leEnableId->setText(QString("0x%1").arg(_ucm._ucs.getEnableId(), 0, 16));
	ui->cbSystem->setCurrentIndex(_ucm._ucs.getUnitSystem());
}

void UnitConversionPropertyPage::applyPage()
{
	// Write the changes when dirty.
	_lmConversions->sync();
	// Write the changes when dirty.
	_lmFollowers->sync();
	//
	_ucm._ask = ui->cbAsk->isChecked();
	_ucm._ucs.setEnableId(ui->leEnableId->text().toLongLong(nullptr, 0));
	// This must be done last so the system can use its new settings.
	_ucm._ucs.setUnitSystem(static_cast<UnitConversionServerEx::EUnitSystem>(ui->cbSystem->currentIndex()));

}

bool UnitConversionPropertyPage::isPageModified() const
{
	bool rv = false;
	rv |= _ucm._ask != ui->cbAsk->isChecked();
	rv |= _ucm._ucs.getEnableId() != ui->leEnableId->text().toLongLong(nullptr, 0);
	rv |= _ucm._ucs.getUnitSystem() != ui->cbSystem->currentIndex();
	rv |= _lmConversions->isDirty();
	rv |= _lmFollowers->isDirty();
	return rv;
}

void UnitConversionPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		_ucm.settingsReadWrite(true);
		_lmConversions->refresh();
		_lmFollowers->refresh();
	}
}

void UnitConversionPropertyPage::stateSaveRestore(QSettings& settings, bool save)
{
	if (save)
	{
		settings.setValue(ui->tabWidget->objectName(), ui->tabWidget->currentIndex());
	}
	else
	{
		auto index = settings.value(ui->tabWidget->objectName(), ui->tabWidget->currentIndex()).toInt();
		ui->tabWidget->setCurrentIndex(index);
	}
}

}
