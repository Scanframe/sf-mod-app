#pragma once
#include <misc/qt/PropertyPage.h>
#include "UnitConversionAppModule.h"

namespace sf
{

namespace Ui {class UnitConversionPropertyPage;}

class ConversionListModel;
class FollowersListModel;

class UnitConversionPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit UnitConversionPropertyPage(UnitConversionAppModule& ucm, QWidget* parent);

		~UnitConversionPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

		void stateSaveRestore(QSettings& settings, bool save) override;

	private:
		Ui::UnitConversionPropertyPage* ui;

		UnitConversionAppModule& _ucm;

		QAction* _actionAddConversion{nullptr};
		QAction* _actionRemoveConversion{nullptr};
		QAction* _actionEditConversion{nullptr};
		QAction* _actionAddFollower{nullptr};
		QAction* _actionRemoveFollower{nullptr};
		QAction* _actionEditFollower{nullptr};

		ConversionListModel* _lmConversions{nullptr};
		FollowersListModel* _lmFollowers{nullptr};
};

}