#include <QApplication>
#include <QFileDialog>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setStyle("fusion");
	QFileDialog dialog;
	dialog.setOption(QFileDialog::DontUseNativeDialog, false);
	dialog.exec();
	//return app.exec();
}
