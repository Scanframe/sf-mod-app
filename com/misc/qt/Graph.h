#pragma once

#include <QPainter>
#include <QPalette>
#include "Draw.h"
#include "../global.h"
#include "../gen/gen_utils.h"

namespace sf
{

/**
 * @brief Class for drawing a graph having optional rulers and grid.
 *
 * This class uses `sf::Draw` class for drawing a graph rulers and grid.
 * @image html "com/misc/doc/Graph.png"
 *
 * Use this class as a private class for a custom widget which produces a graph.
 */
class _MISC_CLASS Graph
{
	public:
		/**
		 * Constructor passing a palette.
		 */
		explicit Graph(const QPalette& palette = {});

		/**
		 * @brief Sets and thus enabling the ruler for the passed orientation.
		 * @param ro Orientation of the ruler.
		 * @param start Ruler start value.
		 * @param stop Ruler stop value.
		 * @param digits Precision of the ruler values.
		 * @param unit Unit text of the ruler.
		 */
		void setRuler(Draw::ERulerOrientation ro, double start, double stop, int digits, const QString& unit);

		/**
		 * @brief Enables grid painting in the graph.
		 *
		 * When painting this enables drawing grid for the passed orientation using
		 * the information of the passed ruler orientation.
		 * @param go Grid orientation
		 * @param ro Ruler orientation.
		 */
		void setGrid(Draw::EGridOrientation go, Draw::ERulerOrientation ro);

		/**
		 * @brief Paints the graph.
		 *
		 * Paints the graph passing the passed boundaries and the optional region when handling an event.
		 * @param painter Painter instance.
		 * @param bounds Boundaries to paint in.
		 * @param region Region in case of an event.
		 * @return Rectangle of the resulting graph area. Same value as #getGraphArea() returns.
		 */
		const QRect& paint(QPainter& painter, const QRect& bounds, const QRegion& region = null_ref<QRegion>());

		/**
		 * @brief Gets the area for the graph after rulers and grid are painted.
		 *
		 * @return Area rectangle of the graph.
		 */
		[[nodiscard]] const QRect& getGraphArea() const;

		/**
		 * @brief Colors enumeration of colors in the graph.
		 *
		 * Used by `setColor` function.
		 */
		enum EColor
		{
			/** Color of unit and values.*/
			cText,
			/** Color for lines of major and minor ticks.*/
			cLine,
			/** Color of the grid lines.*/
			cGrid,
			/** Color of ruler background.*/
			cRulerBackground,
			/** Color of the graph area.*/
			cGraphBackground,
		};

		/**
		 * @brief Allows setting of individual indexed colors for the graph.
		 *
		 * @param index Index of the color which is set.
		 * @param color The specified color.
		 */
		void setColor(EColor index, QColor color);

		/**
		 * @brief Draws only area bounds contours when enabled.
		 */
		bool _debug{false};

	protected:
		/**
		 * @brief Holds the colors for painting the rulers en grid.
		 */
		QVector<QColor> _colors;

		/**
		 * @brief Holds information on the rulers.
		 *
		 * Holds set and calculated information.
		 */
		struct RulerInfo
		{
			// Ruler enabled flag.
			bool enabled{false};
			// Ruler start value.
			double start{0};
			// Ruler stop value.
			double stop{0};
			// Precision digits.
			int digits{3};
			// Unit text.
			QString unit;
			// Bounds for value texts.
			QRect bounds;
			// Rectangle for the ticks.
			QRect rect;
			// Calculated ruler size (width/height).
			int size{100};
		} _top, _left, _right, _bottom;

		/**
		 * @brief Holds which ruler information is responsible.
		 */
		Draw::ERulerOrientation _horizontal{0}, _vertical{0};

		/**
		 * @brief Gets the ruler information by orientation.
		 *
		 * @param ro Ruler orientation.
		 * @return Ruler information structure.
		 */
		RulerInfo* getRulerInfo(Draw::ERulerOrientation ro);

		/**
		 * @brief Holds the last calculated graph area rectangle.
		 */
		 QRect _graphArea;
};

}
