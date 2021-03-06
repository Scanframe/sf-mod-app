#include "SfAnalogClock.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>

namespace sf
{

AnalogClock::AnalogClock(QWidget* parent)
	:QWidget(parent), ObjectExtension(this)
{
	auto* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
	timer->start(1000);
	setWindowTitle(tr("Analog Clock"));
}

bool AnalogClock::isRequiredProperty(const QString& name)
{
/*
	static const char* keys[] = {
		"geometry",
		"toolTip",
		"whatsThis",
		"text"
		"sizePolicy"
	};
	// Check if passed property name is in the keys list.
	return std::any_of(&keys[0], &keys[sizeof(keys) / sizeof(keys[0])], [name](const char* prop)
	{
		return name == prop;
	});
*/
	return true;
}

void AnalogClock::paintEvent(QPaintEvent*)
{
	static const QPoint hourHand[3] = {
		QPoint(7, 8),
		QPoint(-7, 8),
		QPoint(0, -40)
	};
	static const QPoint minuteHand[3] = {
		QPoint(7, 8),
		QPoint(-7, 8),
		QPoint(0, -70)
	};

	QColor hourColor(127, 0, 127);
	QColor minuteColor(0, 127, 127, 191);

	int side = qMin(width(), height());
	QTime time = QTime::currentTime();

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate(width() / 2.0, height() / 2.0);
	painter.scale(side / 200.0, side / 200.0);

	painter.setPen(Qt::NoPen);
	painter.setBrush(hourColor);

	painter.save();
	painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
	painter.drawConvexPolygon(hourHand, 3);
	painter.restore();

	painter.setPen(hourColor);

	for (int i = 0; i < 12; ++i)
	{
		painter.drawLine(88, 0, 96, 0);
		painter.rotate(30.0);
	}

	painter.setPen(Qt::NoPen);
	painter.setBrush(minuteColor);

	painter.save();
	painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
	painter.drawConvexPolygon(minuteHand, 3);
	painter.restore();

	painter.setPen(minuteColor);

	for (int j = 0; j < 60; ++j)
	{
		if ((j % 5) != 0)
		{
			painter.drawLine(92, 0, 96, 0);
		}
		painter.rotate(6.0);
	}
}

}
