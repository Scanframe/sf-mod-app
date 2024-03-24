#include <test/catch.h>

#include "misc/qt/qt_utils.h"
#include <QApplication>
#include <QPainter>
#include <QPushButton>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTimer>
#include <misc/qt/DrawWidget.h>
#include <misc/qt/FormDialog.h>

extern int debug_level;

TEST_CASE("sf::ActionButton", "[gui][qt]")
{
	SECTION("Load from Resource")
	{
		// When not GUI application has been started skip this test.
		if (!sf::isGuiApplication())
		{
			SKIP("QApplication is not running.");
		}
		// Set the file location to the resource.
		QFileInfo fi(":/ui/action-button");
		REQUIRE(fi.exists());
		//
		sf::FormDialog dlg;
		dlg.Load(QFile(fi.absoluteFilePath()));
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
