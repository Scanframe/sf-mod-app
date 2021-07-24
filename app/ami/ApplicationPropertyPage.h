#pragma once

#include <QWidget>
#include <misc/qt/PropertyPage.h>
#include "MainWindow.h"

namespace Ui {class ApplicationPropertyPage;}

namespace sf
{

class ApplicationPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit ApplicationPropertyPage(MainWindow* main_win, PropertySheetDialog* parent = nullptr);

		~ApplicationPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		[[nodiscard]] bool isPageModified() const override;

		void applyPage() override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::ApplicationPropertyPage* ui;
		MainWindow* _mainWindow;
};

}

