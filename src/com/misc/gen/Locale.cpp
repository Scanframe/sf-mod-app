#include "Locale.h"

namespace sf
{

Locale::Locale()
	// Save the current locale.
	: _locale(std::locale())
{
	// Change the numeric locale only.
	std::locale numeric_locale(_locale, std::locale::classic(), std::locale::numeric);
	// Set the locale.
	std::locale::global(numeric_locale);
}

Locale::~Locale()
{
	// When there is no change with the initial value.
	if (_locale != std::locale::classic())
		std::locale::global(std::locale::classic());
}

}// namespace sf
