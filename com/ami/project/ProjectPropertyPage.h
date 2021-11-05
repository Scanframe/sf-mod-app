#pragma once
#include <misc/qt/PropertyPage.h>
#include "ProjectAppModule.h"

namespace sf
{

namespace Ui {class ProjectPropertyPage;}

class ProjectPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit ProjectPropertyPage(ProjectAppModule& am, QWidget* parent);

		~ProjectPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

	private:
		Ui::ProjectPropertyPage* ui;

		ProjectAppModule& _am;
};

}
