#pragma once

#include <misc/qt/PropertySheetDialog.h>
#include "PaletteServer.h"

namespace sf
{

namespace Ui {class PaletteServerPropertyPage;}

class PaletteServer;

class PaletteServerPropertyPage :public PropertyPage
{
	Q_GADGET
	public:
		explicit PaletteServerPropertyPage(PaletteServer* mainWindow, PropertySheetDialog* parent = nullptr);

		~PaletteServerPropertyPage() override;

		[[nodiscard]] QIcon getPageIcon() const override;

		[[nodiscard]] QString getPageName() const override;

		[[nodiscard]] QString getPageDescription() const override;

		[[nodiscard]] bool isPageModified() const override;

		void afterPageApply(bool was_modified) override;

		void updatePage() override;

		void applyPage() override;

	private:
		Ui::PaletteServerPropertyPage* ui;
		PaletteServer* _server;
		bool _propChange;
};

}
