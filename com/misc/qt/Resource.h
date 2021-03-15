#pragma once

#include <QIcon>

#include "../global.h"

namespace sf
{
/**
 * Class to make resource accessible using enums which make autocomplete possible.
 */
class _MISC_CLASS Resource
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
		/**
		 * Retrieves resource in a string.
		 * @param resource_location location in the resource tree.
		 * @return Resource file content.
		 */
		static QString getString(const QString& resource_location);

	private:
		/*
		 * Holds all the icon names in the res file.
		 */
		static const char* _iconNames[];

};

} // namespace

