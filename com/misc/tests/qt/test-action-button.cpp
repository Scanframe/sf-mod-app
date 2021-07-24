#include <catch2/catch.hpp>

#include <QApplication>
#include <QTimer>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QPainter>
#include <QIcon>
#include <QPushButton>
#include <misc/qt/FormDialog.h>
#include <misc/qt/DrawWidget.h>

extern int debug_level;

TEST_CASE("sf::ActionButton", "[qt][gui]")
{
	SECTION("Load from Resource")
	{
		// Set the file location to the resource.
		QFileInfo fi(":/ui/action-button");
		REQUIRE(fi.exists());
		//
		sf::FormDialog dlg;
		dlg.Load(QFile(fi.absoluteFilePath()));
		//
		QTimer::singleShot(2000, [&]
		{
			QString fp = QDir::temp().filePath(QFileInfo(__FILE__).baseName() + ".png");
			if (debug_level)
			{
				qDebug() << "Saving grabbed content to: " << "file://" + fp;
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
