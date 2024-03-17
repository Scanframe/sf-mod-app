#include <ostream>
#include <QApplication>
#include <QDialog>
#include <QTime>
#include <QShortcut>
#include <QMouseEvent>
#include <QTextStream>
#include <QVBoxLayout>
#include <QCheckBox>
#include <gsc/GlobalShortcut.h>

QTextStream out(stdout);
QTextStream err(stderr);

class MyMainWindow :public QDialog
{
	public:
		explicit MyMainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
		:QDialog(parent, flags)
		,_shortcut(new QShortcut(this))
		,_globalShortcut1(new sf::GlobalShortcut(this))
		,_globalShortcut2(new sf::GlobalShortcut(this))
		{
			auto vbl = new QVBoxLayout(this);
			setLayout(vbl);
			setFixedSize(600, 300);

			_shortcut->setKey(QKeySequence("Meta+F11"));
			_shortcut->setEnabled(true);
			_shortcut->setContext(Qt::ApplicationShortcut);
			QObject::connect(_shortcut, &QShortcut::activated, [&]()
			{
				out << QString("Shortcut %1 pressed!").arg(_shortcut->key().toString()) << Qt::endl;
			});

			//QKeySequence shortcut("Meta+F12");
			_globalShortcut1->setSequence(QKeySequence("F1"));
			_globalShortcut1->setEnabled(true);
			_globalShortcut1->setGlobal(false);
			if (!_globalShortcut1->isValid())
			{
				err << QString("Error: Failed to set shortcut %1").arg(_globalShortcut1->sequence().toString()) << Qt::endl;
			}
			out << QString("Press shortcut %1 (or CTRL+C to exit)").arg(_globalShortcut1->sequence().toString()) << Qt::endl;

			_globalShortcut2->setSequence(QKeySequence("F3"));
			_globalShortcut2->setEnabled(true);
			_globalShortcut2->setGlobal(true);
			_globalShortcut2->setAutoRepeat(true);
			if (!_globalShortcut2->isValid())
			{
				err << QString("Error: Failed to set shortcut %1").arg(_globalShortcut2->sequence().toString()) << Qt::endl;
			}
			out << QString("Press shortcut %1 (or CTRL+C to exit)").arg(_globalShortcut2->sequence().toString()) << Qt::endl;

			auto func = [](sf::GlobalShortcut* gsc)
			{
				out << QString("Shortcut %1 pressed!").arg(gsc->sequence().toString()) << " " << QTime::currentTime().toString(Qt::ISODateWithMs) << Qt::endl;
				//win.activateWindow();
				//QApplication::quit();
			};

			QObject::connect(_globalShortcut1, &sf::GlobalShortcut::activated, func);
			QObject::connect(_globalShortcut2, &sf::GlobalShortcut::activated, func);

			auto cb = new QCheckBox(this);
			vbl->addWidget(cb);
			//layout()->addWidget(cb);
			cb->setText(QString("Global shortcut (%1)").arg(_globalShortcut1->sequence().toString()));
			connect(cb, &QCheckBox::stateChanged, [&](int state)
			{
				_globalShortcut1->setGlobal(!!state);
				if (!_globalShortcut1->isValid())
				{
					err << QString("Error: Failed to set shortcut %1").arg(_globalShortcut1->sequence().toString()) << Qt::endl;
				}
			});
			cb = new QCheckBox(this);
			vbl->addWidget(cb);
			//layout()->addWidget(cb);
			cb->setText(QString("Auto Repeat"));
			connect(cb, &QCheckBox::stateChanged, [&](int state)
			{
				_globalShortcut1->setAutoRepeat(!!state);
			});
		}

		void mousePressEvent(QMouseEvent* event) override
		{
			QWidget::mousePressEvent(event);
			if (event->modifiers() == Qt::Modifier::CTRL)
			{
				auto dlg = new QDialog(this, Qt::WindowType::Dialog);
				dlg->setFixedSize(400, 250);
				dlg->setWindowTitle("My Dialog");
				dlg->setModal(true);
				dlg->show();
			}
		}

	private:
		QShortcut* _shortcut;
		sf::GlobalShortcut* _globalShortcut1;
		sf::GlobalShortcut* _globalShortcut2;
};

int main(int argc, char** argv)
{
	QApplication app(argc, argv);


	MyMainWindow win;
	win.show();

	return QApplication::exec();
}

