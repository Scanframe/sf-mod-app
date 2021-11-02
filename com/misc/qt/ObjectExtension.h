#pragma once

#include <QObject>
#include <QtDesigner/QAbstractFormBuilder>

#include "../global.h"

namespace sf
{

// Forward definition.
class PropertySheetDialog;

/**
 * @brief Interface class to multiple inherit when the sf::FormWriter class
 * needs to be restricted in writing object properties.
 */
class _MISC_CLASS ObjectExtension
{
	protected:
		/**
		 * @brief Protected constructor only. Inlined so no external libs are need for designer.
		 *
		 * @param object This pointer of the extended class instance.
		 */
		explicit ObjectExtension(QObject* object);

	public:
		/**
		 * @brief Returns true if a property is to be saved to file.
		 * @param name Name of the property.
		 * @return True saving is required.
		 */
		virtual bool isRequiredProperty(const QString& name) = 0;

		/**
		 * @brief Adds the pages specific for this object.
		 *
		 * @param sheet Parent and property sheet for the page.
		 */
		virtual void addPropertyPages(sf::PropertySheetDialog* sheet);

		/**
		 * @brief Gets if the children needs saving by the FormBuilder class.
		 *
		 * @return False by default.
		 */
		[[nodiscard]] virtual bool getSaveChildren() const;

		/**
		 * @brief Gets the designer status.
		 * @return True when in the designer application.
		 */
		static bool inDesigner();

		/**
		 * @brief Return if the passed object has this type as parent.
		 *
		 * It starts by testing the passed object itself.
		 *
		 * @param obj Object to start with in the search.
		 * @param first When false is passed it returns the last one found in the path.
		 * @return True when a parent was found that matches.
		 */
		static QObject* getExtensionParent(QObject* obj, bool first = true);

	private:
		/**
		 * @brief Holds the concerned base object which this class is extending.
 		 */
		QObject* _object;
};

}
