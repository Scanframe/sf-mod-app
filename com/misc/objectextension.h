#ifndef MISC_OBJECTEXTENSION_H
#define MISC_OBJECTEXTENSION_H

#include <QObject>
#include <QtDesigner/QAbstractFormBuilder>

#include "global.h"

namespace sf
{
/**
 * Interface class to multiple inherit when the sf::FormWriter class
 * needs to be restricted in writing object properties.
 */
class _MISC_CLASS ObjectExtension
{
	protected:
		/**
		 * Protected constructor only. Inlined so no external libs are need for designer.
		 * @param object This pointer of the extended class instance.
		 */
		explicit ObjectExtension(QObject* object);

	public:
		/**
		 * Returns true if a property is to be saved to file.
		 * @param name Name of the property.
		 * @return True saving is required.
		 */
		virtual bool isRequiredProperty(const QString& name) = 0;

	private:
		/**
		 * Holds the concerned base object which this class is extending.
 		 */
		QObject& _object;
};

inline
ObjectExtension::ObjectExtension(QObject* object)
	:_object(*object) {}

} // namespace

#endif
