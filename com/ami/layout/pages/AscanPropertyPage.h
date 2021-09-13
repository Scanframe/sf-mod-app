#pragma once

#include <misc/qt/PropertyPage.h>
#include <gii/qt/AscanGraph.h>
#include <qt/IdPropertyList.h>

namespace sf
{
namespace Ui {class AscanPropertyPage;}

class AscanPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit AscanPropertyPage(AscanGraph* target, QWidget* parent = nullptr);

		~AscanPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::AscanPropertyPage* ui;
		AscanGraph* _target{nullptr};
		IdPropertyList _idPropertyList;
};

}

