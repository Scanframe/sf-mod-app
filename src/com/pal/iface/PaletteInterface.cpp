#include "PaletteInterface.h"

namespace sf
{

// Implementations of static functions and data members to be able to create registered stars implementations.
SF_IMPL_IFACE(PaletteInterface, PaletteInterface::Parameters, Interface)

PaletteInterface::PaletteInterface(const PaletteInterface::Parameters& params)
	:QObject(params._parent)
	 , Persistent(this)
	 , _callback(params._callback)
{
}

void PaletteInterface::addPropertyPages(PropertySheetDialog* sheet)
{
	// Deliberate not implemented.
}

void PaletteInterface::setSizes(qsizetype colorsUsed, qsizetype colorsSize)
{
	if (_colorsUsed != colorsUsed || _colorsSize != colorsSize)
	{
		_colorsUsed = colorsUsed;
		_colorsSize = colorsSize;
		if (_callback)
		{
			_callback();
		}
	}
}

}
