#pragma once
#include "ServiceAppModule.h"
#include <misc/qt/PropertyPage.h>

namespace sf
{

namespace Ui {class ServicePropertyPage;}

class ServicePropertyPage :public PropertyPage
{
	public:
		explicit ServicePropertyPage(ServiceAppModule& ucm, QWidget* parent);

		~ServicePropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::ServicePropertyPage* ui;

		ServiceAppModule& _ism;

	// LLDB Crashes and GDB freezes when this MACRO is in top of the class.
	Q_OBJECT
};

}