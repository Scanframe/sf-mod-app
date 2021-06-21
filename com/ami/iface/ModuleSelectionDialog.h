#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui {class ModuleSelectionDialog;}


namespace sf
{

class AppModuleList;

class ModuleSelectionDialog :public QDialog
{
	Q_OBJECT

	public:

		static ModuleSelectionDialog* getDialog(QSettings* settings, QWidget* parent);

		~ModuleSelectionDialog() override;

	private:
		explicit ModuleSelectionDialog(QSettings* settings, QWidget* parent = nullptr);

		void applyClose();

		Ui::ModuleSelectionDialog* ui;

		QSettings _settings;

		AppModuleList* _moduleList;

		static ModuleSelectionDialog* _singleton;
};

}