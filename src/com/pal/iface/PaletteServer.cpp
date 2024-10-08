#include "PaletteServer.h"
#include "PaletteServerPropertyPage.h"
#include <misc/gen/pointer.h>
#include <misc/qt/qt_utils.h>

namespace sf
{

PaletteServer::PaletteServer(QObject* parent)
	: QObject(parent)
	, _implementation(nullptr)
	, _colorsUsed(256)
	, _colorsSize(256)
	, _flagGenerate(true)
{
}

PaletteServer::~PaletteServer()
{
	delete _implementation;
}

void PaletteServer::addPropertyPages(PropertySheetDialog* sheet)
{
	sheet->addPage(new PaletteServerPropertyPage(this, sheet));
	if (_implementation)
	{
		_implementation->addPropertyPages(sheet);
	}
}

ColorTable PaletteServer::getColorTable() const
{
	if (_flagGenerate)
	{
		// Since this is a const function casting is needed.
		auto self = const_cast<PaletteServer*>(this);
		self->_flagGenerate = false;
		// Update the copy of the local color table.
		self->_colorTable = _implementation->getColorTable();
	}
	return _colorTable;
}

void PaletteServer::paint(QPainter& painter, const QRect& bounds) const
{
	// Save the painter state to restore it later.
	painter.save();
	// Create list to hold the 8-bit image data.
	QList<uchar> data;
	// Only for the amount of colors that are actually used.
	data.resize(_colorsUsed);
	// Fill the data list with incremental values from 0 to colors used.
	std::iota(std::begin(data), std::end(data), 0);
	// Create grayscale image from the data.
	QImage image((const uchar*) data.data(), _colorsUsed, 1, _colorsUsed, QImage::Format_Grayscale8);
	if (_implementation)
	{
		// Convert the image to an 8-bit indexed format and set the color table for each index.
		auto img = image.convertToFormat(QImage::Format_Indexed8);
		img.setColorTable(getColorTable());
		painter.drawImage(bounds, img);
	}
	else
	{
		// Draw the converted image.
		painter.drawImage(bounds, image);
	}
	if (_implementation)
	{
		// Coordinates for storing the curve.
		QPolygon polygon;
		// Set the fixed size of the polygon for drawing the curve.
		constexpr int count = 20;
		// Calculate the polygon points.
		for (int i = 0; i <= count; i++)
		{
			auto x = (1.0 * i) / count;
			polygon.append(bounds.bottomLeft() + QPoint(static_cast<int>(bounds.width() * x), static_cast<int>((1 - bounds.height()) * _implementation->getCurveLevel(x))));
		}
		// Select black for the curve.
		painter.setPen(Qt::white);
		// Draw the curve into the palette image.
		painter.drawPolyline(polygon);
	}
	// Bring painter state back to as it was found.
	painter.restore();
}

void PaletteServer::setColorCount(int colorsUsed, int colorsSize)
{
	if (colorsSize < 0)
	{
		colorsSize = 256;
	}
	// Compare the values if a change happened.
	if (_colorsUsed != colorsUsed || _colorsSize != colorsSize)
	{
		// Update the values.
		_colorsUsed = colorsUsed;
		_colorsSize = colorsSize;
		if (_implementation)
		{
			_implementation->setSizes(_colorsUsed, _colorsSize);
			// Set flag so method getColorTable() retrieves a new copy if not set already by setSizes().
			_flagGenerate = true;
		}
	}
}

std::string PaletteServer::currentImplementationName() const
{
	// Get the current name of the server.
	if (_implementation)
	{
		return PaletteInterface::Interface().getRegisterName(_implementation);
	}
	return {};
}

bool PaletteServer::createImplementation(const std::string& name)
{
	// If another implementation exists delete it first.
	if (_implementation)
	{
		delete_null(_implementation);
	}
	if (!name.empty())
	{
		// Create a new implementation of a registered class.
		_implementation = PaletteInterface::Interface().create(name, PaletteInterface::Parameters(PaletteInterface::Parameters::Callback([&]() {
																														 // Make a call to getColorTable() generate a new local copy.
																														 _flagGenerate = true;
																														 // Signal a change.
																														 Q_EMIT changed(this);
																													 })));
		if (_implementation)
		{
			// Make a call to getColorTable() generate a new local copy.
			_flagGenerate = true;
			// Signal a change.
			Q_EMIT changed(this);
			return true;
		}
	}
	return false;
}

QList<QPair<QString, QString>> PaletteServer::getImplementationNames() const
{
	QList<QPair<QString, QString>> rv;
	for (size_t index = 0; index < PaletteInterface::Interface().size(); index++)
	{
		rv << QPair<QString, QString>(PaletteInterface::Interface().getName(index), PaletteInterface::Interface().getDescription(index));
	}
	return rv;
}

const QString& PaletteServer::getImplementationName() const
{
	return _implementationName;
}

bool PaletteServer::isAvailable() const
{
	return _implementation != nullptr;
}

void PaletteServer::setImplementationName(const QString& name)
{
	if (_implementationName != name)
	{
		_implementationName = name;
		delete_null(_implementation);
		// Check if creation is successful.
		if (createImplementation(_implementationName.toStdString()))
		{
			// Apply existing properties.
			_implementation->setProperties(_implementationProperties);
		}
	}
}

void PaletteServer::setImplementationProperties(const QStringList& props)
{
	// Only update when there are changes.
	if (_implementationProperties != props)
	{
		// Assign the changed properties.
		_implementationProperties = props;
		// Check if the implementation needs to change.
		if (_implementationProperties.first() != _implementationName)
		{
			_implementationName = _implementationProperties.first();
			delete_null(_implementation);
			createImplementation(_implementationName.toStdString());
		}
		// When an implementation exists update it's properties.
		if (_implementation)
		{
			_implementation->setProperties(_implementationProperties);
		}
	}
}

const QStringList& PaletteServer::getImplementationProperties()
{
	// When an implementation exists get the properties from it.
	if (_implementation)
	{
		_implementationProperties.clear();
		// First is the implementation name.
		_implementationProperties.append(_implementationName);
		// Append the all implementation properties.
		_implementationProperties.append(_implementation->getProperties());
	}
	return _implementationProperties;
}

}// namespace sf
