#pragma once

#include <bitset>
#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <misc/qt/ObjectExtension.h>
#include <misc/qt/Macros.h>
#include <gii/qt/Macros.h>

class QStyleOptionFrame;

class QPaintEvent;

namespace sf
{

class AcquisitionControl :public QWidget, public ObjectExtension
{
	Q_OBJECT

	public:
		explicit AcquisitionControl(QWidget* parent = nullptr);

		~AcquisitionControl() override;

		void addPropertyPages(sf::PropertySheetDialog* sheet) override;

		[[nodiscard]] QSize minimumSizeHint() const override;

		bool isRequiredProperty(const QString& name) override;

	protected:
		void keyPressEvent(QKeyEvent* event) override;

		void mousePressEvent(QMouseEvent* event) override;

		void mouseReleaseEvent(QMouseEvent* event) override;

		void paintEvent(QPaintEvent* event) override;

		void focusInEvent(QFocusEvent* event) override;

		void focusOutEvent(QFocusEvent* event) override;

		void resizeEvent(QResizeEvent* event) override;

		void mouseMoveEvent(QMouseEvent* event) override;

	public:

		Q_PROPERTY(int gripHeight READ getGripHeight WRITE setGripHeight)

		SF_DECL_PROP_GS(int, GripHeight)

		// The absolute maximum drawable signal value.
		Q_PROPERTY(int valueMax READ getValueMax WRITE setValueMax)

		SF_DECL_PROP_GS(int, ValueMax)

		// Makes the threshold be changed using dragging.
		Q_PROPERTY(bool thresholdDrag READ getThresholdDrag WRITE setThresholdDrag)

		SF_DECL_PROP_GS(bool, ThresholdDrag)

		// The value of the threshold is linked to the value range of the data.
		Q_PROPERTY(bool thresholdLinked READ getThresholdLinked WRITE setThresholdLinked)

		SF_DECL_PROP_GS(bool, ThresholdLinked)

		Q_PROPERTY(bool gridEnabled READ getGridEnabled WRITE setGridEnabled)

		SF_DECL_PROP_GS(bool, GridEnabled)

		Q_PROPERTY(QColor colorForeground READ getColorForeground WRITE setColorForeground)

		SF_DECL_PROP_GS(QColor, ColorForeground)

		Q_PROPERTY(QColor colorBackground READ getColorBackground WRITE setColorBackground)

		SF_DECL_PROP_GS(QColor, ColorBackground)

		// Special color for the TCG line.
		Q_PROPERTY(QColor colorTcgLine READ getColorTcgLine WRITE setColorTcgLine)

		SF_DECL_PROP_GS(QColor, ColorTcgLine)

		Q_PROPERTY(QColor colorTcgRange READ getColorTcgRange WRITE setColorTcgRange)

		SF_DECL_PROP_GS(QColor, ColorTcgRange)

		// Color for 10% line.
		Q_PROPERTY(QColor colorAdjustmentLine READ getColorAdjustmentLine WRITE setColorAdjustmentLine)

		SF_DECL_PROP_GS(QColor, ColorAdjustmentLine)

		// Color for the grid lines.
		Q_PROPERTY(QColor colorGridLines READ getColorGridLines WRITE setColorGridLines)

		SF_DECL_PROP_GS(QColor, ColorGridLines)

		// Period time of the sample frequency of the results.
		Q_PROPERTY(sf::Gii::IdType idTimeUnit READ getIdTimeUnit WRITE setIdTimeUnit)

		SF_DECL_INFO_ID(IdTimeUnit)

		Q_PROPERTY(sf::Gii::IdType idGateCount READ getIdGateCount WRITE setIdGateCount)

		SF_DECL_INFO_ID(IdGateCount)

		Q_PROPERTY(sf::Gii::IdType idIfShift READ getIdIfShift WRITE setIdIfShift)

		SF_DECL_INFO_ID(IdIfShift)

		// A-scan data source
		Q_PROPERTY(sf::Gii::IdType idCopyData READ getIdCopyData WRITE setIdCopyData)

		SF_DECL_INFO_ID(IdCopyData)

		// A-scan data and redirection index.
		Q_PROPERTY(sf::Gii::IdType idCopyIndex READ getIdCopyIndex WRITE setIdCopyIndex)

		SF_DECL_INFO_ID(IdCopyIndex)

		// A-scan delay and range.
		Q_PROPERTY(sf::Gii::IdType idCopyDelay READ getIdCopyDelay WRITE setIdCopyDelay)

		SF_DECL_INFO_ID(IdCopyDelay)

		Q_PROPERTY(sf::Gii::IdType idCopyRange READ getIdCopyRange WRITE setIdCopyRange)

		SF_DECL_INFO_ID(IdCopyRange)

		// TCG parameters
		Q_PROPERTY(sf::Gii::IdType idTcgEnable READ getIdTcgEnable WRITE setIdTcgEnable)

		SF_DECL_INFO_ID(IdTcgEnable)

		Q_PROPERTY(sf::Gii::IdType idTcgDelay READ getIdTcgDelay WRITE setIdTcgDelay)

		SF_DECL_INFO_ID(IdTcgDelay)

		Q_PROPERTY(sf::Gii::IdType idTcgRange READ getIdTcgRange WRITE setIdTcgRange)

		SF_DECL_INFO_ID(IdTcgRange)

		Q_PROPERTY(sf::Gii::IdType idTcgSlavedTo READ getIdTcgSlavedTo WRITE setIdTcgSlavedTo)

		SF_DECL_INFO_ID(IdTcgSlavedTo)

		// Gate 0 (If??)
		Q_PROPERTY(sf::Gii::IdType idGate0Delay READ getIdGate0Delay WRITE setIdGate0Delay)

		SF_DECL_INFO_ID(IdGate0Delay)

		Q_PROPERTY(sf::Gii::IdType idGate0Range READ getIdGate0Range WRITE setIdGate0Range)

		SF_DECL_INFO_ID(IdGate0Range)

		Q_PROPERTY(sf::Gii::IdType idGate0Threshold READ getIdGate0Threshold WRITE setIdGate0Threshold)

		SF_DECL_INFO_ID(IdGate0Threshold)

		Q_PROPERTY(sf::Gii::IdType idGate0SlavedTo READ getIdGate0SlavedTo WRITE setIdGate0SlavedTo)

		SF_DECL_INFO_ID(IdGate0SlavedTo)

		Q_PROPERTY(sf::Gii::IdType idGate0TrackRange READ getIdGate0TrackRange WRITE setIdGate0TrackRange)

		SF_DECL_INFO_ID(IdGate0TrackRange)

		Q_PROPERTY(sf::Gii::IdType idGate0Tof READ getIdGate0Tof WRITE setIdGate0Tof)

		SF_DECL_INFO_ID(IdGate0Tof)

		Q_PROPERTY(sf::Gii::IdType idGate0Amp READ getIdGate0Amp WRITE setIdGate0Amp)

		SF_DECL_INFO_ID(IdGate0Amp)

		// Gate 1 (If??)
		Q_PROPERTY(sf::Gii::IdType idGate1Delay READ getIdGate1Delay WRITE setIdGate1Delay)

		SF_DECL_INFO_ID(IdGate1Delay)

		Q_PROPERTY(sf::Gii::IdType idGate1Range READ getIdGate1Range WRITE setIdGate1Range)

		SF_DECL_INFO_ID(IdGate1Range)

		Q_PROPERTY(sf::Gii::IdType idGate1Threshold READ getIdGate1Threshold WRITE setIdGate1Threshold)

		SF_DECL_INFO_ID(IdGate1Threshold)

		Q_PROPERTY(sf::Gii::IdType idGate1SlavedTo READ getIdGate1SlavedTo WRITE setIdGate1SlavedTo)

		SF_DECL_INFO_ID(IdGate1SlavedTo)

		Q_PROPERTY(sf::Gii::IdType idGate1TrackRange READ getIdGate1TrackRange WRITE setIdGate1TrackRange)

		SF_DECL_INFO_ID(IdGate1TrackRange)

		Q_PROPERTY(sf::Gii::IdType idGate1Tof READ getIdGate1Tof WRITE setIdGate1Tof)

		SF_DECL_INFO_ID(IdGate1Tof)

		Q_PROPERTY(sf::Gii::IdType idGate1Amp READ getIdGate1Amp WRITE setIdGate1Amp)

		SF_DECL_INFO_ID(IdGate1Amp)

		// Gate 2 (If??)
		Q_PROPERTY(sf::Gii::IdType idGate2Delay READ getIdGate2Delay WRITE setIdGate2Delay)

		SF_DECL_INFO_ID(IdGate2Delay)

		Q_PROPERTY(sf::Gii::IdType idGate2Range READ getIdGate2Range WRITE setIdGate2Range)

		SF_DECL_INFO_ID(IdGate2Range)

		Q_PROPERTY(sf::Gii::IdType idGate2Threshold READ getIdGate2Threshold WRITE setIdGate2Threshold)

		SF_DECL_INFO_ID(IdGate2Threshold)

		Q_PROPERTY(sf::Gii::IdType idGate2SlavedTo READ getIdGate2SlavedTo WRITE setIdGate2SlavedTo)

		SF_DECL_INFO_ID(IdGate2SlavedTo)

		Q_PROPERTY(sf::Gii::IdType idGate2TrackRange READ getIdGate2TrackRange WRITE setIdGate2TrackRange)

		SF_DECL_INFO_ID(IdGate2TrackRange)

		Q_PROPERTY(sf::Gii::IdType idGate2Tof READ getIdGate2Tof WRITE setIdGate2Tof)

		SF_DECL_INFO_ID(IdGate2Tof)

		Q_PROPERTY(sf::Gii::IdType idGate2Amp READ getIdGate2Amp WRITE setIdGate2Amp)

		SF_DECL_INFO_ID(IdGate2Amp)

		// Gate 3 (If??)
		Q_PROPERTY(sf::Gii::IdType idGate3Delay READ getIdGate3Delay WRITE setIdGate3Delay)

		SF_DECL_INFO_ID(IdGate3Delay)

		Q_PROPERTY(sf::Gii::IdType idGate3Range READ getIdGate3Range WRITE setIdGate3Range)

		SF_DECL_INFO_ID(IdGate3Range)

		Q_PROPERTY(sf::Gii::IdType idGate3Threshold READ getIdGate3Threshold WRITE setIdGate3Threshold)

		SF_DECL_INFO_ID(IdGate3Threshold)

		Q_PROPERTY(sf::Gii::IdType idGate3SlavedTo READ getIdGate3SlavedTo WRITE setIdGate3SlavedTo)

		SF_DECL_INFO_ID(IdGate3SlavedTo)

		Q_PROPERTY(sf::Gii::IdType idGate3TrackRange READ getIdGate3TrackRange WRITE setIdGate3TrackRange)

		SF_DECL_INFO_ID(IdGate3TrackRange)

		Q_PROPERTY(sf::Gii::IdType idGate3Tof READ getIdGate3Tof WRITE setIdGate3Tof)

		SF_DECL_INFO_ID(IdGate3Tof)

		Q_PROPERTY(sf::Gii::IdType idGate3Amp READ getIdGate3Amp WRITE setIdGate3Amp)

		SF_DECL_INFO_ID(IdGate3Amp)

		// TCG Delay parameter ID's
		Q_PROPERTY(QByteArray idsTcgTime READ getIdsTcgTime WRITE setIdsTcgTime)

		SF_DECL_PROP_GRS(QByteArray, IdsTcgTime)

		Q_PROPERTY(QByteArray idsTcgGain READ getIdsTcgGain WRITE setIdsTcgGain)

		SF_DECL_PROP_GRS(QByteArray, IdsTcgGain)

	private:
		struct Private;
		Private* _p;

		void initStyleOption(QStyleOptionFrame* option) const;
};

}