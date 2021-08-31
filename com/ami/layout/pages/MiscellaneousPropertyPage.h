#pragma once

#include <misc/qt/PropertyPage.h>
#include <QWidget>
#include <QLineEdit>

namespace sf
{
namespace Ui {class MiscellaneousPropertyPage;}

class MiscellaneousPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit MiscellaneousPropertyPage(QObject* target, QWidget* parent = nullptr);

		~MiscellaneousPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::MiscellaneousPropertyPage* ui;
		QObject* _target;
		QLineEdit* _targetLineEdit;
};

}

