#pragma once

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QPaintEvent>
#include <misc/qt/ObjectExtension.h>
#include <gii/qt/Macros.h>

class QStyleOptionFrame;

namespace sf
{

class QDESIGNER_WIDGET_EXPORT BscanGraph :public QWidget, public ObjectExtension
{
	Q_OBJECT
		// Need full namespace for this property macro.
		Q_PROPERTY(sf::Gii::IdType idData READ getIdData WRITE setIdData)
		Q_PROPERTY(sf::Gii::IdType idIndex READ getIdIndex WRITE setIdIndex)
		Q_PROPERTY(sf::Gii::IdType idTimeUnit READ getIdTimeUnit WRITE setIdTimeUnit)
		Q_PROPERTY(sf::Gii::IdType idTimeDelay READ getIdTimeDelay WRITE setIdTimeDelay)
		Q_PROPERTY(sf::Gii::IdType idTimeRange READ getIdTimeRange WRITE setIdTimeRange)
		Q_PROPERTY(sf::Gii::IdType IdCursorEvent READ getIdCursorEvent WRITE setIdCursorEvent)
		Q_PROPERTY(sf::Gii::IdType idCursorFraction READ getIdCursorFraction WRITE setIdCursorFraction)
		Q_PROPERTY(sf::Gii::IdType idCursorIndex READ getIdCursorIndex WRITE setIdCursorIndex)
		Q_PROPERTY(sf::Gii::IdType IdCursorPopIndex READ getIdCursorPopIndex WRITE setIdCursorPopIndex)
		Q_PROPERTY(sf::Gii::IdType idScanDelay READ getIdScanDelay WRITE setIdScanDelay)
		Q_PROPERTY(sf::Gii::IdType idScanRange READ getIdScanRange WRITE setIdScanRange)
		Q_PROPERTY(sf::Gii::IdType idScanLeft READ getIdScanLeft WRITE setIdScanLeft)
		Q_PROPERTY(sf::Gii::IdType idScanRight READ getIdScanRight WRITE setIdScanRight)

	public:
		explicit BscanGraph(QWidget* parent = nullptr);

		~BscanGraph() override;

		void addPropertyPages(sf::PropertySheetDialog* sheet) override;

		[[nodiscard]] QSize minimumSizeHint() const override;

		bool isRequiredProperty(const QString& name) override;

		SF_DECL_INFO_ID(IdData)

		SF_DECL_INFO_ID(IdIndex)

		SF_DECL_INFO_ID(IdTimeUnit)

		SF_DECL_INFO_ID(IdTimeDelay)

		SF_DECL_INFO_ID(IdTimeRange)

		SF_DECL_INFO_ID(IdCursorEvent)

		SF_DECL_INFO_ID(IdCursorFraction)

		SF_DECL_INFO_ID(IdCursorIndex)

		SF_DECL_INFO_ID(IdCursorPopIndex)

		SF_DECL_INFO_ID(IdScanDelay)

		SF_DECL_INFO_ID(IdScanRange)

		SF_DECL_INFO_ID(IdScanLeft)

		SF_DECL_INFO_ID(IdScanRight)

	protected:

		void paintEvent(QPaintEvent* event) override;

		void resizeEvent(QResizeEvent* event) override;

		void mousePressEvent(QMouseEvent* event) override;

		void mouseReleaseEvent(QMouseEvent* event) override;

		void mouseMoveEvent(QMouseEvent* event) override;

		void keyPressEvent(QKeyEvent* event) override;

		void keyReleaseEvent(QKeyEvent* event) override;
/*

		void focusInEvent(QFocusEvent* event) override;

		void focusOutEvent(QFocusEvent* event) override;
*/


		void initStyleOption(QStyleOptionFrame* option) const;

	private:
		struct Private;
		Private* _p;
};

}
