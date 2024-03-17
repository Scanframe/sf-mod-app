#pragma once

#include <QDialog>

class QFormLayout;

class QGridLayout;

class QBoxLayout;

class QAbstractItemModel;

class QUiLoader;

namespace sf
{

namespace Ui {class AddItemDialog;}

class AddItemDialog :public QDialog
{
	public:
		explicit AddItemDialog(QWidget* parent);

		QObjectList execute(QLayout* layout);

	private:

		[[nodiscard]] bool hasLabel();

		[[nodiscard]] QStringList getAvailableItems() const;

		Ui::AddItemDialog* ui;
		QLayout* _layout{nullptr};
		QFormLayout* _formLayout{nullptr};
		QBoxLayout* _boxLayout{nullptr};
		QGridLayout* _gridLayout{nullptr};

		bool validated();
};

}
