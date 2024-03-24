#pragma once
#include <misc/qt/PropertyPage.h>
#include "LayoutEditorAppModule.h"

namespace sf
{

namespace Ui {class LayoutEditorPropertyPage;}

class LayoutEditorPropertyPage :public PropertyPage
{
		Q_OBJECT

	public:
		explicit LayoutEditorPropertyPage(LayoutEditorAppModule& ucm, QWidget* parent);

		~LayoutEditorPropertyPage() override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] QIcon getPageIcon() const override;

		void updatePage() override;

		void applyPage() override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

		void stateSaveRestore(QSettings& settings, bool save) override;

	private:
		Ui::LayoutEditorPropertyPage* ui;

		LayoutEditorAppModule& _lem;
};

}
