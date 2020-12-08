#ifndef MISC_QRESOURCE_H
#define MISC_QRESOURCE_H

#include <QIcon>

#include "global.h"

namespace sf
{
/**
 * Class to make resource accessible using enums which make autocomplete possible.
 */
class _MISC_CLASS QResource
{
	public:
		/**
		 * List of available icons which is easy wen using auto complete.
		 */
		enum class Icon: int
		{
			Clear, Reload, Submit
		};

		/**
		 * Gets an icon name from the pass enum.
		 */
		static const char* getIconName(Icon icon);
		/**
		 * Gets an QIcon type from resource.
		 */
		static QIcon getIcon(Icon icon);

	private:
		/*
		 * Holds all the icon names in the resource file.
		 */
		static const char* _iconNames[];

};

} // namespace sf

#endif // MISC_QRESOURCE_H
