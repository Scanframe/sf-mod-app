#pragma once

class QSettings;

namespace sf
{

class CodeEditorConfiguration
{
	public:
		explicit CodeEditorConfiguration(QSettings& settings);

		void save();

		void load();

		QString fontType{"Monospace"};
		int fontSize{10};
		bool showGutter{true};
		bool darkMode{false};
		bool useHighLighting{true};

	private:
		void settingsReadWrite(bool rw);
		QSettings& _settings;
};

}
