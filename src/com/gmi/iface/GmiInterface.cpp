#include "GmiInterface.h"

namespace sf::gmi
{

ParamState::ParamState(const ParamState& ps)
	: _name(ps._name)
	, _value(ps._value)
{}

ParamState::ParamState(const std::string& name, Value value)
	: _name(name)
	, _value(value)
{}

bool ParamState::operator==(const ParamState& ps) const
{
	return _name == ps._name;
}

}// namespace sf::gmi
