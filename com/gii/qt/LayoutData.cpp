#include <QGuiApplication>
#include "LayoutData.h"

namespace sf
{

struct LayoutData::Private
{
	static QString getDataObjectName()
	{
		return {"__sf_layout_data__"};
	}

	LayoutData*_ld;
	QDir _directory{};
	Gii::IdType _idOffset{};
	bool _readOnly{true};

	explicit Private(LayoutData* layoutData)
	:_ld(layoutData)
	{
		_ld->_p = this;
	}
};

LayoutData::LayoutData(QObject* parent)
:QObject(parent)
{
	setObjectName(Private::getDataObjectName());
	new Private(this);
}

LayoutData* LayoutData::from(QObject* obj)
{
	QObject* o = obj;
	// Iterate down to the root of the passed object.
	while(o)
	{
		// Check if the instance has a LayoutData direct child.
		if (auto ld = o->findChild<LayoutData*>(Private::getDataObjectName(), Qt::FindDirectChildrenOnly))
		{
			return ld;
		}
		o = o->parent();
	}
	return nullptr;
}

void LayoutData::openPropertyEditor(QObject* target)
{
	// Deliberately empty.
}

void LayoutData::popupContextMenu(QObject* target, const QPoint& pos)
{
	// Deliberately empty.
}

void LayoutData::setReadOnly(bool ro)
{
	_p->_readOnly = ro;
}

bool LayoutData::getReadOnly() const
{
	return _p->_readOnly;
}

QString LayoutData::getFileSuffix()
{
	return "ui";
}

QDir LayoutData::getDirectory() const
{
	return _p->_directory;
}

void LayoutData::setDirectory(const QDir& directory)
{
	_p->_directory = directory;
}

Gii::IdType LayoutData::getIdOffset() const
{
	return _p->_idOffset;
}

void LayoutData::setIdOffset(Gii::IdType idOffset) const
{
	_p->_idOffset = idOffset;
}

bool LayoutData::hasMenuModifiers()
{
	return QGuiApplication::keyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier);
}

}
