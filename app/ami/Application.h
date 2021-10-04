#pragma once

#include <QApplication>
#include <QCommandLineParser>
#include <SingleInstance>
#include <misc/qt/qt_utils.h>
#include <misc/qt/ModuleConfiguration.h>
#include "MainWindow.h"

namespace sf
{

class Application :public QApplication
{
	public:
		Application(int& argc, char** argv, int flags = ApplicationFlags);

		~Application() override;

		ModuleConfiguration& getModuleConfiguration();

		void initialize(AppModuleInterface::InitializeStage stage);

		MainWindow* getMainWindow();

		void parseCommandline();

		void processCommandLine();

	private:

		std::string ConfigLocationHandler(const std::string& option);

		void handleInstanceMessage(quint32 instanceId, const QByteArray& message);

		ModuleConfiguration* _moduleConfiguration;

		MainWindow* _mainWindow{nullptr};

		QSettings* _settings{nullptr};

		SingleInstance* _singleInstance{nullptr};

		// Create instance to handle settings.
		ApplicationSettings appSettings;

		QCommandLineParser _commandLineParser;

		/**
		 * TODO: Must be available from application property page to change.
		 * @brief Holds the sustain interval in milliseconds.
		 */
		int _sustainInterval;

		friend class ApplicationPropertyPage;

		void settingsReadWrite(bool save);

		bool _allowSecondary{false};
};

}
