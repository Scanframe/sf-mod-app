#pragma once

#include <QPaintEvent>
#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
#include <gii/qt/Macros.h>
#include <misc/qt/ObjectExtension.h>

class QStyleOptionFrame;

namespace sf
{

class QDESIGNER_WIDGET_EXPORT AscanGraph
	: public QWidget
	, public ObjectExtension
{
		Q_OBJECT
		// Need full namespace for this property macro.
		Q_PROPERTY(sf::gii::IdType idTimeUnit READ getIdTimeUnit WRITE setIdTimeUnit)
		Q_PROPERTY(sf::gii::IdType idDelay READ getIdDelay WRITE setIdDelay)
		Q_PROPERTY(sf::gii::IdType idRange READ getIdRange WRITE setIdRange)
		Q_PROPERTY(sf::gii::IdType idAmplitudeUnit READ getIdAmplitudeUnit WRITE setIdAmplitudeUnit)
		Q_PROPERTY(sf::gii::IdType idData READ getIdData WRITE setIdData)
		Q_PROPERTY(sf::gii::IdType idAttenuation READ getIdAttenuation WRITE setIdAttenuation)

	public:
		explicit AscanGraph(QWidget* parent = nullptr);

		~AscanGraph() override;

		void addPropertyPages(sf::PropertySheetDialog* sheet) override;

		[[nodiscard]] QSize minimumSizeHint() const override;

		bool isRequiredProperty(const QString& name) override;

		SF_DECL_INFO_ID(IdTimeUnit)

		SF_DECL_INFO_ID(IdDelay)

		SF_DECL_INFO_ID(IdRange)

		SF_DECL_INFO_ID(IdAmplitudeUnit)

		SF_DECL_INFO_ID(IdData)

		SF_DECL_INFO_ID(IdAttenuation)

	protected:
		void paintEvent(QPaintEvent* event) override;

		void initStyleOption(QStyleOptionFrame* option) const;

	private:
		struct Private;
		Private* _p;
};

}// namespace sf
