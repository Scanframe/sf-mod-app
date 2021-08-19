#pragma once

#include <QMap>
#include <ami/iface/AppModuleInterface.h>
#include <ami/iface/MultiDocInterface.h>

namespace sf
{

class [[maybe_unused]] LayoutEditorAppModule :public AppModuleInterface
{
	public:

		explicit LayoutEditorAppModule(const Parameters& params);

		void addPropertyPages(PropertySheetDialog*) override;

		MultiDocInterface* createWidget(QWidget* parent) const override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		bool _readOnly = true;

		void settingsReadWrite(bool save);
};

}
