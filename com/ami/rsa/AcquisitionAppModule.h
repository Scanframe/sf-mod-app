#pragma once

#include <ami/iface/AppModuleInterface.h>
#include <rsa/iface/RsaServer.h>
#include <gen/Variable.h>

namespace sf
{

class AcquisitionAppModule :public AppModuleInterface
{
	Q_OBJECT

	public:

		explicit AcquisitionAppModule(const Parameters& params);

		~AcquisitionAppModule() override;

		void initialize(bool init) override;

		void addPropertyPages(PropertySheetDialog*) override;

		void addMenuItems(MenuType menuType, QMenu* menu) override;

		void addToolBars(QMainWindow* mainWindow) override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		void settingsReadWrite(bool save);

		void createDevices();

		QSettings* _settings{nullptr};

		Variable _vSwitch;

		RsaServer* _serverUt{nullptr};

		RsaServer* _serverEt{nullptr};

		QString _serverUtName;

		QString _serverEtName;

		QAction* _actionMonitorVariable{nullptr};

		QAction* _actionMonitorResultData{nullptr};
};

}
