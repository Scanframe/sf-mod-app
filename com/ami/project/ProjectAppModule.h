#pragma once

#include <ami/iface/AppModuleInterface.h>
#include <rsa/iface/RsaServer.h>
#include <gen/Variable.h>
#include <ipj/ProjectConfig.h>

namespace sf
{

class ProjectAppModule :public AppModuleInterface
{
	Q_OBJECT

	public:

		explicit ProjectAppModule(const Parameters& params);

		~ProjectAppModule() override;

		void initialize(InitializeStage stage) override;

		void addPropertyPages(PropertySheetDialog*) override;

		void addMenuItems(MenuType menuType, QMenu* menu) override;

		void addToolBars(QMainWindow* mainWindow) override;

		[[nodiscard]] QString getName() const override;

		[[nodiscard]] QString getDescription() const override;

		[[nodiscard]] QString getLibraryFilename() const override;

		[[nodiscard]] QString getSvgIconResource() const override;

		void settingsReadWrite(bool save);

		void createDevices();

		ProjectConfig* _projectConfig{nullptr};

		QSettings* _settings{nullptr};

		QString _serverUtName;

		QString _serverEtName;

		QString _serverMotionName;

		QString _serverStorageName;

		QAction* _actionMonitorVariable{nullptr};

		QAction* _actionMonitorResultData{nullptr};

		QAction* _actionSaveSettings{nullptr};

		QAction* _actionLoadSettings{nullptr};

		QString _settingsFilename;

		QString _currentSettingsFile;

		[[nodiscard]] QStringList getSettingsFilenames() const;
};

}
