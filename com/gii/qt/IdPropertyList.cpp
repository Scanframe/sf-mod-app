#include "IdPropertyList.h"

namespace sf
{

IdPropertyList::IdPropertyList(QObject* obj)
	:_target(obj)
{
}

void IdPropertyList::add(const QByteArray& name, InformationIdEdit* iie)
{
	_list.append({name, iie});
}

void IdPropertyList::update()
{
	for (auto& e: _list)
	{
		e.second->setId(_target->property(e.first).value<Gii::TypeId>());
	}
}

void IdPropertyList::apply()
{
	for (auto& e: _list)
	{
		_target->setProperty(e.first, QVariant::fromValue(e.second->getId()));
	}
}

bool IdPropertyList::isModified() const
{
	return std::find_if(_list.begin(), _list.end(), [&](const ListType::value_type& e) -> bool
	{
		return _target->property(e.first.data()).value<Gii::IdType>() != e.second->getId();
	}) != _list.end();
}

}