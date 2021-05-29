#include "test-GraphWindow.h"
#include "ui_test-GraphWindow.h"

#include <iostream>
#include <QDebug>
#include <QAbstractButton>
#include <QPainter>
#include <misc/qt/CaptureListModel.h>
#include <misc/qt/Resource.h>
#include <misc/qt/Draw.h>
#include <misc/qt/qt_utils.h>
#include <misc/qt/Graph.h>

GraphWindow::GraphWindow(QWidget* parent)
	:QMainWindow(parent)
	 , ui(new Ui::GraphWindow)
{
	ui->setupUi(this);
	// Set an icon on the window.
	setWindowIcon(QIcon(":logo/scanframe"));
	centralWidget()->setAttribute(Qt::WA_AlwaysShowToolTips, true);
	// Assign the model to the listView.
	auto clm = new sf::CaptureListModel(ui->listViewLog);
	// Set to capture 'clog'.
	clm->setSource(clm->ssClog);
	// Assign the modal to the listview.
	ui->listViewLog->setModel(clm);
	// Method of connecting Qt5 style
	connect(ui->actionAction_1, &QAction::triggered, this, &GraphWindow::onAction1);
	connect(ui->actionAction_2, &QAction::triggered, this, &GraphWindow::onAction2);
	connect(ui->drawWidget, &sf::DrawWidget::paint, this, &GraphWindow::onPaint);
	//connect(ui->drawWidget, &sf::DrawWidget::paint, this, &GraphWindow::onPaintTest);
}

GraphWindow::~GraphWindow()
{
	auto clm = reinterpret_cast<sf::CaptureListModel*>(ui->listViewLog->model());
	clm->setSource(0);
	std::clog << __FUNCTION__ << std::endl;
}

void GraphWindow::onAction1()
{
	std::clog << __FUNCTION__ << std::endl;
}

void GraphWindow::onAction2()
{
	std::clog << __FUNCTION__ << std::endl;
}

void GraphWindow::onPaint(QPaintEvent* event)
{
	sf::Graph graph;
	//
	//graph.setColor(graph.cRulerBackground, QColor(0, 127, 127, 64));
	graph.setColor(graph.cRulerBackground, QColor());
	//
	graph.setRuler(sf::Draw::roLeft, -10, 10, 2, "V");
	graph.setRuler(sf::Draw::roRight, 10, -10, 2, "m/s");
	//graph.setRuler(sf::Draw::roRight, 10e-2, -20e-2, 3, "sec");
	//graph.setRuler(sf::Draw::roTop, -5e03, 10e03, 2, "m/s");
	graph.setRuler(sf::Draw::roTop, -10, 0, 2, "m/s");
	graph.setRuler(sf::Draw::roBottom, 0, 10, 2, "s");
	//
	graph.setGrid(sf::Draw::goHorizontal, sf::Draw::roLeft);
	graph.setGrid(sf::Draw::goVertical, sf::Draw::roBottom);
	//
	QPainter painter(ui->drawWidget);
	graph.paint(painter, painter.window(), event->region());
}
