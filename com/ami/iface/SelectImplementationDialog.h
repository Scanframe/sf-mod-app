#pragma once

#include <QDialog>
#include <QSettings>
#include "AppModuleInterface.h"
#include "global.h"

namespace sf
{

namespace Ui {class SelectImplementationDialog;}

class _AMI_CLASS SelectImplementationDialog :public QDialog
{
	Q_OBJECT

	public:
		explicit SelectImplementationDialog(QSettings* settings, bool file_only, QWidget* parent);

		~SelectImplementationDialog() override;

		[[nodiscard]] AppModuleInterface* getSelected() const;

		static AppModuleInterface* execute(QSettings* settings, bool file_only, QWidget* parent);

	private:

		void stateSaveRestore(bool save);

		Ui::SelectImplementationDialog* ui;

		QSettings* _settings;

		AppModuleInterface* _selected{nullptr};
};

}


