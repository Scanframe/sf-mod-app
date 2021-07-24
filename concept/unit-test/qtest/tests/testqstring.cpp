#include <QtTest/QtTest>

class TestQString: public QObject
{
	Q_OBJECT
	private Q_SLOTS:
		void toUpper();
		void toLower();
};

void TestQString::toLower()
{
	QString str = "Hello";
	QCOMPARE(str.toLower(), QString("hello"));
}

void TestQString::toUpper()
{
	QString str = "Hello";
	QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestQString)

#include "testqstring.moc"
