#pragma once

#include <QIcon>
#include <QFile>
#include <QPalette>

#include "../global.h"

namespace sf
{
/**
 * @brief Class for accessing resources like SVG images as a QIcon.
 */
class _MISC_CLASS Resource
{
	public:
		/**
		 * @brief List of available icons which is easy wen using auto complete.
		 */
		enum Icon: int
		{
				Clear,
				Reload,
				Submit,
				New,
				Open,
				Cut,
				Copy,
				Paste,
				Undo,
				Redo,
				Save,
				Cancel,
				Close,
				Check,
				Okay,
				Exit,
				CloseWindow,
				CloseWindows,
				Configuration,
				Settings,
				Application,
				Development,
				Compile,
				Initialize,
				Step,
				Run,
				Start,
				Stop,
				Edit,
				Add,
				Remove
		};

		/**
		 * @brief Gets an svg icon resource location from the pass enumerate value.
		 *
		 * @param icon Enumerate color value.
		 * @return Resource location
		 */
		static QString getSvgIconResource(Icon icon);

		/**
		 * @brief Retrieves resource in a byte array.
		 *
		 * @param resource location in the resource tree or file.
		 * @return Resource file content.
		 */
		static QByteArray getByteArray(const QString& resource);

		/**
		 * @brief Gets a SVG using the passed color as the main color set in the 'svg' tags attribute.
		 *
		 * @param file File instance.
		 * @param color HTML formed color string. Default is non valid color.
		 * @param size Size of the square icon where when -1 the svg size is not set and thus not restricted (default).
		 * @return The SVG data on success and zero length on failure.
		 */
		static QByteArray getSvg(QFile& file, const QColor& color = QColor(), const QSize& size = QSize());

		/**
		 * @brief Gets a SVG using the passed color as the main color set in the 'svg' tags attribute.
		 *
		 * When the color's alpha channel has been set to total image is made transparent since Qt does not support
		 * rgba(...) colors when rendering.
		 *
		 * @param resource Location of the image.
		 * @param color HTML formed color string. Default is non valid color.
		 * @param size Size of the square icon where when -1 the svg size is not set and thus not restricted (default).
		 * @return SVG image on success or the default warning PNG image.
		 */
		static QImage getSvgImage(const QString& resource, const QColor& color = QColor(), const QSize& size = QSize());

		/**
		 * @brief Gets an SVG QIcon type from a resource location using the passed color and optional size.
		 *
		 * @param resource Location of the icon.
		 * @param color Color for the icon.
		 * @param size Size of the icon where when not passed default set to _defaultIconSize (128x128).
		 * @return On success the colored icon otherwise the warning icon.
		 */
		static QIcon getSvgIcon(const QString& resource, const QColor& color = QColor(), QSize size = QSize());

		/**
		 * @brief Gets an SVG QIcon type from a resource location using the palette and role for color.
		 *
		 * @param resource Location of the Icon.
		 * @param palette The pallet to determine the color for the Icon.
		 * @param role The color role to determine the color for the Icon.
		 * @param size Size of the icon where when not passed default set to _defaultIconSize (128x128).
		 * @return On success the colored icon.
		 */
		static QIcon
		getSvgIcon(const QString& resource, const QPalette& palette, QPalette::ColorRole role, QSize size = QSize());

		/**
		 * @brief Gets an SVG QIcon type from a resource location using the application palette and the passed role for color.
		 *
		 * @param resource Location of the Icon.
		 * @param role The color role to determine the color for the Icon.
		 * @param size Size of the icon where when not passed default set to _defaultIconSize (128x128).
		 * @return On success the colored icon.
		 */
		static QIcon getSvgIcon(const QString& resource, QPalette::ColorRole role, QSize size = QSize());

	private:

		/**
		 * @brief Gets the passed color in a different brightness.
		 *
		 * @param color Base color.
		 * @param brightness Multiplier for HSV V value.
		 * @return Changed color.
		 */
		static QColor getColor(const QColor& color, float brightness = 1.0);

		/**
		 * Gets the luminance of the passed color. Could be used for determining dark or light mode.
		 *
		 * The threshold for dark or light mode could be 0.5.
		 * @param color Color value.
		 * @return Floating point value between 0 and 1.
		 */
		static float getColorLuminance(const QColor& color);

		/*
		 * @brief Holds all the icon names in the res file.
		 */
		static const char* _iconResources[];

		/**
		 * @brief Location of the warning image.
		 */
		static const char* _warningLocation;
		/**
		 * @brief Default icon size when no size (-1) has been passed.
		 */
		static int _defaultIconSize;
};

} // namespace
