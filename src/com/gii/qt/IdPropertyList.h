#pragma once
#include <QObject>
#include <QList>
#include "InformationIdEdit.h"
#include "../global.h"

namespace sf
{

/**
 * @brief Class making checking of widgets Information ID's easier.
 */
class _GII_CLASS  IdPropertyList
{
	public:
		/**
		 * @brief Constructor.
		 *
		 * @param target Targeted object having the properties.
		 */
		explicit IdPropertyList(QObject* target);

		/**
		 * @brief Adds a property name and corresponding ID edit widget.
		 *
		 * @param name Name of the property.
		 * @param iie Pointer of the ID edit widget.
		 */
		void add(const QByteArray& name, InformationIdEdit* iie);

		/**
		 * @brief Method which updates the ID edit widgets using the property value.
		 */
		void update();

		/**
		 * @brief Method which sets the target property using the ID edit widget's value.
		 */
		void apply();

		/**
		 * @brief Gets the modified status of the ID edit widgets.
		 * This method corresponds with the #sf::PropertyPage::isPageModified() methods.
		 * @return True when one of the ID edit widgets has changed.
		 */
		[[nodiscard]] bool isModified() const;

	private:
		/**
		 * @brief Container type for name and id edit widget.
		 */
		typedef QList <QPair<QByteArray, InformationIdEdit*>> ListType;
		/**
		 * @brief Holds the target object having the properties containing ID's.
		 */
		QObject* _target;
		/**
		 * @brief Holds the property names and the corresponding edit widgets in a dialog.
		 */
		ListType _list;
};

}