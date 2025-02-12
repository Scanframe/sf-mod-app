#include "GmiTypes.h"

namespace sf::gmi
{

static struct
{
		const char* Name;
		const char* Pos;
		const char* Vel;
		const char* Acc;
} AxisInformation[] = {{"N/A", "N/A", "N/A", "N/A"},       {"X", "m", "m/s", "m/s\xB2"},       {"Y", "m", "m/s", "m/s\xB2"},
											 {"Z", "m", "m/s", "m/s\xB2"},       {"A", "rad", "rad/s", "rad/s\xB2"}, {"B", "rad", "rad/s", "rad/s\xB2"},
											 {"C", "rad", "rad/s", "rad/s\xB2"}, {"D", "rad", "rad/s", "rad/s\xB2"}, {"E", "m", "m/s", "m/s\xB2"},
											 {"Aux1", "m", "m/s", "m/s\xB2"},    {"Aux2", "m", "m/s", "m/s\xB2"},    {"Aux3", "m", "m/s", "m/s\xB2"},
											 {"Aux4", "m", "m/s", "m/s\xB2"}};

_GMI_FUNC const char* getAxisName(int axis_loc)
{
	// Correct the index by 1.
	size_t index = axis_loc + 1;
	if (index >= std::size(AxisInformation))
	{
		throw std::range_error(std::string(__FUNCTION__) + "() invalid axis location!");
	}
	return AxisInformation[index].Name;
}

_GMI_FUNC const char* getMovementsUnit(AxisMovements ams, EAxisValueType type)
{
	// Differ between to known axes.
	return getAxisUnit(ams.has(amRADIAL) ? alC : alX, type);
}

_GMI_FUNC const char* getAxisUnit(int axis_loc, EAxisValueType type)
{
	// Correct the index by 1.
	size_t index = axis_loc + 1;
	if (index >= std::size(AxisInformation))
	{
		throw std::range_error(std::string(__FUNCTION__) + "() invalid axis location!");
	}
	// Correct the accessing index by 1.
	switch (type)
	{
		case avtPOSITION:
			return AxisInformation[index].Pos;
		case avtVELOCITY:
			return AxisInformation[index].Vel;
		case avtACCELERATION:
			return AxisInformation[index].Acc;
		default:
			return AxisInformation[0].Pos;
	}
}

}// namespace sf::gmi
