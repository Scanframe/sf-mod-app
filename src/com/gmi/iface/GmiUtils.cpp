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
	if ((acc * dist) > (vel * vel))
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
	if ((acc * dist) > (vel * vel))
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
	bool negative = dist < 0;
	// Make all values positive.
	dist = std::fabs(dist);
	vel = std::fabs(vel);
	acc = std::fabs(acc);
	cur_tm = std::fabs(cur_tm);
	double trg_time = 0.0;
	double acc_time = 0.0;
	// Get acceleration and target time.
	if (getTargetAccelerationTime(dist, vel, acc, trg_time, acc_time))
	{
		// Check if the current time is beyond the target time.
		if (cur_tm > trg_time)
		{
			SF_NORM_NOTIFY(DO_DEFAULT, "Passed movement: " << cur_tm);
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
				if (cur_tm < (trg_time - acc_time))
				{
					// Constant time here so no calulations for the velocity.
					cur_vel = vel;
					// Calculate the distance for acceleration.
					cur_pos = acc * acc_time * acc_time * 0.5;
					// Add the distance for the time the velocity is constant.
					cur_pos += (cur_tm - acc_time) * vel;
					// Correct sign of return value.
					if (negative)
						cur_pos *= -1.0;
					//NOTIFY(DO_DEFAULT, "Const: " << curtm << "  Pos: " << curpos << "  Vel: " << curvel);
				}
				else
				{// Current time is in the deceleration area.
					double tm = (trg_time - cur_tm);
					cur_vel = acc * tm;
					cur_pos = dist - acc * tm * tm * 0.5;
					// Correct sign of return value.
					if (negative)
					{
						cur_pos *= -1.0;
					}
					//NOTIFY(DO_DEFAULT, "Dec: " << curtm << "  Pos: " << curpos << "  Vel: " << curvel);
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
				//NOTIFY(DO_DEFAULT, "Acc: " << curtm << "  Pos: " << curpos << "  Vel: " << curvel);
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
	if ((acc * dist) > (vel * vel))
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

bool getTargetTime(const TAxesCoord& dist, const TAxesCoord& max_vel, const TAxesCoord& max_acc, TAxesCoord& trg_time)
{
	// Clear return values first.
	trg_time.Clear();
	// Get the bit map of set position values of the coordinate.
	int map = dist.GetMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.GetMap() & map) != map || (max_acc.GetMap() & map) != map)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "gmi::getTargetTime(): Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Iterate through the bitmap of set values.
	for (unsigned int i = alFirst; i < alLAST_ENTRY; i++)
	{
		// Calculate the time which is needed for the current selected axis to
		// move the distance.
		if (map & (1 << i))
		{
			double tm;
			if (getTargetTime(dist[i].value, max_vel[i].value, max_acc[i].value, tm))
				trg_time.Set(i, tm);
			else
			{
				trg_time.Clear();
				return false;
			}
		}
	}
	return true;
}

bool getAccelerationTime(const TAxesCoord& dist, const TAxesCoord& max_vel, const TAxesCoord& max_acc, TAxesCoord& trg_time)
{
	// Clear return values first.
	trg_time.Clear();
	// Get the bit map of set position values of the coordinate.
	int map = dist.GetMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.GetMap() & map) != map || (max_acc.GetMap() & map) != map)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "gmi::getAccelerationTime(): Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Iterate through the bitmap of set values.
	for (unsigned int i = alFirst; i < alLAST_ENTRY; i++)
	{
		// Calculate the time which is needed for the current selected axis to accelerate.
		if (map & (1 << i))
		{
			double tm;
			if (getAccelerationTime(dist[i].value, max_vel[i].value, max_acc[i].value, tm))
				trg_time.Set(i, tm);
			else
			{
				trg_time.Clear();
				return false;
			}
		}
	}
	return true;
}

bool getLinearValues(const TAxesCoord& dist, const TAxesCoord& max_vel, const TAxesCoord& max_acc, TAxesCoord& trg_vel, TAxesCoord& trg_acc, double& trg_time)
{
	// Clear return values first.
	trg_vel.Clear();
	trg_acc.Clear();
	// Get the bit map of set position values of the coordinate.
	int map = dist.GetMap();
	// Check if the velocity and acceleration values are present needed for
	// the calculation.
	if ((max_vel.GetMap() & map) != map || (max_acc.GetMap() & map) != map)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "gmi::getLinearValues(): Improper amount of information!");
		// Signal failure.
		return false;
	}
	// Initialize the times.
	double trgtm = 0.0;
	double acctm = 0.0;
	int axis = -1;
	// Iterate through the bitmap of set values.
	for (int i = alFirst; i < alLAST_ENTRY; i++)
	{
		if (map & (1 << i))
		{
			double ttm;
			double atm;
			// Calculate the time which is needed for the current selected axis to accelerate.
			if (!getTargetAccelerationTime(dist[i].value, max_vel[i].value, max_acc[i].value, ttm, atm))
				return false;
			// When the calculated time is larger than last largest time update the value and axis.
			if (trgtm < ttm || axis == -1)
			{
				trgtm = ttm;
				acctm = atm;
				axis = i;
			}
		}
	}
	// Check if there is anything that needs to be calculated.
	if (axis != -1)
	{
		SF_NORM_NOTIFY(DO_DEFAULT, "getLinearValues() Dominant axis = " << GetAxisName(axis) << "-Axis");
		calcLinearValues(dist, trgtm, acctm, trg_vel, trg_acc);
		trg_time = trgtm;
	}
	return true;
}

bool calcLinearValues(const TAxesCoord& dist, double trg_time, double acc_time, TAxesCoord& vel, TAxesCoord& acc)
{
	// Iterate through the bitmap of set values.
	for (unsigned int i = alFirst; i < alLAST_ENTRY; i++)
	{// Check for a value in the coord which is set.
		if (dist.IsSet(i))
		{
			// Calculate the time which is needed for the current selected axis to
			// move the distance.
			double _vel;
			double _acc;
			if (!calcLinearValue(dist[i].value, trg_time, acc_time, _vel, _acc))
				return false;
			// Set the coord velocity return value.
			vel.Set(i, _vel);
			// Set the coord acceleration return value.
			acc.Set(i, _acc);
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

}