#include "ColorTable.h"
#include <misc/gen/gen_utils.h>
#include <misc/gen/Value.h>

namespace sf
{

void shiftHsv(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve)
{
	if (fromEntry >= toEntry)
	{
		return;
	}
	int beginH, beginS, beginV, beginA;
	int endH, endS, endV, endA;
	//
	colorBegin.getHsv(&beginH, &beginS, &beginV, &beginA);
	colorEnd.getHsv(&endH, &endS, &endV, &endA);
	//
	QColor color;
	auto len = std::distance(fromEntry, toEntry);
	for (auto i = fromEntry; i < toEntry; i++)
	{
		auto j = std::distance(fromEntry, i);
		// Get x between 0.0 and 1.0 ad a floating point value.
		auto x = calculateOffset<ptrdiff_t, Value::flt_type>(j, 0, len, 1.0, true);
		// Linear is default.
		auto level = x;
		// When curve closure has been set.
		if (curve)
		{
			level = curve(level);
		}
		//
		auto hue = beginH + calculateOffset<Value::flt_type>(level, 0.0, 1.0, endH - beginH, true);
		auto value = beginV + calculateOffset<Value::flt_type>(level, 0.0, 1.0, endV - beginV, true);
		// Set the color using the new values.
		color.setHsv(hue, beginS, value, beginA);
		// Assign the rgba value from the color.
		*i = color.rgba();
	}
}

void shiftHsl(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve)
{
	if (fromEntry >= toEntry)
	{
		return;
	}
	int beginH, beginS, beginL, beginA;
	int endH, endS, endL, endA;
	//
	colorBegin.getHsl(&beginH, &beginS, &beginL, &beginA);
	colorEnd.getHsl(&endH, &endS, &endL, &endA);
	//
	QColor color;
	auto len = std::distance(fromEntry, toEntry);
	for (auto i = fromEntry; i < toEntry; i++)
	{
		auto j = std::distance(fromEntry, i);
		auto hue = beginH + calculateOffset<ptrdiff_t>(j, 0, len, endH - beginH, true);
		auto light = beginL + calculateOffset<ptrdiff_t>(j, 0, len, endL - beginL, true);
		// Set the color using the new values.
		color.setHsl(hue, beginS, light, beginA);
		// Assign the rgba value from the color.
		*i = color.rgba();
	}
}

void shiftSaturation(const QColor& color, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve)
{
	int beginH, beginS, beginV, beginA;
	color.getHsv(&beginH, &beginS, &beginV, &beginA);
	//
	QColor col;
	auto len = std::distance(fromEntry, toEntry);
	for (auto i = fromEntry; i < toEntry; i++)
	{
		auto j = std::distance(fromEntry, i);
		// Get x between 0.0 and 1.0 ad a floating point value.
		auto x = calculateOffset<ptrdiff_t, qreal>(j, 0, len, 1.0, true);
		// Linear is default.
		auto level = x;
		// When curve closure has been set.
		if (curve)
		{
			level = curve(level);
		}
		// Calculate the new saturation value.
		auto saturation = 255 - calculateOffset<ptrdiff_t>(j, 0, len - 1, 255, true);
		// Set the color using the new values.
		col.setHsv(beginH, saturation, beginV, beginA);
		// Assign the rgba value from the color.
		*i = col.rgba();
	}
}

void shiftRgb(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve)
{
	int beginR, beginG, beginB, beginA;
	int endR, endG, endB, endA;
	//
	colorBegin.getRgb(&beginR, &beginG, &beginB, &beginA);
	colorEnd.getRgb(&endR, &endG, &endB, &endA);
	//
	QColor color;
	auto len = std::distance(fromEntry, toEntry);
	for (auto i = fromEntry; i < toEntry; i++)
	{
		auto j = std::distance(fromEntry, i);
		auto red = beginR + calculateOffset<ptrdiff_t>(j, 0, len, endR - beginR, true);
		auto green = beginG + calculateOffset<ptrdiff_t>(j, 0, len, endG - beginG, true);
		auto blue = beginB + calculateOffset<ptrdiff_t>(j, 0, len, endB - beginB, true);
		auto alpha = beginA + calculateOffset<ptrdiff_t>(j, 0, len, endA - beginA, true);
		// Set the color using the new values.
		color.setRgb(red, green, blue, alpha);
		// Assign the rgba value from the color.
		*i = color.rgba();
	}
}

}
