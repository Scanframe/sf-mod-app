#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
	Q_INIT_RESOURCE(resource);

	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("MDI Example");
	QCoreApplication::setOrganizationName("QtProject");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	QCommandLineParser parser;
	parser.setApplicationDescription("Qt MDI Example");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);

	MainWindow mainWin;
	const QStringList posArgs = parser.positionalArguments();
	for (const QString& fileName : posArgs)
	{
		mainWin.openFile(fileName);
	}
	mainWin.show();
	return QApplication::exec();
}
