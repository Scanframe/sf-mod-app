#pragma once

#include "../global.h"
#include <locale>

namespace sf
{

/**
 * @brief Sets the numeric locale for a specific scope.
 */
class _MISC_CLASS Locale
{
	public:
		/**
		 * @brief Default constructor sets std::locale::classic() which is 'C'.
		 */
		Locale();
		/**
		 * @brief Destructor restoring stored locale.
		 */
		~Locale();

	private:
		/**
		 * @brief Holds the previous locale.
		 */
		std::locale _locale;
};

}// namespace sf
