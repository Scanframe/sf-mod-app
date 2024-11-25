#pragma once

#include <gmi/iface/global.h>
#include <math/Types.h>
#include <misc/gen/TSet.h>
#include <misc/gen/Value.h>
#include <misc/gen/string.h>

namespace sf::gmi
{

/**
 * @brief Type for gathering parameter ID's.
 */
typedef unsigned long long IdType;

/**
* @brief Type for gathering parameter ID's.
*/
typedef TVector<IdType> IdList;

/**
 * @brief List of axis coordinate types.
 */
enum EAxisValueType : unsigned int
{
	/**
	 * @brief Axis not present.
	 */
	avtNONE = 0,
	/**
	 * @brief Axis positional coordinate value.
	 */
	avtPOSITION,
	/**
	 * @brief Axis velocity coordinate value.
	 */
	avtVELOCITY,
	/**
	 * @brief Axis acceleration coordinate value.
	 */
	avtACCELERATION
};

/**
 * @brief Movements an axes can make. (are used as bit locations in TAxisMovements)
 */
enum EAxisMovement : unsigned int
{
	/**
	 * @brief Units are [m, m/s, m/s2m] and axis moves lineair.
	 */
	amLINEAR = 0,
	/**
	 * @brief Units are [rad, rad/s, rad/s2] rotates and/or moves between angles.
	 */
	amRADIAL = 1,
	/**
	 * @brief Movement is limited between begin and end values.
	 */
	amLIMITED = 2
};

/**
 * @brief Bitmap set of axis movements.
 */
typedef TSet<EAxisMovement> AxisMovements;

/**
 * @brief Enumeration of axis locations.
 */
enum EAxisLocation : int
{
	/**
	 * @brief Not available axis.
	 */
	alNA = -1,
	/**
	 * @brief First axis used for iterations to alLAST_ENTRY.
	 */
	alFirst = 0,
	/**
	 * @brief Lineair and moves in [m].
	 */
	alX = 0,
	/**
	 * @brief Lineair and moves in [m].
	 */
	alY,
	/**
	 * @brief Lineair and moves in [m].
	 */
	alZ,
	/**
	 * @brief Angular Gimbal and moves [rad].
	 */
	alA,
	/**
	 * @brief Angular swivel and moves [rad].
	 */
	alB,
	/**
	 * @brief Angular Turntable and moves [rad and/or Hz].
	 */
	alC,
	/**
	 * @brief Tool Angular and moves [rad].
	 */
	alD,
	/**
	 * @brief Tool Lineair and moves in [m].
	 */
	alE,
	/**
	 * @brief Auxliary axis 1 has no location in the constellation of axes.
	 */
	alAUX1,
	/**
	 * @brief Auxiliary axis 2 has no location in the constellation of axes.
	 */
	alAUX2,
	//  alAUX3, // Auxiliary axis 3 has no location in the constellation of axes.
	//  alAUX4, // Auxiliary axis 4 has no location in the constellation of axes.
	/**
	 * @brief Last entry used in iterations.
	 */
	alLAST_ENTRY
};

/**
 * @brief Bitmap set of axis locations.
 */
typedef TSet<EAxisLocation> AxisLocations;

/**
 * Returns the name of the axis location.
 */
_GMI_FUNC const char* getAxisName(int loc);

/**
 * @brief Gets the default axis unit.
 * @param al Axis location.
 * @param type Axis type of value.
 * @return unit
 */
_GMI_FUNC const char* getAxisUnit(int al, EAxisValueType type);

/**
 * @brief Gets the axis unit based on the movements of the axis.
 * @param ams
 * @param type
 * @return Axis units based on the movements of the axis.
 */
_GMI_FUNC const char* getMovementsUnit(AxisMovements ams, EAxisValueType type);

/**
 * @brief Single position, speed or acceleration value of a specific axis.
 */
class _GMI_CLASS AxisValue
{
	public:
		/**
		 * @brief Initializing constructor.
		 * @param location Axis location as #EAxisLocation.
		 * @param value
		 */
		AxisValue(int location, double value);
		/**
		 * @brief Initializing constructor using #sf::Value as arguments.
		 * @param location
		 * @param value
		 */
		AxisValue(const Value& location, const Value& value);
		/**
		 * @brief Default constructor.
		 */
		AxisValue() = default;
		/**
		 * @brief Holds the location of the axis.
		 */
		EAxisLocation _location{alNA};
		/**
		 * @brief Holds the value of the axis.
		 */
		double _value{0.0};
};

inline AxisValue::AxisValue(int location, double value)
	: _value(value)
	, _location(static_cast<EAxisLocation>(location))
{}

inline AxisValue::AxisValue(const Value& location, const Value& value)
	: _value(value.getFloat())
	, _location(static_cast<EAxisLocation>(location.getInteger()))
{}

}// namespace sf::gmi