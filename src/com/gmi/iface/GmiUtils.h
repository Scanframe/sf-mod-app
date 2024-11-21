#pragma once

#include "AxesCoord.h"

namespace sf::gmi
{

/**
 * @brief  Calculates the time needed to move over the distance.
 * @param dist distance to travel.
 * @param vel Maximum velocity.
 * @param acc Maximum acceleration.
 * @param time Returned target time.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getTargetTime(double dist, double vel, double acc, double& time);

/**
 * @brief  Calculates the time needed to move over the distance for all axes in the AxisCoord.
 * @param dist distance to travel.
 * @param max_vel Maximum velocity.
 * @param max_acc Maximum acceleration.
 * @param trg_time Returned target time.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getTargetTime(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_time);

/**
 * @brief Calculates the time needed to move over the distance.
 * @param dist distance to travel.
 * @param vel Maximum velocity.
 * @param acc Maximum acceleration.
 * @param time Returned target time.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getAccelerationTime(double dist, double vel, double acc, double& time);

/**
 * @brief Calculates the time needed to move over the distance for all axes in the AxisCoord.
 * @param dist distance to travel.
 * @param max_vel Maximum velocity.
 * @param max_acc Maximum acceleration.
 * @param trg_time Returned acceleration time.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getAccelerationTime(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_time);

/**
 * @brief Calculates the time and acceleration needed to move over the distance.
 * @param dist distance to travel.
 * @param vel Maximum velocity.
 * @param acc Maximum acceleration.
 * @param trg_time Target time.
 * @param acc_time Acceleration time.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getTargetAccelerationTime(double dist, double vel, double acc, double& trg_time, double& acc_time);

/**
 * @brief Calculates the position and velocity at the current time from the start.
 * @param dist distance to travel.
 * @param vel Maximum velocity.
 * @param acc Maximum acceleration.
 * @param cur_tm Current time.
 * @param cur_pos Current position.
 * @param cur_vel Current velocity.
 * @return True on success to calculate.
 */
_GMI_FUNC bool getPositionVelocity(double dist, double vel, double acc, double cur_tm, double& cur_pos, double& cur_vel);

/**
 * @brief ???
 * @param dist distance to travel for each axis.
 * @param trg_time Time in which to reach the target
 * @param acc_time Time in which to accelerate.
 * @param vel Velocity needed to move the distance in time.
 * @param acc Acceleration needed to reach the previous velocity.
 * @return True on success to calculate.
 */
_GMI_FUNC bool calcLinearValue(double dist, double trg_time, double acc_time, double& vel, double& acc);

/**
 * @brief ???
 * @param dist Distance to travel for each axis.
 * @param trg_time Time in which to reach the target
 * @param acc_time Time in which to accelerate.
 * @param vel Velocity needed to move the distance in time.
 * @param acc Acceleration needed to reach the previous velocity.
 * @return True when calculations are successful.
 */
_GMI_FUNC bool calcLinearValues(const AxesCoord& dist, double trg_time, double acc_time, AxesCoord& vel, AxesCoord& acc);

/**
 * @brief Calculates the velocity and acceleration needed to move a distance in a linear manner. Returns
 * @param dist Distance to travel.
 * @param max_vel Maximum velocity.
 * @param max_acc Maximum acceleration.
 * @param trg_vel Target velocity.
 * @param trg_acc Target acceleration.
 * @param trg_time Time needed to reach target.
 * @return True when the function was able to make the calculation.
 */
_GMI_FUNC bool getLinearValues(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_vel, AxesCoord& trg_acc, double& trg_time);

/**
 * @brief Compares the passed values v1 and v2.
 * When the two values have no bigger absolute difference the maximum absolute difference passed are assumed equal.
 * The last parameter specifies if the compared value is a radial unlimited axis.
 * @param v1
 * @param v2
 * @param tolerance
 * @param rad_unlimited
 * @return If v1 < v2 the return value is <0 and for v1 > v2 it is >0.
 */
_GMI_FUNC int compareValue(double v1, double v2, double tolerance, bool rad_unlimited);

}// namespace sf::gmi