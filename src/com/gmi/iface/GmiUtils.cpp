#include "GmiUtils.h"

namespace sf::gmi
{

bool getTargetTime(double dist, double vel, double acc, double& time)
{
	// Make the input parameters absolute.
	dist = std::fabs(dist);
	vel = std::fabs(vel);
	acc = std::fabs(acc);
	if (isZero(acc))
	{
		return false;
	}
	if (acc * dist > vel * vel)
	{
		time = dist / vel + vel / acc;
	}
	else
	{
		time = std::sqrt(dist / acc) * 2.0;
	}
	return true;
}

bool getAccelerationTime(double dist, double vel, double acc, double& time)
{
	dist = std::fabs(dist);
	vel = std::fabs(vel);
	acc = std::fabs(acc);
	if (isZero(acc))
	{
		return false;
	}
	// Make a decision on how to calculate.
	if (acc * dist > vel * vel)
	{
		time = vel / acc;
	}
	else
	{
		time = std::sqrt(dist / acc);
	}
	return true;
}

bool getPositionVelocity(double dist, double vel, double acc, double cur_tm, double& cur_pos, double& cur_vel)
{
	// Check if the distance is negative to be able to correct the return value.
	const bool negative = dist < 0;
	// Make all values positive.
	dist = std::fabs(dist);
	vel = std::fabs(vel);
	acc = std::fabs(acc);
	cur_tm = std::fabs(cur_tm);
	double acc_time = 0.0;
	double trg_time = 0.0;
	// Get acceleration and target time.
	if (getTargetAccelerationTime(dist, vel, acc, trg_time, acc_time))
	{
		// Check if the current time is beyond the target time.
		if (cur_tm > trg_time)
		{
			SF_FUNC_NOTIFY(DO_DEFAULT, "Passed movement: " << cur_tm);
			// Position is the target position
			cur_pos = dist;
			// There is no movement so the velocity is zero.
			cur_vel = 0.0;
		}
		else
		{
			// Check if the current time is past acceleration area.
			if (cur_tm > acc_time)
			{// Check if the current time is before the deceleration area.
				if (cur_tm < trg_time - acc_time)
				{
					// Constant time here so no calculations for the velocity.
					cur_vel = vel;
					// Calculate the distance for acceleration.
					cur_pos = acc * acc_time * acc_time * 0.5;
					// Add the distance for the time the velocity is constant.
					cur_pos += (cur_tm - acc_time) * vel;
					// Correct sign of return value.
					if (negative)
						cur_pos *= -1.0;
					//NOTIFY(DO_DEFAULT, "Const: " << cur_tm << "  Pos: " << cur_pos << "  Vel: " << cur_vel);
				}
				else
				{
					// Current time is in the deceleration area.
					const double tm = trg_time - cur_tm;
					cur_vel = acc * tm;
					cur_pos = dist - acc * tm * tm * 0.5;
					// Correct sign of return value.
					if (negative)
					{
						cur_pos *= -1.0;
					}
					//NOTIFY(DO_DEFAULT, "Dec: " << cur_tm << "  Pos: " << cur_pos << "  Vel: " << cur_vel);
				}
			}
			else
			{
				// Current time is in the acceleration area.
				cur_vel = acc * cur_tm;
				cur_pos = acc * cur_tm * cur_tm * 0.5;
				// Correct sign of return value.
				if (negative)
				{
					cur_pos *= -1.0;
				}
				//NOTIFY(DO_DEFAULT, "Acc: " << cur_tm << "  Pos: " << cur_pos << "  Vel: " << cur_vel);
			}
		}
	}
	else
	{
		cur_pos = 0.0;
		cur_vel = 0.0;
		// Signal failure.
		return false;
	}
	// Signal success.
	return true;
}

bool getTargetAccelerationTime(double dist, double vel, double acc, double& trg_time, double& acc_time)
{
	dist = std::fabs(dist);
	vel = std::fabs(vel);
	acc = std::fabs(acc);
	// Make a decision on how to calculate.
	if (acc * dist > vel * vel)
	{
		if (isZero(vel + acc_time))
		{
			trg_time = acc_time = 0;
			return false;
		}
		acc_time = vel / acc;
		trg_time = dist / vel + acc_time;
	}
	else
	{
		if (isZero(acc))
		{
			trg_time = acc_time = 0;
			return false;
		}
		acc_time = std::sqrt(dist / acc);
		trg_time = acc_time * 2.0;
	}
	return true;
}

bool getTargetTime(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_time)
{
	// Clear return values first.
	trg_time.clear();
	// Get the bit map of set position values of the coordinate.
	const int map = dist.getMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.getMap() & map) != map || (max_acc.getMap() & map) != map)
	{
		SF_FUNC_NOTIFY(DO_DEFAULT, "Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Iterate through the bitmap of set values.
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{
		// Calculate the time which is needed for the current selected axis to
		// move the distance.
		if (map & 1 << i)
		{
			double tm;
			if (getTargetTime(dist[i]._value, max_vel[i]._value, max_acc[i]._value, tm))
				trg_time.set(i, tm);
			else
			{
				trg_time.clear();
				return false;
			}
		}
	}
	return true;
}

bool getAccelerationTime(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_time)
{
	// Clear return values first.
	trg_time.clear();
	// Get the bit map of set position values of the coordinate.
	const int map = dist.getMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.getMap() & map) != map || (max_acc.getMap() & map) != map)
	{
		SF_FUNC_NOTIFY(DO_DEFAULT, "Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Iterate through the bitmap of set values.
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{
		// Calculate the time which is needed for the current selected axis to accelerate.
		if (map & 1 << i)
		{
			double tm;
			if (getAccelerationTime(dist[i]._value, max_vel[i]._value, max_acc[i]._value, tm))
				trg_time.set(i, tm);
			else
			{
				trg_time.clear();
				return false;
			}
		}
	}
	return true;
}

bool getLinearValues(const AxesCoord& dist, const AxesCoord& max_vel, const AxesCoord& max_acc, AxesCoord& trg_vel, AxesCoord& trg_acc, double& trg_time)
{
	// Clear return values first.
	trg_vel.clear();
	trg_acc.clear();
	// Get the bit map of set position values of the coordinate.
	const int map = dist.getMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.getMap() & map) != map || (max_acc.getMap() & map) != map)
	{
		SF_FUNC_NOTIFY(DO_DEFAULT, "Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Initialize the times.
	double trg_tm = 0.0;
	double acc_tm = 0.0;
	int axis = -1;
	// Iterate through the bitmap of set values.
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{
		if (map & 1 << i)
		{
			double ttm;
			double atm;
			// Calculate the time which is needed for the current selected axis to accelerate.
			if (!getTargetAccelerationTime(dist[i]._value, max_vel[i]._value, max_acc[i]._value, ttm, atm))
				return false;
			// When the calculated time is larger than last largest time update the value and axis.
			if (trg_tm < ttm || axis == -1)
			{
				trg_tm = ttm;
				acc_tm = atm;
				axis = i;
			}
		}
	}
	// Check if there is anything requiring calculation.
	if (axis != -1)
	{
		SF_FUNC_NOTIFY(DO_DEFAULT, "Dominant axis = " << getAxisName(axis) << "-Axis");
		calcLinearValues(dist, trg_tm, acc_tm, trg_vel, trg_acc);
		trg_time = trg_tm;
	}
	return true;
}

bool calcLinearValues(const AxesCoord& dist, double trg_time, double acc_time, AxesCoord& vel, AxesCoord& acc)
{
	// Iterate through the bitmap of set values.
	for (int i = alFIRST_ENTRY; i < alLAST_ENTRY; i++)
	{// Check for a value in the coord which is set.
		if (dist.isSet(i))
		{
			// Calculate the time which is needed for the current selected axis to
			// move the distance.
			double d_vel;
			double d_acc;
			if (!calcLinearValue(dist[i]._value, trg_time, acc_time, d_vel, d_acc))
				return false;
			// Set the coord velocity return value.
			vel.set(i, d_vel);
			// Set the coord acceleration return value.
			acc.set(i, d_acc);
		}
	}
	return true;
}

bool calcLinearValue(double dist, double trg_time, double acc_time, double& vel, double& acc)
{
	vel = acc = 0.0;
	// Avoid division by zero.
	if (isZero(acc_time) && isZero(trg_time - acc_time))
	{
		return false;
	}
	// Always return positive values.
	vel = fabs(dist / (trg_time - acc_time));
	acc = fabs(vel / acc_time);
	return true;
}

int compareValue(double v1, double v2, double tolerance, bool rad_unlimited)
{
	// Get absolute difference between the two values.
	double diff = v1 - v2;
	diff = std::fabs(diff);
	// Normalize when having radial unlimited axes.
	if (rad_unlimited)
	{
		diff = std::fmod(diff, numbers::pi_v<decltype(v1)> * 2.0);
		diff = std::min(std::abs(diff - numbers::pi_v<decltype(v1)> * 2.0), diff);
	}
	// Is the difference within the accuracy then they are assumed equal.
	if (diff < std::fabs(tolerance))
	{
		return 0;
	}
	// For the other possibilities regular comparison.
	if (v1 < v2)
	{
		return -1;
	}
	return 1;
}

}// namespace sf::gmi