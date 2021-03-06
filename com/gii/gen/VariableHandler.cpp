#include "VariableHandler.h"
#include "Variable.h"

namespace sf
{

VariableHandler::~VariableHandler()
{
	Variable::removeHandler(this);
}

}