#pragma once

#include <pal/iface/PaletteInterface.h>
#include <misc/qt/Macros.h>
#include <QColor>

namespace sf
{

class SimplePalette :public PaletteInterface
{
	Q_OBJECT
		Q_PROPERTY(Mode mode READ getMode WRITE setMode)
		Q_PROPERTY(QColor colorFrom READ getColorFrom WRITE setColorFrom)
		Q_PROPERTY(QColor colorTo READ getColorTo WRITE setColorTo)
		Q_PROPERTY(QColor colorCenter READ getColorCenter WRITE setColorCenter)
		Q_PROPERTY(QString formula READ getFormula WRITE setFormula)

	public:
		/**
		 * @brief Mode for generation of colors.
		 */
		enum Mode
		{
			/* Shifts the color rgb values linear */
			ShiftRgb,
			/* Shifts hue and value linear. */
			ShiftHsv,
			/* Shifts hue and lightness linear. */
			ShiftHsl,
		};

		Q_ENUM(Mode);

		/**
		 * @brief Interface constructor
		 */
		explicit SimplePalette(const Parameters&);

		/**
		 * @brief Overridden from base class.
		 */
		void addPropertyPages(PropertySheetDialog* sheet) override;

		/*
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] ColorTable getColorTable() const override;

		/*
		 * @brief Overridden from base class.
		 */
		[[nodiscard]] qreal getCurveLevel(qreal x) const override;

		SF_DECL_PROP_GS(QColor, ColorFrom)

		SF_DECL_PROP_GS(QColor, ColorTo)

		SF_DECL_PROP_GS(QColor, ColorCenter)

		SF_DECL_PROP_GS(Mode, Mode)

		SF_DECL_PROP_GRS(QString, Formula)

	private:
		/**
		 * @brief Called by property setter method when a change occurred.
		 */
		void notify(void*);
		/**
		 * @brief Holds Color to start at.
		 */
		QColor _colorFrom{Qt::red};
		/**
		 * @brief Holds Color to stop at.
		 */
		QColor _colorTo{Qt::blue};
		/**
		 * @brief Holds the color in the center.
		 */
		QColor _colorCenter{0, 0, 0, 0};
		/**
		 * @brief Holds the palette mode value.
		 */
		Mode _mode{ShiftRgb};
		/**
		 * @brief Holds the scrip calculating. Like: 'sin(x * PI / 2)' or '1.0 - pow(1.0 - x, 3.0)'
		 */
		QString _formula{};
};

}
