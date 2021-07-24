#include <ostream>
#include <QApplication>

#include <globalshortcut/QxtGlobalShortcut.h>

#include <QTextStream>
#include <QMainWindow>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QTextStream out(stdout);
	QTextStream err(stderr);

	//QKeySequence shortcut("Ctrl+K");
	QKeySequence shortcut("Meta+F12");
	QxtGlobalShortcut globalShortcut(shortcut);
	globalShortcut.setEnabled(true);

/*
	QMainWindow win;
	win.show();
*/

	if (!globalShortcut.isValid())
	{
		err << QString("Error: Failed to set shortcut %1").arg(shortcut.toString()) << Qt::endl;
		return 1;
	}

	out << QString("Press shortcut %1 (or CTRL+C to exit)").arg(shortcut.toString()) << Qt::endl;

	QObject::connect(
		&globalShortcut, &QxtGlobalShortcut::activated, &globalShortcut, [&]
		{
			out << QLatin1String("Shortcut pressed!") << Qt::endl;
			QApplication::quit();
		});

	return QApplication::exec();
}
