#include "test-Window.h"
#include "ui_test-Window.h"
#include <QPainter>
#include <misc/qt/qt_utils.h>

using namespace sf;

Window::Window(QSettings* settings, QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::Window)
	, _paletteServer(new PaletteServer(this))
	, _settings(settings)
{
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	//
	ui->setupUi(this);
	// Connect close button.
	connect(ui->btnClose, &QPushButton::clicked, this, &QWidget::close);
	//
	connect(ui->btnProperties, &QPushButton::clicked, this, &Window::openPropertySheet);
	//
	connect(ui->sbUsedColors, &QSpinBox::valueChanged, [&]() { ui->drawWidget->update(); });
	//
	connect(ui->drawWidget, &DrawWidget::paint, this, &Window::onPaint);
	connect(_paletteServer, &PaletteServer::changed, [&]() { ui->drawWidget->update(); });
	//
	ui->lwProperties->setAlternatingRowColors(true);
	//
	stateSaveRestore(false);
}

Window::~Window()
{
	stateSaveRestore(true);
}

void Window::stateSaveRestore(bool save)
{
	_settings->beginGroup(getObjectNamePath(this).join('.').prepend("State."));
	constexpr QAnyStringView keyColorsUsed("ColorsUsed");
	constexpr QAnyStringView keyWidgetRect("WidgetRect");
	constexpr QAnyStringView keyPaletteServer("PaletteServer");
	if (save)
	{
		_settings->setValue(keyColorsUsed, ui->sbUsedColors->value());
		_settings->setValue(keyWidgetRect, geometry());
		_settings->setValue(keyPaletteServer, _paletteServer->getImplementationProperties());
	}
	else
	{
		ui->sbUsedColors->setValue(_settings->value(keyColorsUsed).toInt());
		// Get the keys in the section to check existence in the ini-section.
		if (_settings->value(keyWidgetRect).toRect().isValid())
		{
			setGeometry(_settings->value(keyWidgetRect).toRect());
			_paletteServer->setImplementationProperties(_settings->value(keyPaletteServer).toStringList());
		}
	}
	_settings->endGroup();
	ui->lwProperties->clear();
	ui->lwProperties->addItems(_paletteServer->getImplementationProperties());
}

void Window::onPaint(QPaintEvent* event) const
{
	QPainter p(ui->drawWidget);
	_paletteServer->setColorCount(ui->sbUsedColors->value());
	// Select black form the border.
	p.setPen(Qt::black);
	// Draw a rectangle inside the bounds.
	p.drawRect(p.window().adjusted(0, 0, -1, -1));
	//
	_paletteServer->paint(p, p.window().adjusted(1, 1, -1, -1));
}

void Window::openPropertySheet()
{
	const auto dlg = new PropertySheetDialog("ColorPalette", _settings, this);
	dlg->setWindowTitle("Color Palette");
	_paletteServer->addPropertyPages(dlg);
	dlg->exec();
	ui->lwProperties->clear();
	ui->lwProperties->addItems(_paletteServer->getImplementationProperties());
	delete dlg;
}
