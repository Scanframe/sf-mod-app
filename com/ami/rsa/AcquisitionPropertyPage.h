#pragma once
#include <misc/qt/PropertyPage.h>
#include "AcquisitionAppModule.h"

namespace sf
{

namespace Ui {class AcquisitionPropertyPage;}

class AcquisitionPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit AcquisitionPropertyPage(AcquisitionAppModule& am, QWidget* parent);

		~AcquisitionPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

		void stateSaveRestore(QSettings& settings, bool save) override;

	private:
		Ui::AcquisitionPropertyPage* ui;

		AcquisitionAppModule& _am;
};

}
