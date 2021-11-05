#pragma once

#include "global.h"
#include "misc/gen/TClosure.h"
#include <QList>
#include <QRgb>
#include <QColor>

namespace sf
{

typedef QList<QRgb> ColorTable;

typedef TClosure<qreal, qreal> ColorShiftCurve;

_PAL_FUNC void shiftHsv(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve = {});

_PAL_FUNC void shiftHsl(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve = {});

_PAL_FUNC void shiftSaturation(const QColor& color, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve = {});

_PAL_FUNC void shiftRgb(const QColor& colorBegin, const QColor& colorEnd, ColorTable::iterator fromEntry, ColorTable::iterator toEntry, const ColorShiftCurve& curve = {});

}