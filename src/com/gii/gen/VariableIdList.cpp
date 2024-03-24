#include "VariableIdList.h"

namespace sf
{

void VariableIdList::setIds(const std::string& ids)
{
	bool change = false;
	strings sl;
	sl.split(ids, ',');
	id_type id;
	if (_ptrList)
	{
		for (auto i = _ptrList->begin(); i != _ptrList->end(); i++)
		{
			auto index = std::distance(_ptrList->begin(), i);
			id = index < sl.size() ? std::stoull(sl.at(index)) : 0;
			if ((*i)->getDesiredId() != id)
			{
				(*i)->setup(id, true);
				// A change in id's was made so set the flag.
				change |= true;
			}
		}
	}
	if (_list)
	{
		for (auto i = _list->begin(); i != _list->end(); i++)
		{
			auto index = std::distance(_list->begin(), i);
			id = index < sl.size() ? std::stoull(sl.at(index)) : 0;
			if ((*i).getDesiredId() != id)
			{
				const_cast<Variable&>(*i).setup(id, true);
				// A change in id's was made so set the flag.
				change |= true;
			}
		}
	}
	// When a change was detected emit this event.
	if (change && onChange)
	{
		onChange(this);
	}
}

std::string VariableIdList::getIds() const
{
	strings rv;
	if (_ptrList)
	{
		for (auto i: *_ptrList)
		{
			rv.append("0x" + itostr(i->getDesiredId(), 16));
		}
	}
	if (_list)
	{
		for (auto i: *_list)
		{
			rv.append("0x" + itostr(i.getDesiredId(), 16));
		}
	}
	return rv.join(",");
}

}
