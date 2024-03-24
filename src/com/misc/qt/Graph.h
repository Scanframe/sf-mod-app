#pragma once

#include "Draw.h"
#include <QPainter>
#include <QPalette>
#include "../gen/gen_utils.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Class for drawing a graph having optional rulers and grid.
 *
 * This class uses `sf::Draw` class for drawing a graph rulers and grid.
 * @image html "doc/Graph.png"
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
		 * @param start ruler start value.
		 * @param stop ruler stop value.
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
		 * @param ro ruler orientation.
		 */
		void setGrid(Draw::EGridOrientation go, Draw::ERulerOrientation ro);

		/**
		 * @brief Sets the bounding rectangle for the graph for painting in.
		 *
		 * Paints the graph passing the passed boundaries and the optional region when handling an event.
		 * @param fontMetrics For calculating the ruler widths and heights.
		 * @param bounds Boundary to paint in.
		 */
		void setBounds(const QFontMetrics& fontMetrics, const QRect& bounds);

		/**
		 * @brief Paints the graph.
		 *
		 * Paints the graph passing the passed boundaries and the optional region when handling an event.
		 * @param painter Painter instance.
		 * @param bounds Boundaries to paint in.
		 * @param region Region in case of an event.
		 * @return Rectangle of the resulting graph area. Same value as #getPlotArea() returns.
		 */
		const QRect& paint(QPainter& painter, const QRect& bounds, const QRegion& region = null_ref<QRegion>());

		/**
		 * @brief Paints a cross with text in the plot area.
		 * Used when a plot can not be painted.
		 *
		 * @param painter Painter instance.
		 * @param text Text to paint in the middle in a rectangle.
		 */
		void paintPlotCross(QPainter& painter, const QString& text);

		/**
		 * @brief Gets the remaining area to plot the graph after rulers and grid have been painted.
		 *
		 * @return Area rectangle to plot the actual graphic.
		 */
		[[nodiscard]] const QRect& getPlotArea() const;

		/**
		 * @brief Colors enumeration of colors in the graph.
		 *
		 * Used by `setColor` function.
		 */
		enum EColor
		{
			/** Color of unit and values.*/
			cRulerText,
			/** Color for lines of major and minor ticks.*/
			cRulerLine,
			/** Color of the grid lines.*/
			cGridLines,
			/** Color of ruler background.*/
			cRulerBackground,
			/** Color of the graph area background.*/
			cGraphBackground,
			/** Color of the graph.*/
			cGraphForeground,
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
			/**
			 * @brief Ruler enabled flag.
			 */
			bool enabled{false};
			/**
			 * @brief Ruler start value.
			 */
			double start{0};
			/**
			 * @brief Ruler stop value.
			 */
			double stop{0};
			/**
			 * @brief Precision digits.
			 */
			int digits{3};
			/**
			 * @brief Unit text.
			 */
			QString unit;
			/**
			 * @brief Bounds of the ruler area for painting value texts i.e.
			 */
			QRect bounds;
			/**
			 * @brief Rectangle for the ticks.
			 */
			QRect rect;
			/**
			 * @brief Calculated ruler size (width/height).
			 */
			int size{100};

		} _top, _left, _right, _bottom;

		/**
		 * @brief Holds which ruler information is responsible.
		 */
		Draw::ERulerOrientation _horizontal{Draw::roNone}, _vertical{Draw::roNone};

		/**
		 * @brief Gets the ruler information by orientation.
		 *
		 * @param ro ruler orientation.
		 * @return ruler information structure.
		 */
		RulerInfo* getRulerInfo(Draw::ERulerOrientation ro);

		/**
		 * @brief Holds the last calculated graph area rectangle.
		 */
		 QRect _plotArea;
};

}
