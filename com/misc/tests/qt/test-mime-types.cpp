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

		qDebug() << "QMimeType count:" << mdb.allMimeTypes().count();

		auto mime = mdb.mimeTypeForName("text/xml");
		qDebug() << "QMimeType(\"text/javascript\")" << mime;

		qDebug() << "iconName():" << mime.iconName();
		qDebug() << "aliases():" << mime.aliases();
		qDebug() << "allAncestors():" << mime.allAncestors();
		qDebug() << "preferredSuffix():" << mime.preferredSuffix();
		qDebug() << "suffixes():" << mime.suffixes();
		qDebug() << "parentMimeTypes():" << mime.parentMimeTypes();
		qDebug() << "comment():" << mime.comment();

		CHECK(mdb.mimeTypeForName("text/javascript") == mdb.mimeTypeForName("application/javascript"));
		CHECK(mdb.mimeTypeForName("text/javascript").preferredSuffix() == "js");
	}


}
