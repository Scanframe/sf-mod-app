#pragma once

#include <QMap>
#include <ami/iface/AppModuleInterface.h>
#include <ami/iface/MultiDocInterface.h>
#include <misc/qt/PropertySheetDialog.h>

namespace sf
{

class [[maybe_unused]] TextEditorAppModule :public AppModuleInterface
{
	public:

		explicit TextEditorAppModule(const Parameters& params);

		void initialize(InitializeStage stage) override;

		void addPropertyPages(PropertySheetDialog*) override;

		MultiDocInterface* createWidget(QWidget* parent) const override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;
};


}
