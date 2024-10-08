#include "FollowersDialog.h"
#include "ui_FollowersDialog.h"
#include <QInputDialog>
#include <QPushButton>
#include <QTimer>
#include <misc/qt/Resource.h>
#include <qt/InformationSelectDialog.h>

namespace sf
{

FollowersDialog::FollowersDialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::FollowersDialog)
{
	ui->setupUi(this);
	//
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Cancel), QPalette::ButtonText));
	ui->buttonBox->button(QDialogButtonBox::Ok)->setIcon(Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Icon::Okay), QPalette::ButtonText));
	//
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &FollowersDialog::close);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &FollowersDialog::okay);
	//
	QPalette palette = ui->lblMessage->palette();
	palette.setColor(ui->lblMessage->backgroundRole(), QColorConstants::Svg::red);
	palette.setColor(ui->lblMessage->foregroundRole(), QColorConstants::Svg::red);
	ui->lblMessage->setPalette(palette);
	ui->lblMessage->setVisible(false);

	// Create the actions for the buttons.
	for (auto& t: std::vector<std::tuple<QToolButton*, QAction*&, Resource::Icon, QString, QString>>{
				 {ui->tbEdit, _actionEdit, Resource::Edit, tr("Edit"), tr("Edit the selected entry.")},
				 {ui->tbAdd, _actionAdd, Resource::Add, tr("Add"), tr("Add entry.")},
				 {ui->tbRemove, _actionRemove, Resource::Remove, tr("Remove"), tr("Remove the selected entry.")},
			 })
	{
		std::get<1>(t) = new QAction(
			Resource::getSvgIcon(Resource::getSvgIconResource(std::get<2>(t)), QPalette::ButtonText),
			std::get<3>(t),
			this
		);
		std::get<1>(t)->setToolTip(std::get<4>(t));
		std::get<0>(t)->setDefaultAction(std::get<1>(t));
		//std::get<0>(t)->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
		ui->lwFollowers->addAction(std::get<1>(t));
	}
	//
	connect(_actionEdit, &QAction::triggered, [&]() {
		auto si = ui->lwFollowers->selectedItems();
		if (!si.empty())
		{
			auto s = QInputDialog::getText(this, tr("Edit Follower"), tr("Follower id"), QLineEdit::Normal, si[0]->text());
			if (!s.isEmpty())
			{
				si[0]->setText(s);
			}
		}
	});
	connect(_actionAdd, &QAction::triggered, [&]() {
		for (auto id: InformationSelectDialog(this).execute(Gii::Multiple))
		{
			ui->lwFollowers->addItem(QString("0x%1").arg(id, 0, 16));
		}
	});
	connect(_actionRemove, &QAction::triggered, [&]() {
		for (auto item: ui->lwFollowers->selectedItems())
		{
			delete ui->lwFollowers->takeItem(ui->lwFollowers->row(item));
		}
	});
}

FollowersDialog::~FollowersDialog()
{
	delete ui;
}

bool FollowersDialog::checkContent()
{
	// Remove unwanted characters.
	ui->leUnit->setText(ui->leUnit->text().remove(' ').remove(','));
	ui->leFormula->setText(ui->leFormula->text().remove('\''));
	if (ui->leUnit->text().isEmpty())
	{
		ui->lblMessage->setText("unit cannot be empty!");
		ui->lblMessage->setVisible(true);
		QTimer::singleShot(5000, [&]() {
			ui->lblMessage->setVisible(false);
		});
		return false;
	}
	return true;
}

void FollowersDialog::okay()
{
	if (checkContent())
	{
		done(QDialog::Accepted);
	}
}

bool FollowersDialog::execute(FollowersListModel::Entry& fle)
{
	ui->leMasterId->setText(QString("0x%1").arg(fle._masterId, 0, 16));
	ui->leFormula->setText(fle._formula);
	ui->leUnit->setText(fle._unit);
	for (auto id: fle._followerIds)
	{
		Variable var(id, false);
		//		ui->lwFollowers->addItem(QString("0x%1 %2").arg(id, 0, 16).arg(QString::fromStdString(var.getName())));
		auto item = new QListWidgetItem(QString("0x%1").arg(id, 0, 16), ui->lwFollowers);
		ui->lwFollowers->addItem(item);
	}
	//
	if (exec() == QDialog::Accepted)
	{
		fle._masterId = ui->leMasterId->text().toULongLong(nullptr, 0);
		fle._unit = ui->leUnit->text().trimmed();
		fle._formula = ui->leFormula->text().trimmed();
		fle._followerIds.clear();
		for (int i = 0; i < ui->lwFollowers->count(); i++)
		{
			auto item = ui->lwFollowers->item(i);
			auto sl = item->text().split(' ');
			if (!sl.empty())
			{
				Variable::id_type id = sl.at(0).toULongLong(nullptr, 0);
				if (id)
				{
					fle._followerIds.append(id);
				}
			}
		}
		return true;
	}

	return false;
}

}// namespace sf
