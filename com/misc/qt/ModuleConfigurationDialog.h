#pragma once

#include <QDialog>
#include <QSettings>

#include "../global.h"

namespace Ui {class ModuleConfigurationDialog;}

namespace sf
{

class AppModuleList;
class ModuleConfiguration;

class _MISC_CLASS ModuleConfigurationDialog :public QDialog
{
	Q_OBJECT

	public:

		explicit ModuleConfigurationDialog(ModuleConfiguration* config, QWidget* parent = nullptr);

		~ModuleConfigurationDialog() override;

		void applyClose();

		void apply();

		Ui::ModuleConfigurationDialog* ui;

		AppModuleList* _moduleList;
};

}