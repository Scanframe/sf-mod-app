#include <QUiLoader>
#include <QFormLayout>
#include <QBoxLayout>
#include <misc/gen/ConfigLocation.h>
#include <misc/qt/ActionButton.h>
#include <misc/qt/Globals.h>
#include <misc/qt/qt_utils.h>
#include "AddItemDialog.h"
#include "ui_AddItemDialog.h"

namespace sf
{

AddItemDialog::AddItemDialog(QWidget* parent)
	:QDialog(parent)
	 , ui(new Ui::AddItemDialog)
{
	ui->setupUi(this);
	// Fill the combo box with types.
	ui->cbType->addItems(getAvailableItems());
	// Limit the allowed character for the object name.
	ui->leObjectName->setValidator(new QRegularExpressionValidator(QRegularExpression("[a-zA-Z0-9_]+"), ui->leObjectName));
	// Fill the form layout role combo box.
	for (auto role: {QFormLayout::FieldRole, QFormLayout::LabelRole, QFormLayout::SpanningRole})
	{
		ui->cbFormRole->addItem(enumToKey(role), role);
	}
	//
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [&]()
	{
		if (validated())
		{
			accept();
		}
	});
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool AddItemDialog::validated()
{
	if (ui->leObjectName->text().isEmpty())
	{
		QMessageBox::warning(this, tr("Validation Error"), "Empty object name is not allowed.");
		return false;
	}
	return true;
}

QStringList AddItemDialog::getAvailableItems() const
{
	// Get standard available widgets to remove from the list later except a few.
	QStringList listAllowed;
	for (auto& name: getGlobalUiLoader()->availableWidgets())
	{
		if (name.at(0) != 'Q')
		{
			listAllowed.append(name);
		}
	}
	// Add some standard widgets to the allowed ones.
	listAllowed << "QWidget" << "QFrame" << "QLabel" << "QTabWidget" << "QGroupBox" << "QLineEdit";
	listAllowed.sort(Qt::CaseInsensitive);
	return listAllowed;
}

bool AddItemDialog::hasLabel()
{
	// Get the selected role.
	auto role = ui->cbFormRole->currentData().value<QFormLayout::ItemRole>();
	// Set the flag weather the created item has an accompanied label.
	return (ui->cbFormLabelBuddy->isChecked() || !ui->leFormLabelText->text().isEmpty()) && role == QFormLayout::FieldRole;
}

QObjectList AddItemDialog::execute(QLayout* layout)
{
	QObjectList rv;
	_layout = layout;
	// Sanity check.
	if (!_layout)
	{
		return rv;
	}
	// Find out which type of layout.
	_formLayout = dynamic_cast<QFormLayout*>(layout);
	_boxLayout = dynamic_cast<QBoxLayout*>(layout);
	_gridLayout = dynamic_cast<QGridLayout*>(layout);
	// Display only the stuff needed.
	ui->gbFormLayout->setVisible(!!_formLayout);
	ui->gbGridLayout->setVisible(!!_gridLayout);
	ui->gbBoxLayout->setVisible(!!_boxLayout);
	// Resize to content.
	adjustSize();
	//
	if (_boxLayout)
	{
		auto name = QString("%1 (%2)");
		ui->cbBoxBefore->addItem(tr("Append"), -1);
		for (int index = 0; index < _boxLayout->count(); index++)
		{
			auto item = _boxLayout->itemAt(index);
			if (item && !item->isEmpty())
			{
				if (item->widget())
				{
					ui->cbBoxBefore->addItem(name.arg(item->widget()->metaObject()->className()).arg(item->widget()->objectName()), index);
				}
				else if (item->layout())
				{
					ui->cbBoxBefore->addItem(name.arg(item->layout()->metaObject()->className()).arg(item->layout()->objectName()), index);
				}
			}
		}
	}
	if (_formLayout)
	{
		// First entry is the 'None' for no buddy.
		ui->cbFormBefore->addItem(tr("Append"), -1);
		for (int row = 0; row < _formLayout->rowCount(); row++)
		{
			for (auto role: {QFormLayout::SpanningRole, QFormLayout::FieldRole, QFormLayout::LabelRole})
			{
				auto item = _formLayout->itemAt(row, role);
				if (item && !item->isEmpty())
				{
					auto name = QString("%1 (%2)");
					if (item->widget())
					{
						ui->cbFormBefore->addItem(name.arg(item->widget()->metaObject()->className()).arg(item->widget()->objectName()), row);
						break;
					}
					if (item->layout())
					{
						ui->cbFormBefore->addItem(name.arg(item->layout()->metaObject()->className()).arg(item->layout()->objectName()), row);
						break;
					}
				}
			}
		}
	}
	//
	if (exec() == QDialog::Accepted)
	{
		if (auto child = getGlobalUiLoader()->createWidget(ui->cbType->currentText(), _layout->parentWidget(), ui->leObjectName->text()))
		{
			// Add the child to the returned list of objects.
			rv.append(child);
			// When it concerns a box-layout.
			if (_boxLayout)
			{
				auto index = ui->cbBoxBefore->currentData().toInt();
				if (index < 0)
				{
					_boxLayout->addWidget(child);
				}
				else
				{
					_boxLayout->insertWidget(index, child);
				}
			}
				// When it concerns a form-layout.
			else if (_formLayout)
			{
				// Get the position of before object.
				auto row = ui->cbFormBefore->currentData().toInt();
				// Get the selected role.
				auto role = ui->cbFormRole->currentData().value<QFormLayout::ItemRole>();
				// When row equals -1 and the row is appended/added.
				if (row < 0)
				{
					// Check if a label needs to be added as well.None
					if (hasLabel())
					{
						// Create label.
						auto label = new QLabel(_layout->parentWidget());
						// Add the label to the returned list of objects.
						rv.append(label);
						// Prepend the name with 'lbl' for the label.
						label->setObjectName(ui->leObjectName->text().prepend("lbl"));
						// Added the row using label and child.
						_formLayout->addRow(label, child);
						label->setText(ui->leFormLabelText->text());
						// When this label is also meant as buddy of the added child.
						if (ui->cbFormLabelBuddy->isChecked())
						{
							label->setBuddy(child);
						}
					}
					else
					{
						switch (role)
						{
							case QFormLayout::SpanningRole:
								_formLayout->addRow(child);
								break;

							case QFormLayout::FieldRole:
								_formLayout->addRow(nullptr, child);
								break;

							case QFormLayout::LabelRole:
								_formLayout->addRow(child, (QWidget*) nullptr);
								break;
						}
//						_formLayout->setWidget(pos.first, role, child);
					}
				}
				else
				{
					// Check if a label needs to be added as well.None
					if (hasLabel())
					{
						// Create label.
						auto label = new QLabel(_layout->parentWidget());
						// Add the label to the returned list of objects.
						rv.append(label);
						// Prepend the name with 'lbl' for the label.
						label->setObjectName(ui->leObjectName->text().prepend("lbl"));
						// Added the row using label and child.
						_formLayout->addRow(label, child);
						label->setText(ui->leFormLabelText->text());
						// When this label is also meant as buddy of the added child.
						if (ui->cbFormLabelBuddy->isChecked())
						{
							label->setBuddy(child);
						}
						_formLayout->insertRow(row, label, child);
					}
					else
					{
						_formLayout->insertRow(row, child);
					}
				}
			}
			else
			{
				delete child;
			}
		}
	}
	return rv;
}

}
