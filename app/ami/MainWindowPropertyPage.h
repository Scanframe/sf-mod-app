#pragma once

#include <QWidget>
#include <misc/qt/PropertyPage.h>
#include "MainWindow.h"

namespace Ui {class MainWindowPropertyPage;}

namespace sf
{

class MainWindowPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit MainWindowPropertyPage(MainWindow* mainWindow, PropertySheetDialog* parent = nullptr);

		~MainWindowPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		[[nodiscard]] bool isPageModified() const override;

		void applyPage() override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::MainWindowPropertyPage* ui;
		MainWindow* _mainWindow;
};

}

