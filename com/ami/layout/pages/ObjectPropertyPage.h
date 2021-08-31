#pragma once

#include <misc/qt/PropertyPage.h>
#include <QWidget>

namespace sf
{
namespace Ui {class ObjectPropertyPage;}

class ObjectPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit ObjectPropertyPage(QObject* target, QWidget* parent = nullptr);

		~ObjectPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::ObjectPropertyPage* ui;
		QObject* _target;
};

}

