#pragma once
#include <QPainter>
#include "../global.h"

namespace sf
{

/**
 * @brief Contains functions for drawing general stuff for graphs.
 *
 * @showrefs
 */
class _MISC_CLASS Draw
{
	public:
		/**
		 * @brief Default constructor.
		 */
		Draw();

		/**
		 * @brief Orientation for drawing a graph ruler.
		 */
		enum ERulerOrientation :int
		{
			/** Disable value. */
			roNone = 0x0,
			/** Draws ruler as situated at the left of a graph. */
			roLeft = 0x1,
			/** Draws ruler as situated at the right of a graph. */
			roRight = 0x2,
			/** Draws ruler as situated at the top of a graph. */
			roTop = 0x4,
			/** Draws ruler as situated at the bottom of a graph. */
			roBottom = 0x8,
		};

		/**
		 * @brief Draws a ruler for example around a graph.
		 *
		 * @param painter Painter context.
		 * @param ro Orientation.
		 * @param color Color for the ticks.
		 * @param font_color Color of the texts.
		 * @param bounds Boundaries for the ruler.
		 * @param area Area available to paint texts on.
		 * @param start Start value of ruler.
		 * @param stop Stop value of ruler.
		 * @param digits Amount of significant digits or resolution of the printed values.
		 * @param unit Unit string.
		 * @return True on success false in case of an exception.
		 */
		bool ruler
			(
				QPainter& painter,
				ERulerOrientation ro,
				const QColor& color,
				const QColor& font_color,
				const QRect& bounds,
				const QRect& area,
				double start,
				double stop,
				int digits,
				const QString& unit
			) const;

		/**
		 * @brief Orientation for drawing grid lines in a graph.
		 */
		enum EGridOrientation :int
		{
			/** Draws grid horizontally. */
			goHorizontal = 0x1,
			/** Draws grid vertically. */
			goVertical = 0x2
		};

		/**
		 * @brief Draws the lines in the graph as the ruler ticks.
		 *
		 * @param painter Painter context.
		 * @param go Orientation.
		 * @param color Color of the grid lines.
		 * @param bounds Area boundaries for the ruler.
		 * @param start Start value of grid lines.
		 * @param stop Stop value of grid lines.
		 * @param digits To make the grid correspond with the ruler when vertical.
		 * @return True on success false in case of an exception.
		 */
		bool gridLines
			(
				QPainter& painter,
				EGridOrientation go,
				const QColor& color,
				const QRect& bounds,
				double start,
				double stop,
				unsigned digits
			) const;

		/**
		 * @brief Draws a cross with text rectangle in the middle.
		 *
		 * @param painter Painter context.
		 * @param bounds Area boundaries for the ruler.
		 * @param text Text put in a rectangle in the middle.
		 * @param color Color of the cross and text.
		 * @return True on success false in case of an exception.
		 */
		bool textCross(QPainter& painter, const QRect& bounds, const QString& text, const QColor& color);

	private:
		/**
		 * @brief Separation factor for the texts.
		 */
		const double _sepFactor;

		/**
		 * @brief Lookup list for ticks.
		 */
		static struct Tick
		{
			int Value;
			int Ticks;
		} _tickListVertical[], _tickListHorizontal[];
};

}