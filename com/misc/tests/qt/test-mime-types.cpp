#include <catch2/catch.hpp>
#include <iostream>
#include "misc/qt/qt_utils.h"
#include <QMimeType>
#include <QMimeDatabase>

TEST_CASE("QMimeType", "[qt][mime]")
{
	using Catch::Equals;

	SECTION("FromPath")
	{
		QMimeDatabase mdb;

//		qDebug() << mdb.mimeTypeForFile("MyJavascript.js").aliases();
//		qDebug() << mdb.mimeTypeForName("text/javascript");

		CHECK(mdb.mimeTypeForName("text/javascript") == mdb.mimeTypeForName("application/javascript"));
	}


}
