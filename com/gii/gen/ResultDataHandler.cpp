#include "ResultDataHandler.h"
#include "ResultData.h"

namespace sf
{

ResultDataHandler::~ResultDataHandler()
{
	ResultData::removeHandler(this);
}

}

