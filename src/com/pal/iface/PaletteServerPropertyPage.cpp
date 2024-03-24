#include "PaletteServerPropertyPage.h"
#include "ui_PaletteServerPropertyPage.h"
#include <pal/iface/PaletteServer.h>
#include <gii/qt/Macros.h>
#include <gii/qt/InformationSelectDialog.h>
#include <misc/qt/Resource.h>
#include <misc/qt/ObjectPropertyModel.h>
#include <QLineEdit>
#include <misc/qt/qt_utils.h>

namespace sf
{

PaletteServerPropertyPage::PaletteServerPropertyPage(PaletteServer* server, PropertySheetDialog* parent)
	:PropertyPage(parent)
	 , ui(new Ui::PaletteServerPropertyPage)
	, _server(server)
	, _propChange(false)
{
	ui->setupUi(this);
	//
	auto model = new ObjectPropertyModel();
	model->setDelegates(ui->tvProperties);
	// When the list view is edited set the change flag.
	connect(model, &ObjectPropertyModel::changed, [&]()
	{
		_propChange |= true;
	});
	// Make the qulonglong type QProperties have actions.
	connect(model, &ObjectPropertyModel::addLineEditActions, [](QLineEdit* lineEdit, QObject* obj, int propertyIndex, bool dynamic)
	{
		for (auto isType: {Gii::ResultData, Gii::Variable})
		{
			auto action = lineEdit->addAction(
				Resource::getSvgIcon(isType == Gii::Variable ? ":icon/svg/variable" : ":icon/svg/resultdata", lineEdit->palette(), QPalette::Text),
				QLineEdit::TrailingPosition);
			connect(action, &QAction::triggered, [action, isType]()
			{
				if (auto le = qobject_cast<QLineEdit*>(action->parent()))
				{
					InformationSelectDialog dlg(le);
					auto ids = dlg.execute(Gii::Single, isType);
					if (!ids.isEmpty())
					{
						le->setText(QString("0x%1").arg(ids.first(), 0, 16));
					}
				}
			});
		}
	});
	ui->tvProperties->setModel(model);
	ui->tvProperties->setAlternatingRowColors(true);
	resizeColumnsToContents(ui->tvProperties);
	// Add no implementation selection item.
	ui->cbImplementationName->addItem(tr("None"), "");
	// Iterate through the implementation name descriptions pairs.
	for(auto& name: _server->getImplementationNames())
	{
		ui->cbImplementationName->addItem(QString("%1: %2").arg(name.first).arg(name.second), name.first);
	}
}

PaletteServerPropertyPage::~PaletteServerPropertyPage()
{
	delete ui;
}

QIcon PaletteServerPropertyPage::getPageIcon() const
{
	return Resource::getSvgIcon(Resource::getSvgIconResource(Resource::Palette), QPalette::Text);
}

QString PaletteServerPropertyPage::getPageName() const
{
	return tr("Palette Server");
}

QString PaletteServerPropertyPage::getPageDescription() const
{
	return tr("Palette selection and configuration.");
}

void PaletteServerPropertyPage::applyPage()
{
	_propChange = false;
	_server->setImplementationName(ui->cbImplementationName->currentData().toString());
}

bool PaletteServerPropertyPage::isPageModified() const
{
	bool rv = _propChange;
	rv |= _server->getImplementationName() != ui->cbImplementationName->currentData().toString();
	return rv;
}

void PaletteServerPropertyPage::updatePage()
{
	ui->cbImplementationName->setCurrentIndex(clip<int>(ui->cbImplementationName->findData(_server->getImplementationName()), 0, 99));
	dynamic_cast<ObjectPropertyModel*>(ui->tvProperties->model())->setTarget(_server->_implementation);
	resizeColumnsToContents(ui->tvProperties);
}

void PaletteServerPropertyPage::afterPageApply(bool was_modified)
{
	if (was_modified)
	{
		dynamic_cast<ObjectPropertyModel*>(ui->tvProperties->model())->setTarget(_server->_implementation);
		resizeColumnsToContents(ui->tvProperties);
	}
}

}

