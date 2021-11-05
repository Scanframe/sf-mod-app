#pragma once

#include <ami/iface/AppModuleInterface.h>
#include <gii/gen/InformationServer.h>
#include "InformationService.h"

namespace sf
{

class InformationServiceAppModule :public AppModuleInterface
{
	Q_OBJECT

	public:

		explicit InformationServiceAppModule(const Parameters& params);

		~InformationServiceAppModule() override;

		void initialize(InitializeStage stage) override;

		void addPropertyPages(PropertySheetDialog*) override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		void settingsReadWrite(bool save);

		QSettings* _settings;

		InformationService& _informationService;
};

}
