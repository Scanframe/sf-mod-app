#pragma once

#include "global.h"
#include "math/Types.h"
#include <misc/gen/TSet.h>
#include <misc/gen/Value.h>
#include <misc/gen/string.h>

namespace sf::gmi
{

/*
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
_GMI_FUNC const char* GetAxisName(int loc);

/**
 * @brief Gets the default axis unit.
 * @param al Axis location.
 * @param type Axis type of value.
 * @return unit
 */
_GMI_FUNC const char* GetAxisUnit(int al, EAxisValueType type);

/**
 * @brief Gets the axis units based on the movements of the axis.
 * @param ams
 * @param type
 * @return Axis units based on the movements of the axis.
 */
_GMI_FUNC const char* GetMovementsUnit(AxisMovements ams, EAxisValueType type);

/**
 * @brief Single position, speed or acceleration value of a specific axis.
 */
class _GMI_CLASS TAxisValue
{
	public:
		/**
		 * @brief Initializing constructor.
		 * @param location Axis location as #EAxisLocation.
		 * @param value
		 */
		TAxisValue(int location, double value);
		/**
		 * @brief Initializing constructor using #sf::Value as arguments.
		 * @param location
		 * @param value
		 */
		TAxisValue(const Value& location, const Value& value);
		/**
		 * @brief Default constructor.
		 */
		TAxisValue() = default;
		/**
		 * @brief Holds the location of the axis.
		 */
		EAxisLocation location{alNA};
		/**
		 * @brief Holds the value of the axis.
		 */
		double value{0.0};
};

inline TAxisValue::TAxisValue(int location, double value)
	: value(value)
	, location(static_cast<EAxisLocation>(location))
{}

inline TAxisValue::TAxisValue(const Value& location, const Value& value)
	: value(value.getFloat())
	, location(static_cast<EAxisLocation>(location.getInteger()))
{}

}// namespace sf::gmi