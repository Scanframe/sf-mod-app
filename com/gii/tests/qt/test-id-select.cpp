#include <catch2/catch.hpp>

#include <QDialog>
#include <QPushButton>
#include <QTimer>
#include <gen/Variable.h>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>

extern int debug_level;

namespace sf
{

QDialog* createDialog()
{
	auto dlg = new QDialog();
	dlg->setObjectName("MyDialog");
	dlg->setAttribute(Qt::WA_DeleteOnClose);
	auto layout = new QVBoxLayout(dlg);
	layout->setObjectName("MyLayout");
	dlg->setLayout(layout);
	auto btnClose = new QPushButton("&Close", dlg);
	qDebug() << "Button Parent" << btnClose->parent()->objectName();
	qDebug() << "Button Parent" << btnClose->parentWidget()->layout()->metaObject()->className();
	QObject::connect(btnClose, &QPushButton::clicked, [dlg]()
	{
		dlg->close();
	});
	layout->addWidget(btnClose);
	return dlg;
}

}

TEST_CASE("Id Dialog", "[debug]")
{

	sf::Variable::initialize();

	SECTION("Variable")
	{
		auto dlg = sf::createDialog();

		QTimer::singleShot(2000, [&]
		{
			QString fp = QDir::temp().filePath(QFileInfo(__FILE__).baseName() + ".png");
			if (debug_level)
			{
				qDebug() << "Saving grabbed content to: " << "file://" + fp;
			}
			CHECK(dlg->grab().save(fp));
			if (!debug_level)
			{
				dlg->close();
			}
		});
		dlg->exec();
	}


}
