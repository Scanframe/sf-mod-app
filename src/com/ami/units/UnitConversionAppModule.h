#pragma once

#include <ami/iface/AppModuleInterface.h>
#include <gii/gen/UnitConversionServerEx.h>

namespace sf
{

class UnitConversionAppModule :public AppModuleInterface
{
	Q_OBJECT

	public:

		explicit UnitConversionAppModule(const Parameters& params);

		~UnitConversionAppModule() override;

		void initialize(InitializeStage stage) override;

		void addPropertyPages(PropertySheetDialog*) override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		QString _configFilePath;

		UnitConversionServerEx _ucs;

		void settingsReadWrite(bool save);

		QSettings* _settings;

		enum UnitSystem :int
		{
			PassThrough = UnitConversionServer::usPassThrough,
			Metric = UnitConversionServer::usMetric,
			Imperial = UnitConversionServer::usImperial
		};

		Q_ENUM(UnitSystem);

		bool unitConversionHandler(UnitConversionEvent& ev) const;

		bool _ask{true};
};

}
