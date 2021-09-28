#pragma once

#include <misc/qt/PropertyPage.h>
#include <gii/qt/VariableWidgetBase.h>

namespace sf
{
namespace Ui {class VariableIdPropertyPage;}

class VariableIdPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit VariableIdPropertyPage(VariableWidgetBase* target, QWidget* parent = nullptr);

		~VariableIdPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::VariableIdPropertyPage* ui;
		VariableWidgetBase* _target{nullptr};
		bool _hasNameLevel{false};
};

}

