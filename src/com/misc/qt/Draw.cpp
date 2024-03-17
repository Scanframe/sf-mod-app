#include "Draw.h"
#include "../gen/gen_utils.h"
#include "../qt/qt_utils.h"

namespace sf
{

Draw::Draw()
	:_sepFactor(2.2)
{
}

Draw::Tick Draw::_tickListVertical[] = {
	{10, 5},
	{20, 4},
	{25, 5},
	{40, 4},
	{50, 5},
	{100, 5},
};

Draw::Tick Draw::_tickListHorizontal[] = {
	{10, 10},
	{20, 8},
	{25, 10},
	{40, 8},
	{50, 10},
	{100, 10},
};

bool Draw::ruler
	(
		QPainter& p,
		ERulerOrientation ro,
		const QColor& color,
		const QColor& font_color,
		const QRect& bounds,
		const QRect& area,
		double start,
		double stop,
		int digits,
		const QString& unit
	) const
{
	// Initialize the return value.
	bool rv = true;
	// Save the translation.
	p.save();
	// Set the new window origin.
	p.translate(bounds.topLeft());
	// Line pen.
	QPen p_line(color);
	QPen p_text(font_color);
	QRect rect(QPoint(0, 0), bounds.size());
	// Area allowed painting in. Adjust for the translation of the painter.
	QRect area_rect(area - bounds.topLeft());
	// Drawing text should not paint the background.
	p.setBackgroundMode(Qt::TransparentMode);
	//p.setBackgroundMode(Qt::OpaqueMode);
	// Calculate the range of the ruler.
	double range = stop - start;
	// Determine if the ruler values are inverted.
	bool inverted = start > stop;
	//
	QFontMetrics fm(p.font());
	// Get easy to access font height.
	int font_height = fm.height();
	// Get easy to access font width.
	int font_width = fm.averageCharWidth();
	// Calculate the wanted line length.
	int line_len = font_width * 3 / 2;
	// if the range is zero do nothing.
	if (fabs(range) > 0 && !rect.isEmpty())
	{
		// Catch a possible division by zero errors.
		try
		{
			if (ro == roLeft || ro == roRight)
			{
				// Calculate the units per pixel.
				double units_per_pixel = range / rect.height();
				// Calculate the minimum distance taking the font height into account.
				double min_dist = units_per_pixel * font_height * _sepFactor;
				// Get the rounded minimum distance of 'precision' digits.
				constexpr int precision = 1;
				std::string tmp = stringf("%+.*le", precision, min_dist);
				// Get the weight of the distance.
				int dist_weight = std::stoi(tmp.substr(precision + 4));
				// Convert dist string to a value.
				double dist = std::stod(tmp);
				// Remove the comma from the precision part.
				tmp = tmp.substr(0, precision + 3).erase(2, 1);
				// Get the value from the list that has a nice dist value.
				int dist_prec = abs((int) std::stod(tmp));
				// Pick the nearest from the list.
				int ticks = 0;
				for (auto& i: _tickListVertical)
				{
					if (i.Value >= dist_prec)
					{
						dist = double(inverted ? -1.0 : 1.0) * i.Value;
						dist *= pow(10.0, dist_weight - 1);
						ticks = i.Ticks;
						break;
					}
				}
				// Get the nearest value to start with dealing when inverted.
				double real_start = start - fmodulo(start, dist);
				// Calculate the pixels between two values.
				int dist_height = static_cast<int>(std::round(dist / units_per_pixel));
				// Specify the end position.
				double end_pos = stop + dist;
				// Height for faster access.
				int height = rect.height();
				// Y-point where the last text was drawn.
				int last_yp = -1;
				// Iterate through the positions on the screen.
				for (double y_pos = real_start; inverted ? (y_pos > end_pos) : (y_pos < end_pos); y_pos += dist) // NOLINT(cert-flp30-c)
				{
					// Set the line color.
					p.setPen(p_line);
					// Scale into the real plot.
					int yp = height - calculateOffset(y_pos, start, stop, height, false);
					// Adjust for last tick.
					if (yp == height)
					{
						yp = height - 1;
					}
					// Draw the major tick.
					if (rect.contains(0, yp))
					{
						if (ro == roLeft)
						{
							p.drawLine(QPoint(rect.width() - 1, yp), QPoint(rect.width() - line_len - 1, yp));
						}
						else
						{
							p.drawLine(QPoint(0, yp), QPoint(line_len, yp));
						}
					}
					else
					{
						continue;
					}
					// Draw the ticks in between.
					for (int i = 1; i < ticks; i++)
					{
						int y = yp + (dist_height * i) / ticks;
						// Adjust for last tick.
						if (y == height)
						{
							y = height - 1;
						}
						// Do not allow drawing outside the clip rect.
						if (y < rect.top() || y >= rect.bottom())
						{
							continue;
						}
						if (ro == roLeft)
						{
							p.drawLine(QPoint(rect.width() - 1, y), QPoint(rect.width() - 1 - line_len / 2, y));
						}
						else
						{
							p.drawLine(QPoint(0, y), QPoint(line_len / 2, y));
						}
					}
					// Get the to be printed value as text and prevent tiny value to be mistaken for zero.
					auto text = QString::fromStdString(numberString<double>((abs(y_pos) < abs(units_per_pixel) / 2) ? 0.0 : y_pos, digits, true));
					// Get the size of the to be drawn text. Seems it is to narrow, so it is adjusted to fit the text.
					auto tr = fm.boundingRect(text).adjusted(-1, 0, 1, 0);
					// Paint the text values.
					QPoint pt(0, yp - font_height / 2);
					if (ro == roLeft)
					{
						pt.setX(rect.width() - line_len);
					}
					else
					{
						pt.setX(rect.width() - 1);
					}
					// Set the text color.
					p.setPen(p_text);
					// Check if the text is within bounds.
					auto ttr = tr;
					ttr.moveTo(pt.x() - tr.width() - font_width / 2, pt.y());
					// Check if the text is within the bounds.
					if (area_rect.contains(ttr))
					{
						// When the text does not intersect with the ruler do not draw it at all.
						if (rect.intersects(ttr))
						{
							p.drawText(ttr, Qt::AlignCenter, text);
							last_yp = pt.y();
						}
					}
					else
						// Check if it can be placed a bit lower or higher
					{
						// Check if the proposed tick line is in the rectangle so that painting text is needed.
						if (area_rect.contains(QPoint(0, yp)))
						{
							// Figure out the top or bottom location?
							int y = (pt.y() < 0) ? 0 : (yp - tr.height() + 1);
							// This will be the top text entry of the ruler.
							tr.moveTo(pt.x() - tr.width() - font_width / 2, y);
							p.drawText(tr, Qt::AlignCenter, text);
						}
					}
				}
				// When it was not set make our own.
				if (last_yp == -1)
				{
					last_yp = (rect.height() - font_height) / 2;
				}
				else
				{
					last_yp += dist_height / 2;
					// When the unit placed outside the area correct it.
					if (last_yp + font_height > rect.height())
					{
						last_yp = rect.height() / 2 - font_height;
					}
				}
				// Draw the units on the ruler.
				auto text = QString("[%1]").arg(unit);
				// Get the size of the to be drawn text.
				auto tr = fm.boundingRect(text);
				// Move the text rect to the horizontal center.
				tr.moveTo((rect.width() - tr.width()) / 2, last_yp);
				// Set the text color.
				p.setPen(p_text);
				// Draw the unit text.
				p.drawText(tr, Qt::AlignCenter, text);
			}
			else
			{
				// Calculate the units per pixel.
				double units_per_pixel = range / rect.width();
				// Calculate the minimum distance when taking the font width into account.
				const auto max_chars = digits + 9;
				double min_dist = units_per_pixel * font_width * max_chars;
				// Get the rounded minimum distance of 'precision' digits.
				constexpr int precision = 1;
				std::string tmp = stringf("%+.*le", precision, min_dist);
				// Get the weight of the distance.
				int dist_weight = std::stoi(tmp.substr(precision + 4));
				// Convert dist string to a value.
				double dist = std::stod(tmp);
				// Remove the comma from the precision part.
				tmp = tmp.substr(0, precision + 3).erase(2, 1);
				// Get the value from the list that has a nice dist value.
				int dist_prec = abs((int) std::stod(tmp));
				// Pick the nearest from the list.
				int ticks = 0;
				for (auto& i: _tickListHorizontal)
				{
					if (i.Value >= dist_prec)
					{
						dist = double(inverted ? -1.0 : 1.0) * i.Value;
						dist *= pow(10.0, dist_weight - 1);
						ticks = i.Ticks;
						break;
					}
				}
				// Get the nearest value to start with dealing when inverted.
				double real_start = start - fmodulo(start, dist);
				// Calculate the pixels between two values.
				int dist_width = static_cast<int>(std::round(dist / units_per_pixel));
				// Specify the end position.
				double end_pos = stop + dist;
				// Height for faster access.
				int width = rect.width();
				// X-point where the last text was drawn.
				int last_xp = 0;
				QRect last_tr;
				// Iterate through the positions on the ruler.
				for (double x_pos = real_start; inverted ? (x_pos > end_pos) : (x_pos < end_pos); x_pos += dist) // NOLINT(cert-flp30-c)
				{
					// Set the line color.
					p.setPen(p_line);
					// Scale into the real plot.
					int xp = calculateOffset(x_pos, start, stop, width, false);
					// Adjust for last tick.
					if (xp == width)
					{
						xp = width - 1;
					}
					// Draw the major tick.
					if (rect.contains(xp, 0))
					{
						if (ro == roTop)
						{
							p.drawLine(QPoint(xp, rect.height() - 1), QPoint(xp, rect.height() - line_len - 1));
						}
						else
						{
							p.drawLine(QPoint(xp, 0), QPoint(xp, line_len));
						}
					}
					// Draw the ticks in between.
					for (int i = 1; i < ticks; i++)
					{
						int x = xp + (dist_width * i) / ticks;
						// Adjust for last tick.
						if (x == width)
						{
							x = width - 1;
						}
						// Only draw as far as it is allowed.
						if (x < rect.left() || x >= rect.right())
						{
							continue;
						}
						if (ro == roTop)
						{
							p.drawLine(QPoint(x, rect.height() - 1), QPoint(x, rect.height() - 1 - line_len / 2));
						}
						else
						{
							p.drawLine(QPoint(x, 0), QPoint(x, line_len / 2));
						}
					}
					// Get the to be printed value as text and prevent tiny value to be mistaken for zero.
					auto text = QString::fromStdString(numberString<double>((abs(x_pos) < abs(units_per_pixel) / 2) ? 0.0 : x_pos, digits, true));
					// Get the size of the to be drawn text.
					auto tr = fm.boundingRect(text);
					tr.setWidth(tr.width() + 2);
					QPoint pt(xp, 0);
					// Paint the text values.
					if (ro == roTop)
					{
						pt.setY(rect.height() - line_len - font_height);
					}
					else
					{
						pt.setY(line_len);
					}
					// Set the text color.
					p.setPen(p_text);
					// Check if the text is within bound.
					auto ttr = tr;
					ttr.moveTo(pt.x() - tr.width() / 2, pt.y());
					if (area_rect.contains(ttr))
					{
						// When the text does not intersect with the ruler do not draw it at all.
						if (rect.intersects(ttr))
						{
							p.drawText(ttr, Qt::AlignCenter, text);
							last_tr = ttr;
							last_xp = pt.x();
						}
					}
					else
					{
						ttr = tr;
						// When not the first value.
						if (xp)
						{
							// Move the text rectangle into the bounding rect.
							ttr.moveTo(pt.x() - tr.width(), pt.y());
							// Check if the text is still within area and does touch the text before.
							if (area_rect.contains(ttr) && !last_tr.intersects(ttr))
							{
								p.drawText(ttr, Qt::AlignCenter, text);
								last_tr = ttr;
								last_xp = pt.x();
							}
						}
						else
						{
							// Move the text rectangle into the bounding rect.
							ttr.moveTo(pt.x(), pt.y());
							p.drawText(ttr, Qt::AlignCenter, text);
							last_tr = ttr;
							last_xp = pt.x();
						}
					}
				}
				// When it was set continue.
				if (last_xp)
				{
					// Draw the units on the ruler.
					auto text = QString("[%1]").arg(unit);
					// Get the size of the to be drawn text.
					auto tr = fm.boundingRect(text);
					// Set the location depending on top or bottom.
					tr.moveTo(rect.width() - tr.width() - font_width / 2, ro == roBottom ? rect.height() - tr.height() - 1 : 0);
					// Set the text color.
					p.setPen(p_text);
					// Draw the unit text.
					p.drawText(tr, Qt::AlignCenter, text);
				}
			}
		}
		catch (...)
		{
			rv = false;
		}
	}
	// Restore the new window translation/origin.
	p.restore();
	// Signal success or failure.
	return rv;
}

bool Draw::gridLines
	(
		QPainter& p,
		EGridOrientation go,
		const QColor& color,
		const QRect& bounds,
		double start,
		double stop,
		unsigned digits
	) const
{
	// Initialize the return value.
	bool rv = true;
	// Save the translation.
	p.save();
	// Set the new window origin.
	p.translate(bounds.topLeft());
	// Set the line color.
	p.setPen(color);
	// Get the bounds after translation.
	QRect rect(QPoint(0, 0), bounds.size());
	// Length for scaling which is depending on the orientation.
	int len = (go == goHorizontal) ? rect.height() : rect.width();
	// Calculate the range of the ruler.
	double range = stop - start;
	bool inverted = start > stop;
	// if the range is zero do nothing.
	if (fabs(range) > 0)
	{
		// Catch a possible division by zero errors.
		try
		{
			// Calculate the units per pixel.
			double units_per_pixel = range / len;
			// Calculate the minimum distance when taking the font height into account.
			double min_dist;
			if (go == goHorizontal)
			{
				min_dist = units_per_pixel * QFontMetrics(p.font()).height() * _sepFactor;
			}
			else
			{
				// Calculate the minimum distance when taking the font width into account.
				const auto max_chars = digits + 9;
				min_dist = units_per_pixel * QFontMetrics(p.font()).averageCharWidth() * max_chars;
			}
			// Get the rounded minimum distance of 'precision' digits.
			constexpr int precision = 1;
			auto tmp = stringf("%+.*le", precision, min_dist);
			// Get the weight of the distance.
			int dist_weight = std::stoi(tmp.substr(precision + 4));
			// Convert dist string to a value.
			double dist = std::stod(tmp);
			// Remove the comma from the precision part.
			tmp = tmp.substr(0, precision + 3).erase(2, 1);
			// Get the value from the list that has a nice dist value.
			int dist_prec = abs((int) std::stod(tmp));
			// Pick the nearest from the list.
			for (auto& i: (go == goHorizontal) ? _tickListVertical : _tickListHorizontal)
			{
				if (i.Value >= dist_prec)
				{
					dist = double(inverted ? -1.0 : 1.0) * i.Value;
					dist *= pow(10.0, dist_weight - 1);
					break;
				}
			}
			// Get the nearest value to start with dealing when inverted.
			double real_start = start - fmodulo(start, dist);
			// Determine the end position of the loop.
			double end_pos = stop + dist;
			// Iterate through the positions on the screen.
			for (double pos = real_start; inverted ? (pos > end_pos) : (pos < end_pos); pos += dist) // NOLINT(cert-flp30-c)
			{
				// Draw the line
				if (go == goHorizontal)
				{
					// Scale into the real plot.
					int p1 = len - calculateOffset(pos, start, stop, len, false);
					// Adjust for last tick.
					if (p1 == len)
					{
						p1 = len - 1;
					}
					if (p1 >= 0 && p1 < len)
					{
						p.drawLine(QPoint(0, p1), QPoint(rect.width() - 1, p1));
					}
				}
				else
				{
					// Scale into the real plot.
					int p1 = calculateOffset(pos, start, stop, len, false);
					// Adjust for last tick.
					if (p1 == len)
					{
						p1 = len - 1;
					}
					if (p1 >= 0 && p1 < len)
					{
						p.drawLine(QPoint(p1, 0), QPoint(p1, rect.height() - 1));
					}
				}
			}
		}
		catch (...)
		{
			rv = false;
		}
	}
	// Restore the new window translation/origin.
	p.restore();
	// Return success or failure.
	return rv;
}

bool Draw::textCross(QPainter& painter, const QRect& bounds, const QString& text, const QColor& color)
{
	auto pen = QPen(color);
	pen.setWidth(1);
	painter.setPen(color);
	// Get font sizes to calculate needed widths and heights.
	QFontMetrics fm(painter.font());
	auto w_adjust = fm.maxWidth() / 4;
	auto h_adjust = fm.height() / 4;
	QRect trc = fm.boundingRect(text).adjusted(-w_adjust, -h_adjust, w_adjust, h_adjust);
	// Move text rect to center of graph area
	trc.moveCenter(bounds.center());
	painter.drawRect(trc);
	painter.drawText(trc, Qt::AlignCenter, text);
	// Draw a cross in the plot area.
	painter.drawLine(trc.topLeft(), bounds.topLeft());
	painter.drawLine(trc.bottomLeft(), bounds.bottomLeft());
	painter.drawLine(trc.topRight(), bounds.topRight());
	painter.drawLine(trc.bottomRight(), bounds.bottomRight());
	return true;
}

}