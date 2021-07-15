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
		explicit SelectImplementationDialog(QSettings* settings, QWidget* parent = nullptr);

		~SelectImplementationDialog() override;

		[[nodiscard]] AppModuleInterface* getSelected() const;

	private:
		void okay();

		void doubleClicked(const QModelIndex& index);

		void stateSaveRestore(bool save);

		Ui::SelectImplementationDialog* ui;

		AppModuleInterface* _selected{};

		QSettings* _settings;
};

}


