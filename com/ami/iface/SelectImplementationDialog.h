#pragma once

#include <QDialog>
#include "global.h"
#include "AppModuleInterface.h"

namespace sf
{

namespace Ui {class SelectImplementationDialog;}

class _AMI_CLASS SelectImplementationDialog :public QDialog
{
	Q_OBJECT

	public:
		explicit SelectImplementationDialog(QWidget* parent = nullptr);

		~SelectImplementationDialog() override;

		[[nodiscard]] AppModuleInterface* getSelected() const;

	private:
		Ui::SelectImplementationDialog* ui;

		AppModuleInterface* _selected{};

		void okay();

		void doubleClicked(const QModelIndex& index);
};

}


