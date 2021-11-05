#pragma once

#include "BscanGraph.h"
#include <qt/IdPropertyList.h>
#include <misc/qt/PropertyPage.h>

namespace sf
{

namespace Ui {class BscanPropertyPage;}

class BscanPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit BscanPropertyPage(BscanGraph* target, QWidget* parent = nullptr);

		~BscanPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		QIcon getPageIcon() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::BscanPropertyPage* ui;
		BscanGraph* _target{nullptr};
		IdPropertyList _idPropertyList;
};

}
