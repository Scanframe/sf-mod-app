#pragma once

#include <QMap>
#include <ami/iface/AppModuleInterface.h>
#include <misc/qt/PropertySheetDialog.h>
#include <misc/qt/Editor.h>
#include "ScriptManager.h"

namespace sf
{

class [[maybe_unused]] ScriptAppModule :public AppModuleInterface
{
	public:

		explicit ScriptAppModule(const Parameters& params);

		void initialize() override;

		void addPropertyPages(PropertySheetDialog*) override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

	protected:
		MultiDocInterface* createWidget(QWidget* parent) const override;

		void openEditor(int index);

	private:
		ScriptManager& _scriptManager;
		Editor::Configuration _configuration;
};

}
