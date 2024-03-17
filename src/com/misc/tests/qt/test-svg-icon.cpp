#include <test/catch.h>

#include "misc/qt/qt_utils.h"
#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTimer>
#include <misc/qt/DrawWidget.h>
#include <misc/qt/FormDialog.h>
#include <misc/qt/Resource.h>

extern int debug_level;

TEST_CASE("sf::SvgIcon", "[gui][qt]")
{
	SECTION("Create QIcon from SVG resource")
	{
		// When not GUI application has been started skip this test.
		if (!sf::isGuiApplication())
		{
			SKIP("QApplication is not running.");
		}
		// Set the file location to the resource.
		QFileInfo fi(":ui/svg-icon");
		REQUIRE(fi.exists());
		//
		sf::FormDialog dlg;
		dlg.Load(QFile(fi.absoluteFilePath()));
		// Find the draw widgets.
		auto widgets = dlg.findChildren<sf::DrawWidget*>();
		// Sort them using their object names.
		std::sort(widgets.begin(), widgets.end(), [](const sf::DrawWidget* a, const sf::DrawWidget* b) -> bool {
			return a->objectName() < b->objectName();
		});
		//
		QIcon::Mode modes[] = {QIcon::Mode::Normal, QIcon::Mode::Active, QIcon::Mode::Disabled, QIcon::Mode::Selected};
		// <rect width="512" height="512" fill="red" />
		auto icon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Reload), QPalette::ColorRole::ButtonText, QSize(100, 100));
		//auto icon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Reload), QColorConstants::Red, -1);
		//auto icon = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Reload), QColor(255, 0, 255, 100), QSize(64, 64));
		//
		for (auto widget: widgets)
		{
			auto idx = widgets.indexOf(widget);
			QObject::connect(widget, &sf::DrawWidget::paint, [modes, icon, idx, widget](QPaintEvent* event) -> void {
				QPainter painter(widget);
				painter.fillRect(painter.window(), QApplication::palette().color(QPalette::ColorRole::Button));
				icon.paint(&painter, painter.window(), Qt::AlignmentFlag::AlignCenter, modes[idx]);
			});
		}
		// Set the Icon for each button in the dialog.
		for (auto btn: dlg.findChildren<QPushButton*>())
		{
			auto ico = sf::Resource::getSvgIcon(sf::Resource::getSvgIconResource(sf::Resource::Icon::Reload), QColor(255, 255, 0), btn->iconSize());
			btn->setIcon(ico);
		}
		//
		QTimer::singleShot(2000, [&] {
			QString fp = QDir::temp().filePath(QFileInfo(__FILE__).baseName() + ".png");
			if (debug_level)
			{
				qDebug() << "Saving grabbed content to: "
								 << "file://" + fp;
			}
			CHECK(dlg.grab().save(fp));
			if (!debug_level)
			{
				dlg.close();
			}
		});
		dlg.exec();
	}
}
