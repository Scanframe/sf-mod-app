#pragma once

#include <misc/qt/PropertyPage.h>
#include <QWidget>

class QFormLayout;
class QGridLayout;
class QBoxLayout;

namespace sf
{

namespace Ui {class PositionPropertyPage;}

class PositionPropertyPage :public PropertyPage
{
	Q_OBJECT

	public:
		explicit PositionPropertyPage(QObject* target, QWidget* parent = nullptr);

		~PositionPropertyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		QIcon getPageIcon() const override;

		void applyPage() override;

		void updatePage() override;

	private:
		Ui::PositionPropertyPage* ui;
		QObject* _target;
		QLayout* _targetLayout;
		QWidget* _targetWidget;
		QLayout* _layout{nullptr};
		QFormLayout* _formLayout{nullptr};
		QBoxLayout* _boxLayout{nullptr};
		QGridLayout* _gridLayout{nullptr};
};

}

