#include "Range.h"
#include "gen_utils.h"
#include <algorithm>
#include <string>

namespace sf
{

bool RangeCompareExact = false;

Range& Range::assign(size_type start, size_type stop, id_type id)
{
	_start = start;
	_stop = stop;
	_id = id;
	normalize();
	return *this;
}

Range& Range::set(const RANGE& r)
{
	*(RANGE*) this = r;
	normalize();
	return *this;
}

void Range::normalize()
{
	// Check if the order is right
	if (_stop < _start)
	{
		// Swap start and stop.
		swap_it(_start, _stop);
	}
}

Range::ECompare Range::compare(const Range& r) const
{
	auto start2 = r._start;
	auto stop2 = r._stop;
	// going for 0
	if ((_stop == stop2) && (_start == start2))
	{
		return cmpSame;
	}
	// going for 5 is this empty
	if (_stop == _start)
	{
		return cmpSelfEmpty;
	}
	// going for -5 is other empty
	if (stop2 == start2)
	{
		return cmpOtherEmpty;
	}
	// going for -3 or -4
	if (_stop <= start2)
	{
		return ((_stop == r._start) ? cmpExtendsSelf : cmpAfterSelf);
	}
	// going for 3 or 4
	if (stop2 <= _start)
	{
		return ((stop2 == _start) ? cmpExtendsOther : cmpBeforeSelf);
	}
	// go for -1, 1
	if (_start == start2)
	{
		return ((_stop < stop2) ? cmpWithinOther : cmpWithinSelf);
	}
	// go for -1, 1
	if (_stop == stop2)
	{
		return ((_start < start2) ? cmpWithinSelf : cmpWithinOther);
	}
	// going for -1, 2
	if (_start > start2)
	{
		return ((_stop <= stop2) ? cmpWithinOther : cmpOverlapsOther);
	}
	// going for 1, -2
	if (start2 > _start)
	{
		return ((_stop >= stop2) ? cmpWithinSelf : cmpOverlapsSelf);
	}
	return cmpError;
}

bool Range::isMergeable(const Range& r) const
{
	auto cmp = compare(r);
	return cmp != cmpBeforeSelf && cmp != cmpAfterSelf;
}

bool Range::isOverlapped(const Range& r) const
{
	auto cmp = compare(r);
	return cmp == cmpOverlapsOther || cmp == cmpOverlapsSelf;
}

bool Range::isExtension(const Range& r) const
{
	auto cmp = compare(r);
	return cmp == cmpExtendsOther || cmp == cmpExtendsSelf;
}

bool Range::isWithinSelf(const Range& r) const
{
	auto cmp = compare(r);
	return (cmp == cmpWithinSelf || cmp == cmpSame);
}

bool Range::isWithinOther(const Range& r) const
{
	auto cmp = compare(r);
	return (cmp == cmpWithinOther || cmp == cmpSame);
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
		rng.clear();
	}
	return rng;
}

Range Range::operator+(const Range& r) const
{
	switch (compare(r))
	{
		case cmpSame:
		case cmpWithinOther:
		case cmpSelfEmpty:
			return r;

		case cmpWithinSelf:
		case cmpOtherEmpty:
			return *this;

		case cmpBeforeSelf:
		case cmpOverlapsOther:
		case cmpExtendsOther:
			return Range(r._start, _stop, _id);

		case cmpAfterSelf:
		case cmpOverlapsSelf:
		case cmpExtendsSelf:
			return Range(_start, r._stop, _id);

		default:
			return Range();
	}
}

int Range::exclude(const Range& r, Range& rest, Range::ECompare* cmp)
{
	auto cr = compare(r);
	if (cmp)
	{
		*cmp = cr;
	}
	switch (cr)
	{
		case cmpSame:
		case cmpWithinOther:
			clear();
			return 0;

		case cmpOverlapsSelf:
			assign(_start, r._start, _id);
			rest.clear();
			return 1;

		case cmpOverlapsOther:
			assign(r._stop, _stop, _id);
			rest.clear();
			return 1;

		case cmpWithinSelf:
			// Prevent to set empty range.
			if (r._start == _start)
			{
				assign(r._stop, _stop, _id);
				rest.clear();
				return 1;
			}
			if (r._stop == _stop)
			{
				assign(_start, r._start, _id);
				rest.clear();
				return 1;
			}
			// Rest value first because it assigning this first wil influence the result.
			rest.assign(r._stop, _stop, _id);
			assign(_start, r._start, _id);
			return 2;

		default:
			// No assignment just nothing changes for this range.
			return 1;
	}
}

Range::Vector Range::Vector::exclude(const Range::Vector& rel) const
{
	// Return value of the function.
	Vector rv;
	Range rm, rest;
	ECompare cmp;
	// Initial start of the exclusion iterator.
	auto it = rel.begin();
	//
	for (auto const& ri: *this)
	{
		// Assign a copy to modify.
		rm.assign(ri);
		//
		while (it != rel.end())
		{
			// Only when 2 ranges result of this exclusion add the first to the list and continue with
			auto result = rm.exclude(*it, rest, &cmp);
			// Speedup things by skipping the rest because the vector is ordered and nothing is expected after this.
			if (cmp == cmpAfterSelf || cmp == cmpExtendsSelf)
			{
				// Just continue the with next range in the outer loop.
				// Do not increment the it iterator. It is the starting point for the next loop.
				//_RTTI_NOTIFY(DO_CLOG, "Skipping on " << rm << " on " << *it);
				break;
			}
			if (result > 1)
			{
				rv.insert(rv.end(), rm);
				rm.assign(rest);
			}
			it++;
		}
		// When not empty add the last probed range.
		if (!rm.isEmpty())
		{
			rv.insert(rv.end(), rm);
		}
	}
	// This vector does not need explicit sorting because it already is.
	return rv;
}

Range::Vector& Range::Vector::exclude(const Range::Vector& rl_ex)
{
	return *this = ((const Range::Vector*) this)->exclude(rl_ex);
}

Range::Vector Range::Vector::extract(const Range::Vector& rl, Range::Vector& rl_ex) const
{
	// Keeps of covered indices.
	TVector<Vector::size_type> il;
	// Covering vector.
	auto itc = rl.begin();
	// Compare value.
	ECompare cmp;
	// Iterate through the sorted vector out of which elements are to be extracted.
	Vector::size_type i = 0;
	while (i < size() && itc != rl.end())
	{
		// Compare and check for coverage.
		cmp = at(i).compare(*itc);
		//__RTTI_NOTIFY(DO_CLOG, __FUNCTION__ << " [" << i << ',' << std::distance(rl.begin(), itc)
		//	<< "," << std::setw(2) << cmp << "] " << at(i) << " <=> " << (*itc))
		// When the current range is covered.
		if (cmp == cmpSame || cmp == cmpWithinOther)
		{
			//_RTTI_NOTIFY(DO_CLOG, __FUNCTION__ << " Adding: " << i)
			// Add the index to the index vector.
			il.add(i);
			// Move to next range for comparing.
			i++;
		}
		// When the range does not cover and the next one will not either.
		else if (cmp == cmpOverlapsSelf || cmp == cmpExtendsSelf || cmp == cmpAfterSelf || cmp == cmpOtherEmpty)
		{
			// Move to next range for comparing.
			i++;
		}
		else
		{
			// Next cover range.
			itc++;
		}
	}
	//_RTTI_NOTIFY(DO_CLOG, __FUNCTION__ << " Original: " << *this)
	// Fill the passed vector with the extracted entries.
	for (auto k: il)
	{
		rl_ex.add(at(k));
	}
	// Copy the vector to modify.
	Vector rv = *this;
	// Second time iteration to remove the entries from the vector in reverse order to not mess up the index.
	for (auto k = il.rbegin(); k != il.rend(); k++)
	{
		rv.detachAt(*k);
	}
	//_RTTI_NOTIFY(DO_CLOG, __FUNCTION__ << " Extracted: " << rl_ex)
	//_RTTI_NOTIFY(DO_CLOG, __FUNCTION__ << " Left over: " << rv)
	return rv;
}

Range::Vector& Range::Vector::sort()
{
	// Sort the vector using the operator '<'.
	std::sort(begin(), end());
	return *this;
}

Range::Vector Range::Vector::sort() const
{
	auto cl = *this;
	// Sort the vector using the operator '<'.
	std::sort(cl.begin(), cl.end());
	return cl;
}

Range::Vector& Range::Vector::rearrange()
{
	// Sort the vector first.
	sort();
	// Merges successive ranges after the sort if they are mergeable.
	size_type k = 0;
	// Iterate through vector which becomes smaller when merging entries.
	while (k < count() - 1 && count())
	{
		// Are the current and next range able to merge.
		if (at(k).isMergeable(at(k + 1)))
		{
			// Combine the 2 successive ranges to 1.
			at(k) += at(k + 1);
			// Remove the next now obsolete range of the vector.
			detachAt(k + 1);
			// Do not increment 'k' here because the range [k+1]
			// has changed because of the detachment of the previous range [k+1]
		}
		else
		{
			// Increment iterator value when merge was not possible.
			k++;
		}
	}
	return *this;
}

Range::Vector& Range::Vector::merge(const Range::Vector& rl_add)
{
	add(rl_add);
	// Rearrange the vector as it should.
	return rearrange();
}

Range::size_type Range::split(Range::size_type seg_sz, const Range& req, Range::Vector& rl_dst)
{
	// Calculate the effected segment range.
	Range seg(req.getStart() / seg_sz, req.getStop() / seg_sz + ((req.getStop() % seg_sz) ? 1 : 0));
	// Temporary value kept out of the loop.
	Range rng;
	// Iterate through the segments.
	for (auto i = seg.getStart(); i < seg.getStop(); i++)
	{
		// Get the data range of the current segment and the ID filed set to the segment index.
		rng.assign(i * seg_sz, (i + 1) * seg_sz, i);
		// Narrow the range down to what is requested.
		rng &= req;
		// Append the range to the list.
		rl_dst.add(rng);
	}
	// Return the amount of effected segments and thus added ranges.
	return seg.getSize();
}

Range::size_type Range::split(Range::size_type seg_sz, const Range::Vector& req, Range::Vector& rl)
{
	size_type count = 0;
	for (Vector::size_type i = 0; i < req.count(); i++)
	{
		count += split(seg_sz, req[i], rl);
	}
	return count;
}

std::istream& operator>>(std::istream& is, Range& r)
{
	char c;
	return is >> c >> r._start >> c >> r._stop >> c >> r._id >> c;
}

std::ostream& operator<<(std::ostream& os, const Range& r)
{
	return os << '(' << r._start << ',' << r._stop << ',' << r._id << ')';
}

std::ostream& operator<<(std::ostream& os, const Range::Vector& rl)
{
	Range::Vector::size_type count = rl.count();
	os << '(' << count;
	if (count)
	{
		for (Range::Vector::size_type i = 0; i < count; i++)
		{
			os << ',' << ' ' << rl[i];
		}
	}
	return os << ')';
}

std::istream& operator>>(std::istream& is, Range::Vector& rl)
{
	Range::Vector::size_type count = 0;
	char c;
	Range r;
	is >> c >> count;
	if (count)
	{
		// Clear the current entries of the vector
		rl.flush();
		// Resize the vector to the correct size so there is no need for automatic resizing the Vector.
		rl.resize(count);
		for (Range::Vector::size_type i = 0; i < count; i++)
		{
			is >> c >> r;
			rl[i] = r;
		}
	}
	return is >> c;
}

}// namespace sf
