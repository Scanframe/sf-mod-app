#pragma once

#include <misc/qt/PropertyPage.h>
#include <QWidget>

namespace sf
{
namespace Ui {class WidgetPropertyPage;}

class WidgetPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit WidgetPropertyPage(QWidget* target, QWidget* parent = nullptr);

		~WidgetPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		void applyPage() override;

		void updatePage() override;

		void stateSaveRestore(QSettings& settings, bool save) override;

	private:
		Ui::WidgetPropertyPage* ui;
		QWidget* _target;
};

}

