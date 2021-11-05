#pragma once

#include <pal/iface/PaletteInterface.h>
#include <gii/gen/VariableHandler.h>
#include <gii/gen/Variable.h>
#include <gii/qt/Macros.h>
#include <misc/qt/Macros.h>
#include <QColor>

namespace sf
{

class AmplitudePalette :public PaletteInterface, private VariableHandler
{
	Q_OBJECT
		/**
		 * Color properties
		 */
		Q_PROPERTY(QColor colorFrom READ getColorFrom WRITE setColorFrom)
		Q_PROPERTY(QColor colorTo READ getColorTo WRITE setColorTo)
		Q_PROPERTY(QColor colorCalib READ getColorCalib WRITE setColorCalib)
		Q_PROPERTY(QColor colorOver READ getColorOver WRITE setColorOver)
		/**
		 * Calibration properties
		 */
		// Alias of the 'calibLevel' property.
		//Q_PROPERTY(int calibIndex READ getCalibIndex WRITE setCalibIndex STORED false)
		Q_PROPERTY(double calibLevel READ getCalibLevel WRITE setCalibLevel)
		Q_PROPERTY(double sizeCalib READ getSizeCalib WRITE setSizeCalib)
		/**
		 * Calibration id properties.
		 */
		Q_PROPERTY(sf::Gii::IdType idCalibMin READ getIdCalibMin WRITE setIdCalibMin)
		Q_PROPERTY(sf::Gii::IdType idCalibMax READ getIdCalibMax WRITE setIdCalibMax)
		Q_PROPERTY(sf::Gii::IdType idCalibPos READ getIdCalibPos WRITE setIdCalibPos)

	public:
		/**
		 * @brief Interface constructor
		 */
		explicit AmplitudePalette(const Parameters&);

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

		/**
		 * @brief Property getter.
		 */
		[[nodiscard]] double getCalibLevel() const;

		/**
		 * @brief Property setter.
		 */
		void setCalibLevel(double value);

		SF_DECL_PROP_GRS(QColor, ColorFrom)

		SF_DECL_PROP_GRS(QColor, ColorTo)

		SF_DECL_PROP_GRS(QColor, ColorOver)

		SF_DECL_PROP_GRS(QColor, ColorCalib)

		SF_DECL_PROP_GS(double, SizeCalib)

		SF_DECL_INFO_ID(IdCalibMin)

		SF_DECL_INFO_ID(IdCalibMax)

		SF_DECL_INFO_ID(IdCalibPos)

	private:

		/**
		 * @brief Variable event handler from base class.
		 */
		void variableEventHandler(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool same_inst) override;

		/**
		 * @brief Property change notification method.
		 */
		void notify(void*);
		/**
		 * @brief Optional calibration position variables.
		 */
		Variable _vCalibMin, _vCalibMax, _vCalibPos;
		/**
		 * @brief Holds the callback function/closure for notifying changes of the palette.
		 */
		PaletteInterface::Parameters::Callback _callback;
		/**
		 * @brief Holds Color to start at for the region bellow the threshold.
		 */
		QColor _colorFrom{Qt::black};
		/**
		 * @brief Holds Color to stop at for the region bellow the threshold.
		 */
		QColor _colorTo{Qt::green};
		/**
		 * @brief Holds the color for the calibration level.
		 */
		QColor _colorCalib{Qt::blue};
		/**
		 * @brief Holds the color for the level over the calibration level.
		 */
		QColor _colorOver{Qt::red};
		/**
		 * @brief Holds the size (0..1) for the amount of indexes used for the calibration color.
		 */
		Value::flt_type _sizeCalib{0.02};
		/**
		 * @brief Holds the calibration level.
		 */
		double _calibLevel{0.7};
};

}
