#pragma once

#include <QtDesigner/QFormBuilder>

#include "global.h"

namespace sf
{
/**
 * Derived class to be able to prevent some properties to be stored when written to file.
 */
class _MISC_CLASS FormWriter :public ::QFormBuilder
{
	protected:
		QList<DomProperty*> computeProperties(QObject* obj) override;
};

}

