#include "Resource.h"
#include "gen/math.h"
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QRegularExpression>

namespace sf
{

const char* Resource::_warningLocation = ":icon/png/warning";

const char* Resource::_iconResources[] =
	{
		":icon/svg/clear",
		":icon/svg/reload",
		":icon/svg/submit",
		":icon/svg/new",
		":icon/svg/open",
		":icon/svg/cut",
		":icon/svg/copy",
		":icon/svg/paste",
		":icon/svg/undo",
		":icon/svg/redo",
		":icon/svg/save",
		":icon/svg/cancel",
		":icon/svg/close",
		":icon/svg/check",
		":icon/svg/okay",
		":icon/svg/exit",
		":icon/svg/close-window",
		":icon/svg/close-windows",
		":icon/svg/configuration",
		":icon/svg/settings",
		":icon/svg/application",
		":icon/svg/development",
		":icon/svg/compile",
		":icon/svg/initialize",
		":icon/svg/step",
		":icon/svg/run",
		":icon/svg/start",
		":icon/svg/stop",
		":icon/svg/edit",
		":icon/svg/add",
		":icon/svg/remove",
		":icon/svg/folder",
		":icon/svg/collapse",
		":icon/svg/expand",
		":icon/svg/open-folder",
		":icon/svg/hand",
		":icon/svg/search",
		":icon/svg/form",
		":icon/svg/container",
		":icon/svg/widget",
		":icon/svg/palette",
		":icon/svg/position",
		":icon/svg/location",
		":icon/svg/tools",
		":icon/svg/graph",
};

int Resource::_defaultIconSize = 128;

QString Resource::getSvgIconResource(Resource::Icon icon)
{
	if (icon < Icon(0) || (size_t) icon > (sizeof(_iconResources) / sizeof(_iconResources[0])))
	{
		qWarning() << QString("Icon enumerate '%1' is out-of-range").arg((int) icon);
	}
	return _iconResources[static_cast<int>(icon)];
}

QByteArray Resource::getByteArray(const QString& resource_location)
{
	QFile file(resource_location);
	if (file.open(QIODevice::ReadOnly))
	{
		return file.readAll();
	}
	return nullptr;
}

float Resource::getColorLuminance(const QColor& color)
{
	return calculateOffset((color.red() * 299 + color.green() * 587 + color.blue() * 114), 0, 255000, 1.0f, true);
}

QColor Resource::getColor(const QColor& color, float brightness)
{
	float h, s, v, a;
	color.getHsvF(&h, &s, &v, &a);
	return QColor::fromHsvF(h, s, clip<float>(v * brightness, 0.0f, 1.0f), a);
}

QByteArray Resource::getSvg(QFile& file, const QColor& color, const QSize& size)
{
	// Open the file for reading.
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << QString("File opening failed on '%1'").arg(file.fileName());
		return nullptr;
	}
	// Create to DOM document for reading the file.
	QDomDocument doc("svg");
	// Declare variables to receive the error information.
	QString err_msg;
	int line;
	// Read the content from the file.
	if (!doc.setContent(&file, &err_msg, &line))
	{
		qWarning() << QString("DomDoc reading failed on '%1:%2' with '%3'").arg(file.fileName()).arg(line).arg(err_msg);
		return nullptr;
	}
	auto elem = doc.firstChildElement("svg");
	// Check ig the svg element is present.
	if (elem.isNull())
	{
		qWarning() << QString("File is not an SVG '%1'").arg(file.fileName());
		return nullptr;
	}
	// Change the color when valid
	if (color.isValid())
	{
		// Using alfa transparency is not working.
		if (true)
		{
			elem.setAttribute("color", color.name(QColor::NameFormat::HexRgb).toUtf8());
		}
		else
		{
			auto color_name = QString("rgba(%1,%2,%3,%4)").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alphaF());
			elem.setAttribute("color", color_name.toUtf8());
		}
	}
	if (size.width() >= 0)
	{
		elem.setAttribute("width", QString("%1px").arg(size.width()));
	}
	else
	{
		elem.setAttribute("width", QString("%1px").arg(_defaultIconSize));
		//elem.removeAttribute("width");
	}
	if (size.height() >= 0)
	{
		elem.setAttribute("height", QString("%1px").arg(size.width()));
	}
	else
	{
		elem.setAttribute("height", QString("%1px").arg(_defaultIconSize));
		//elem.removeAttribute("height");
	}
	// Return the not indented content.
	return doc.toByteArray(-1);
}

QImage Resource::getSvgImage(const QString& resource, const QColor& color, const QSize& size)
{
	// Create file for the passed resource location.
	QFile file(resource);
	// When the icon resource does not exist return the warning PNG one.
	if (!file.exists())
	{
		qWarning() << QString("File '%1' does not exist.").arg(file.fileName());
		return QImage(_warningLocation);
	}
	// Get the svg content.
	auto ba = getSvg(file, color, size);
	// When not valid return warning
	if (!ba.length())
	{
		return QImage(_warningLocation);
	}
	// Get the SVG as an image.
	auto img = QImage::fromData(ba, "SVG");
	// Check if transparency is set on the requested color.
	// This since Qt does not support rgba(...) colors in an SVG.
	if (color.alpha() != 255)
	{
		// Create pixmap canvas of the same size.
		QPixmap pm(img.size().width(), img.size().height());
		// Initialize by filling it with transparent.
		pm.fill(Qt::transparent);
		// Create painter.
		QPainter painter(&pm);
		// Set the opacity using the color's alpha channel.
		painter.setOpacity(color.alphaF());
		// Draw the image.
		painter.drawImage(QRect(QPoint(0, 0), size), img);
		// Put the pixmap into the image.
		img = pm.toImage();
	}
	return img;
}

QIcon Resource::getSvgIcon(const QString& resource, const QColor& color, QSize size)
{
	// Non transparency simple conversion.
	return {QPixmap::fromImage(getSvgImage(resource, color, size))};
}

QIcon Resource::getSvgIcon(const QString& resource, QPalette::ColorRole role, QSize size)
{
	return getSvgIcon(resource, QApplication::palette().color(role), size);
}

QIcon Resource::getSvgIcon(const QString& resource, const QPalette& palette, QPalette::ColorRole role, QSize size)
{
	return getSvgIcon(resource, palette.color(role), size);
}

}// namespace sf