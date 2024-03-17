#pragma once

#include <misc/qt/PropertyPage.h>
#include <qt/IdPropertyList.h>
#include "AcquisitionControl.h"

namespace sf
{
namespace Ui {class AcquisitionControlPropertyPage;}

class AcquisitionControlPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit AcquisitionControlPropertyPage(AcquisitionControl* target, QWidget* parent = nullptr);

		~AcquisitionControlPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		QIcon getPageIcon() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::AcquisitionControlPropertyPage* ui;
		AcquisitionControl* _target{nullptr};
		IdPropertyList _idPropertyList;
};

}

