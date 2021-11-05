#pragma once

#include <QWidget>
#include <QSettings>
#include <pal/iface/PaletteServer.h>

namespace Ui {class Window;}

class Window :public QWidget
{
	Q_OBJECT

	public:
		explicit Window(QSettings* settings, QWidget* parent = nullptr);

		~Window() override;

		void stateSaveRestore(bool save);

	protected:
		void onPaint(QPaintEvent* event);

		void openPropertySheet();

	private:
		Ui::Window* ui;

		sf::PaletteServer* _paletteServer;

		QSettings* _settings;
};
