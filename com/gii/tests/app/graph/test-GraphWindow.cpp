#include "test-GraphWindow.h"
#include <iostream>
#include <QAbstractButton>
#include <QPainter>
#include <misc/qt/CaptureListModel.h>
#include <misc/qt/Draw.h>
#include <misc/qt/Graph.h>
#include "ui_test-GraphWindow.h"

GraphWindow::GraphWindow(QWidget* parent)
	:QDialog(parent)
	 , ui(new Ui::GraphWindow)
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/ico/scanframe"));
	connect(ui->drawWidget, &sf::DrawWidget::paint, this, &GraphWindow::onPaint);
	connect(ui->slider, &QSlider::valueChanged, this, &GraphWindow::onSlider);
	connect(ui->leValueStart, &QLineEdit::textChanged, [&](){
		ui->drawWidget->update();
	});
	connect(ui->leValueStop, &QLineEdit::textChanged, [&](){
		ui->drawWidget->update();
	});
	for (auto i: {ui->cbDebug, ui->cbLeft, ui->cbRight, ui->cbTop, ui->cbBottom})
	{
		connect(i, &QCheckBox::clicked, [&]()
		{
			ui->drawWidget->update();
		});
	}
}

void GraphWindow::onSlider(int pos)
{
	ui->drawWidget->update();
}

void GraphWindow::onPaint(QPaintEvent* event)
{
	sf::Graph graph(palette());
	graph._debug = ui->cbDebug->isChecked();
	//
	if (ui->cbLeft->isChecked())
	{
		graph.setRuler(sf::Draw::roLeft, 0, 10, 2, "V");
		//graph.setRuler(sf::Draw::roLeft, 0, 10, 2, "Y1");
	}
	if (ui->cbRight->isChecked())
	{
		graph.setRuler(sf::Draw::roRight, 0, 10, 2, "Y2");
	}
	if (ui->cbTop->isChecked())
	{
		graph.setRuler(sf::Draw::roTop, 0, 10, 2, "X1");
	}
	auto start = ui->leValueStart->text().toDouble();
	auto stop = ui->leValueStop->text().toDouble();

	if (ui->cbBottom->isChecked())
	{
		graph.setRuler(sf::Draw::roBottom, start, stop, 3, "s");
		//graph.setRuler(sf::Draw::roBottom, 0, 10, 2, "X2");
	}
	//
	graph.setGrid(sf::Draw::goHorizontal, sf::Draw::roLeft);
	graph.setGrid(sf::Draw::goVertical, sf::Draw::roBottom);
	//
	//graph.setRuler(sf::Draw::roRight, 10e-2, -20e-2, 3, "sec");
	//graph.setRuler(sf::Draw::roTop, -5e03, 10e03, 2, "m/s");
	//
	//
	QPainter painter(ui->drawWidget);

	auto rc = painter.window();

	//inflate(rc, -ui->slider->value());
	int ofs = ui->slider->value();
	rc.adjust(ofs, ofs, ofs * -2, ofs * -2);

	graph.paint(painter, rc, event->region());
}
