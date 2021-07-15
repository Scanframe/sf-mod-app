#pragma once

#include <QWidget>
#include <misc/qt/PropertyPage.h>

namespace Ui {class ApplicationPropertyPage;}

namespace sf
{

class ApplicationPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit ApplicationPropertyPage(PropertySheetDialog* parent = nullptr);

		~ApplicationPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		bool isPageModified() const override;

		void applyPage() override;

		void afterPageApply() override;

	private:
		Ui::ApplicationPropertyPage* ui;
};

}

