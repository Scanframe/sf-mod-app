#include <string>
#include "genutils.h"
#include "dbgutils.h"
#include "Range.h"

namespace sf
{

Range& Range::Set(size_type start, size_type stop, id_type id)
{
	_start = start;
	_stop = stop;
	_id = id;
	normalize();
	return *this;
}

Range& Range::Set(const RANGE& r)
{
	*(RANGE*) this = r;
	normalize();
	return *this;
}

void Range::normalize()
{
	// Check if the order is right
	if (_stop < _start)
	{ // Notify of
		_RTTI_NOTIFY(DO_DEFAULT, *this << " Normalized");
		// Swap start and stop.
		swap_it(_start, _stop);
	}
}

int Range::Compare(const Range& r) const
{
	size_type Start2 = r._start;
	size_type Stop2 = r._stop;
	// going for 0
	if ((_stop == Stop2) && (_start == Start2))
	{
		return 0;
	}
	// going for 5 is this empty
	if (_stop == _start)
	{
		return 5;
	}
	// going for -5 is other empty
	if (Stop2 == Start2)
	{
		return -5;
	}
	// going for -3 or -4
	if (_stop <= Start2)
	{
		return ((_stop == r._start) ? -3 : -4);
	}
	// going for 3 or 4
	if (Stop2 <= _start)
	{
		return ((Stop2 == _start) ? 3 : 4);
	}
	// go for -1, 1
	if (_start == Start2)
	{
		return ((_stop < Stop2) ? -1 : 1);
	}
	// go for -1, 1
	if (_stop == Stop2)
	{
		return ((_start < Start2) ? 1 : -1);
	}
	// going for -1, 2
	if (_start > Start2)
	{
		return ((_stop <= Stop2) ? -1 : 2);
	}
	// going for 1, -2
	if (Start2 > _start)
	{
		return ((_stop >= Stop2) ? 1 : -2);
	}
	_RTTI_NOTIFY(DO_DEFAULT, *this << "Compare" << r << " Failure!");
	return INT_MAX;
}

Range Range::operator&(const Range& r) const
{
	Range rng;
	// Get the largest start value.
	rng._start = (r._start > _start) ? r._start : _start;
	// Get the smallest stop value.
	rng._stop = (r._stop < _stop) ? r._stop : _stop;
	// Set the id to this id
	rng._id = _id;
	// Check for an empty range when Start is smaller or equal then Stop.
	if (rng._start >= rng._stop)
	{
		// Clear the range because it should be empty.
		rng.Clear();
	}
	return rng;
}

Range Range::operator-(const Range& r) const
{
	switch (Compare(r))
	{
		case 0:
		case -1:
		default:
			return Range();

		case 1:
			// Check if the the range limits are the same because
			// that will result into a single result instead of two
			if (_start == r._start)
			{
				return Range(r._stop + 1, _stop, _id);
			}
			if (_stop == r._stop)
			{
				return Range(_start, r._start - 1, _id);
			}
			// Notify
			_RTTI_NOTIFY(DO_DEFAULT, *this << "operator-" << r << " Failed. Should Split In Two.");
			return *this;

		case 2:
			return Range(_start, r._stop, _id);

		case -2:
			return Range(r._start, _stop, _id);

		case -3:
		case 3:
		case -4:
		case 4:
		case -5:
		case 5:
			return *this;

	}
}

Range Range::operator+(const Range& r) const
{
	switch (Compare(r))
	{
		case 0:
		case -1:
		case 5:
			return r;

		case 1:
		case -5:
			return *this;

		case 4:
			//RTTI_NOTIFY(DO_DEFAULT, " Improper use 'operator +()' on " << *this << "+" << r);
		case 2:
		case 3:
			return Range(r._start, _stop, _id);

		case -4:
			//RTTI_NOTIFY(DO_DEFAULT , " Improper use 'operator +()' on " << *this << "+" << r);
		case -2:
		case -3:
			return Range(_start, r._stop, _id);

		default:
			return Range();
	}
}

/*
int Range::ExclusiveOr(const Range& r, Range& d1, Range& d2) const
{

  switch (Compare(r))
  {
    case 1:
      d1.Set(Start, r.Start);
      d2.Set(r.Stop, Stop);
      return 2;

    case -1:
      d1.Set(r.Start, Start);
      d2.Set(Stop, r.Stop);
      return 2;

    case 2:
      d1.Set(r.Start, Start);
      d2.Set(r.Stop, Stop);
      return 2;

    case -2:
      d1.Set(Start, r.Start);
      d2.Set(Stop, r.Stop);
      return 2;

    case 3:
    case 4:
      d1 = r;
      d2 = *this;
      return 2;

    case -3:
    case -4:
      d1 = *this;
      d2 = r;
      return 2;

    case 5:
      d1 = r;
      return 1;

    case -5:
      d1 = *this;
      return 1;

    case 0:
    default:
      d1.Clear();
      d2.Clear();
      return 0;
  }
}
*/

}
