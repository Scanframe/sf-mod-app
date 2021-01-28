#ifndef MISC_QFORMWRITER_H
#define MISC_QFORMWRITER_H

#include <QtDesigner/QFormBuilder>

#include "global.h"

namespace sf
{

class _MISC_CLASS QFormWriter : public ::QFormBuilder
{
	protected:
		QList<DomProperty*> computeProperties(QObject* obj) override;
};

}

#endif //MISC_QFORMWRITER_H
