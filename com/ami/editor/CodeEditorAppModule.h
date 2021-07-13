#pragma once

#include <QMap>
#include <ami/iface/AppModuleInterface.h>
#include <ami/iface/MultiDocInterface.h>

namespace sf
{

class [[maybe_unused]] CodeEditorAppModule :public AppModuleInterface
{
	public:

		explicit CodeEditorAppModule(const Parameters& params);

		void addPropertySheets(QWidget*) override;

		MultiDocInterface* createWidget(QWidget* parent) const override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getSvgIconResource() const override;
};

}
