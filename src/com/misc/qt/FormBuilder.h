#pragma once

#include <QtDesigner/QFormBuilder>

#include "../global.h"

// Forward definition.
class QDomDocument;

namespace sf
{
/**
 * @brief Derived class to be able to prevent some properties to be stored when written to file.
 */
class _MISC_CLASS FormBuilder :public ::QFormBuilder
{
	public:
		/**
		 * @brief Overridden from QFormBuilder base class.
		 */
		QWidget* load(QIODevice* dev, QWidget* parentWidget) override;

		/**
		 * @brief Overridden from QFormBuilder base class.
		 */
		void save(QIODevice* dev, QWidget* widget) override;

	protected:
		/**
		 * @brief Overridden from QFormBuilder base class.
		 */
		QList<DomProperty*> computeProperties(QObject* obj) override;

		/**
		 * @brief Fixes the missing property dom elements when saving the dom.
		 */
		void fixSavingProperties(QWidget* widget, QDomDocument& dom);

		/**
		 * @brief Fixes the missing property dom elements when loading the dom.
		 */
		void fixLoadingProperties(QWidget* widget, QDomDocument& dom);
};

}

