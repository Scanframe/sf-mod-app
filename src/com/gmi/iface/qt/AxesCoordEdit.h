#pragma once

#include <QFrame>
#include <QObject>
#include <QtUiPlugin/QDesignerExportWidget>
#include <misc/qt/ObjectExtension.h>

namespace sf
{

/**
 * @brief Widget for editing #sf::Variable value types integer, floating point and single line string.
 */
class QDESIGNER_WIDGET_EXPORT AxesCoordEdit
	: public QWidget
	, ObjectExtension
{
		Q_OBJECT
		/*
		Q_PROPERTY(bool focusFrame READ hasFocusFrame WRITE setFocusFrame)
		Q_PROPERTY(int nameLevel READ nameLevel WRITE setNameLevel)
		*/

	public:
		/**
		 * @brief Constructor.
		 */
		explicit AxesCoordEdit(QWidget* parent = nullptr);

	protected:
		/**
		 * @brief Overridden from base class '#ObjectExtension'.
		 */
		bool isRequiredProperty(const QString& name) override;

	private:
		/**
		 * @brief Forward definition of private implemented class to prevent interfaces exposure.
		 */
		struct Private;
		/**
		 * @brief pointer to private class holding the needed data members.
		 */
		Private* _p;

		Q_DISABLE_COPY(AxesCoordEdit)
};

}// namespace sf
