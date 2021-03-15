#include <sys/time.h> // NOLINT(modernize-deprecated-headers)
#include "TimeSpec.h"
#include "gen/gen_utils.h"

namespace sf
{

TimeSpec& TimeSpec::setTimeOfDay()
{
	timeval tv{};
	::gettimeofday(&tv, nullptr);
	// Convert from timeval to timespec
	tv_sec = tv.tv_sec;
	tv_nsec = tv.tv_usec * 1000l;
	return *this;
}

TimeSpec& TimeSpec::assign(double sec)
{
	tv_sec = sec;
	tv_nsec = (sec - tv_sec) * 1000000000l;
	return *this;
}

TimeSpec& TimeSpec::randomize(double factor)
{
	// Randomize a float between 0.0 and 1.0
	double value = (double) rand() / (double) RAND_MAX; // NOLINT(cert-msc50-cpp)
	// Modify so the number is between -0.5 and +0.5
	value -= 0.5;
	// Modify so the number is between -1.0 and +1.0
	value *= 2.0;
	// Reduce the random value.
	value *= factor;
	// Reduce the value using the factor.
	value += 1.0;
	// Calculate the value to set.
	value *= toDouble();
	// Assign the new value.
	return assign(value);
}

TimeSpec& TimeSpec::add(long int sec, long int nsec)
{
	tv_nsec += nsec;
	if (tv_nsec >= 1000000000l)
	{
		tv_nsec -= 1000000000l;
		tv_sec++;
	}
	else if (tv_nsec < 0)
	{
		tv_nsec += 1000000000l;
		tv_sec--;
	}
	// For all situations.
	tv_sec += sec;
	//
	return *this;
}

TimeSpec& TimeSpec::assign(long int sec, long int nsec)
{
	tv_sec = sec + nsec / 1000000000l;
	tv_nsec = nsec % 1000000000l;
	return *this;
}

double TimeSpec::toDouble() const
{
	return double(tv_sec) + double(tv_nsec) / 1000000000l;
}

std::string TimeSpec::toString() const
{
	return stringf("%lgs", toDouble());
}

}
