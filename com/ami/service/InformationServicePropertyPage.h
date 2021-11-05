#pragma once
#include "InformationServiceAppModule.h"
#include <misc/qt/PropertyPage.h>

namespace sf
{

namespace Ui {class InformationServicePropertyPage;}

class InformationServicePropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit InformationServicePropertyPage(InformationServiceAppModule& ucm, QWidget* parent);

		~InformationServicePropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::InformationServicePropertyPage* ui;

		InformationServiceAppModule& _ism;
};

}