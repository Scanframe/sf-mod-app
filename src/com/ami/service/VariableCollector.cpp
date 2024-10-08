#include "VariableCollector.h"
#include <misc/gen/Thread.h>
#include <misc/gen/dbgutils.h>

namespace sf
{

VariableCollector::VariableCollector()
{
	_variable.setHandler(this);
}

VariableCollector::~VariableCollector()
{
	qDebug() << SF_Q_RTTI_TYPENAME << Thread::getCurrentId();
}

void VariableCollector::addVariable(const Variable& variable)
{
	// Only add the variable when the export flag has been set.
	if (variable.isFlag(flgExport))
	{
		// Create a non owner instance.
		auto* v = new Variable(variable.getId(), false);
		// Set the data member to the id.
		v->setData<id_type>(variable.getId());
		// Add the variable to the list.
		_map[variable.getId()] = v;
		// Assign the handler method of this class.
		v->setHandler(this);
	}
}

void VariableCollector::variableEventHandler(VariableTypes::EEvent event, const Variable& caller, Variable& link, bool same_inst)
{
	switch (event)
	{
		default:
			break;

		// Global event for a new ID.
		case veNewId: {
			addVariable(caller);
			break;
		}

		case veIdChanged:
		case veRemove: {
			// Remove the variable using the id in the data member with which it was created.
			_map.erase(link.getData<id_type>());
			break;
		}

		case veLinked: {
			// When linking iterate through the list of global variables.
			if (&link == &_variable)
			{
				// Lock for member.
				Lock lock(this);
				for (auto* v: Variable::getList())
				{
					addVariable(*v);
					Data d;
					d._id = v->getId();
					d._type = Data::dtAdd;
					d._value.set(v->getSetupString());
					_fifo.push(d);
					d._type = Data::dtValue;
					d._value = v->getCur(false);
					_fifo.push(d);
					d._type = Data::dtFlags;
					d._value.set(v->getCurFlags());
					_fifo.push(d);
				}
			}
			break;
		}

		case veValueChange: {
			// Lock for member.
			Lock lock(this);
			Data d;
			d._id = caller.getId();
			d._type = Data::dtValue;
			d._value.set(caller.getCur(false));
			_fifo.push(d);
			break;
		}

		case veFlagsChange: {
			// Lock for member.
			Lock lock(this);
			Data d;
			d._id = caller.getId();
			d._type = Data::dtFlags;
			d._value.set(caller.getCurFlags());
			_fifo.push(d);
			break;
		}
	}
}

}// namespace sf
