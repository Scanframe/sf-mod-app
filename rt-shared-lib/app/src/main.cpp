#include <QApplication>

#include <iostream>
#include "mainwindow.h"
#include "com/misc/dbgutils.h"

class Application : public QApplication
{
	public:
		Application(int& argc, char** argv, int flags = ApplicationFlags)
			: QApplication(argc, argv, flags) {}

	protected:
		bool notify(QObject*, QEvent*) override;
};

bool Application::notify(QObject* obj, QEvent* evt)
{
	bool done = true;
	try
	{
		done = QApplication::notify(obj, evt);
	}
	catch (std::exception& ex)
	{
		_NORM_NOTIFY(DO_MSGBOX, ex.what())
	}
	return done;
}

int main(int argc, char* argv[])
{
	Application app(argc, argv);
	MainWindow w;
	try
	{
		w.show();
		return Application::exec();
	}
	catch (...)
	{
		_NORM_NOTIFY(DO_MSGBOX, "Caught exception!")
	}
}
