#pragma once

#include <misc/qt/PropertyPage.h>

namespace sf
{

namespace Ui {class ContainerPropertyPage;}

class ContainerPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit ContainerPropertyPage(QWidget* target, QWidget* parent = nullptr);

		~ContainerPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		void updatePage() override;

		[[nodiscard]] bool isPageModified() const override;

		void applyPage() override;

	private:
		Ui::ContainerPropertyPage* ui;
		QWidget* _target;
};

}
