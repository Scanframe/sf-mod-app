#pragma once

#include <QDialog>
#include <QSettings>

#include "../global.h"

namespace Ui {class ModuleConfigurationDialog;}

namespace sf
{

// Forward definition.
class ModuleConfiguration;

// Forward definition.
class AppModuleList;

class _MISC_CLASS ModuleConfigurationDialog :public QDialog
{
	Q_OBJECT

	public:

		explicit ModuleConfigurationDialog(ModuleConfiguration* config, QWidget* parent = nullptr);

		void stateSaveRestore(bool save);

		~ModuleConfigurationDialog() override;

		void applyClose();

		Ui::ModuleConfigurationDialog* ui;

		AppModuleList* _moduleList;

		ModuleConfiguration* _config;
};

}