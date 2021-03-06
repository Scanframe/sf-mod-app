#pragma once

#include <iostream>
#include "TVector.h"
#include "../global.h"

namespace sf
{

// Set byte alignment to 1 byte.
#pragma pack(push, 1)

/**
 * Struct that contains the data members of class 'Range'.
 */
typedef struct tagRANGE
{
	/**
	 * Integer type used for start and stop.
	 */
	typedef int32_t size_type;
	/**
	 * Integer type used for the ID.
	 */
	typedef int32_t id_type;
	/**
	 * Start position of the range or interval.
	 */
	size_type _start;
	/**
	 * Stop position of the range or interval.
	 */
	size_type _stop;
	/**
	 * Identifier of this range.
	 */
	id_type _id;
} RANGE;
// Restore the pack option.
#pragma pack(pop)

/**
 * Class to manage integer ranges.
 */
class _MISC_CLASS Range :private RANGE
{
	public:
		/**
		 * Type to create lists of ranges.
		 */
		typedef TVector <Range> Vector;
		/**
		 * Iteration type for lists of ranges.
		 */
		typedef Vector::iter_type Iterator;

		/**
		 * Default constructor
		 */
		Range();

		/**
		 * Copy constructor
		 * @param r
		 */
		Range(const Range& r);

		/**
		 * Base class constructor
		 */
		explicit Range(const RANGE& r);

		/**
		 * Constructor
		 */
		Range(size_type start, size_type stop, size_type id = 0);

		/**
		 * Swaps start and stop if the order is wrong
		 */
		void normalize();

		/**
		 * InitializeBase data members
		 * @param start
		 * @param stop
		 * @param id
		 * @return
		 */
		Range& Set(size_type start, size_type stop, size_type id = 0);

		/**
		 * InitializeBase instance with other instance
		 */
		Range& Set(const Range& r);

		/**
		 * InitializeBase instance with other instance
		 */
		Range& Set(const RANGE& r);

		/**
		 * Copies the current instance to RANGE base struct.
		 */
		const Range& CopyTo(RANGE& dest) const;

		/**
		 * const function to access the start of the range.
		 */
		[[nodiscard]] size_type GetStart() const;

		/**
		 * Const function to access the stop of the range.
		 */
		[[nodiscard]] size_type GetStop() const;

		/**
		 * clears the range to an empty state.
		 */
		void Clear();

		/**
		 * Returns the size of the range minimum is 1.
		 */
		[[nodiscard]] size_type GetSize() const;

		/**
		 * Return true if the range is empty.
		 */
		[[nodiscard]] bool IsEmpty() const;

		/**
		 * Returns if the passed range have some overlap.
		 * @param r
		 * @return
		 */
		[[nodiscard]] bool IsOverlapped(const Range& r) const;

		/**
		 * Returns true if the passed range is part of this range.
		 */
		[[nodiscard]] bool IsPartOfThis(const Range& r) const;

		/**
		 * Returns true if this range is part of the passed range.
		 */
		[[nodiscard]] bool IsPartOfOther(const Range& r) const;

		/**
		 * Returns if the passed range an extension of this one.
		 * @param r
		 * @return
		 */
		[[nodiscard]] bool IsExtension(const Range& r) const;

		/**
		 * Returns true if the passed range can be added to this range so that
		 * @param r
		 * @return One range
		 */
		[[nodiscard]] bool IsMergeable(const Range& r) const;

		/**
		 * Check if idx is within this range
		 */
		[[nodiscard]] bool IsInRange(size_type idx) const;

		/**
		 * Boolean operator for testing content of this instance
		 */
		explicit operator bool() const;

		/**
		 * Base function for comparing ranges
		 *    '-' token of 'this' range
		 *    '=' token of the 'other' range
		 *     0 = same range                           |------------|
		 *                                              |============|
		 *
		 *     1 = other range is part of this range    |--------------|
		 *                                                 |=======|
		 *
		 *    -1 = this range is part of other range       |-------|
		 *                                              |==============|
		 *
		 *     2 = this range start beyond other start      |---------|
		 *         this range stop beyond other stop    |=========|
		 *
		 *    -2 = this range start before other start  |---------|
		 *         this range stop before other stop        |=========|
		 *
		 *     3 = this range extends other                    |-----|
		 *         this range beyond other range        |=====|
		 *
		 *    -3 = this range extends other             |-----|
		 *         this range before other range               |=====|
		 *
		 *     4 = ranges do not touch                           |-----|
		 *         this range beyond other range        |=====|
		 *
		 *    -4 = ranges do not touch                  |-----|
		 *         this range before other range                 |=====|
		 *
		 *     5 = this range is empty                    ||
		 *                                             |=====|
		 *
		 *    -5 = other range is empty                |-----|
		 *                                                ||
		 */
		[[nodiscard]] int Compare(const Range& other) const;

		/**
		 * Returns the owner id of this range.
		 */
		[[nodiscard]] size_type GetId() const;

		/**
		 * Set the owner id of this range.
		 */
		Range& SetId(size_type id);

		/**
		 * Assignment operator allow derived classes to copy their data members.
		 */
		Range& operator=(const Range& r);

		/**
		 * Assignment operator allow derived classes to copy their data members.
		 */
		Range& operator=(const RANGE& r);

		/**
		 * Tests if the range start and stop members are the same.
		 */
		bool operator==(const Range& r) const;

		/**
		 * Tests if the range start and stop members are not the same.
		 */
		bool operator!=(const Range& r) const;

		/**
		 * Operator used for sorting
		 * Depends only on the start position first and then the stop position.
		 * Empty ranges are always larger then non empty ones to be able to
		 * move empty ones to the end of a vector.
		 */
		int operator<(const Range& r) const;

		/**
		 * And operator which is a subset of both range where elements
		 * of the one also exists in the other range.
		 */
		Range operator&(const Range& r) const;

		Range operator&=(const Range& r);

		/**
		 * Returns a new range which is a sub Set of the two ranges
		 */
		Range operator-(const Range& r) const;

		/**
		 * Returns a new range which is a sub Set of the two ranges
		 */
		Range& operator-=(const Range& r);

		/**
		 * Returns a new range which is a super Set of the two ranges
		 * the two ranges must at least extend each other in order
		 * to perform this operation otherwise the result is a range of (MAX_LONG,LONG_MIN)
		 */
		Range operator+(const Range& r) const;

		/**
		 * Returns a new range which is a super Set of the two ranges
		 * the two ranges must at least extend each other in order
		 * to perform this operation otherwise the result is a range of (MAX_LONG,LONG_MIN)
		 */
		Range& operator+=(const Range& r);

		/**
		 * Shifts the range using the passed offset.
		 */
		Range& OffsetBy(size_type ofs);

		/**
		 * Shifts the range using the passed offset.
		 */
		[[nodiscard]] Range Offset(size_type ofs) const;

		/**
		 * Not allowed operators.
		 */
		bool operator>(const Range& r) const = delete;

		bool operator>(const Range& r) = delete;

		bool operator<=(const Range& r) const = delete;

		bool operator<=(const Range& r) = delete;

		bool operator>=(const Range& r) const = delete;

		bool operator>=(const Range& r) = delete;

		friend std::ostream& operator<<(std::ostream& os, const Range& r);

		friend std::istream& operator>>(std::istream& is, Range& r);

};

inline
std::ostream& operator<<(std::ostream& os, const Range& r)
{
	return os << '(' << r._start << ',' << r._stop << ',' << r._id << ')';
}

inline
std::istream& operator>>(std::istream& is, Range& r)
{
	char c;
	return is >> c >> r._start >> c >> r._stop >> c >> r._id >> c;
}

inline
Range::Range()
	:tagRANGE()
{
	Clear();
}

inline
Range::Range(const Range& r)
	:tagRANGE(r)
{
	Set(r);
}

inline
Range::Range(const RANGE& r)
	:tagRANGE()
{
	Set(r);
}

inline
Range::Range(size_type start, size_type stop, id_type id)
	:tagRANGE()
{
	Set(start, stop, id);
}

inline
Range::size_type Range::GetStart() const
{
	return _start;
}

inline
Range::size_type Range::GetStop() const
{
	return _stop;
}

inline
Range::id_type Range::GetId() const
{
	return _id;
}

inline
Range& Range::SetId(id_type id)
{
	_id = id;
	return *this;
}

inline
void Range::Clear()
{
	_start = 0;
	_stop = 0;
	_id = 0;
}

inline
Range& Range::Set(const Range& r)
{
	*(tagRANGE*) this = *(tagRANGE*) &r;
	return *this;
}

inline
Range::size_type Range::GetSize() const
{
	return _stop - _start;
}

inline
bool Range::IsEmpty() const
{
	return (_stop == _start);
}

inline
bool Range::IsMergeable(const Range& r) const
{
	return abs(Compare(r)) != 4;
}

inline
bool Range::IsOverlapped(const Range& r) const
{
	return abs(Compare(r)) <= 2;
}

inline
bool Range::IsExtension(const Range& r) const
{
	return abs(Compare(r)) == 3;
}

inline
bool Range::IsPartOfOther(const Range& r) const
{
	int res = Compare(r);
	return (res == -1 || res == 0);
}

inline
bool Range::IsPartOfThis(const Range& r) const
{
	int res = Compare(r);
	return (res == 1 || res == 0);
}

inline
Range& Range::operator=(const Range& r)
{
	Set(r);
	return *this;
}

inline
Range& Range::operator=(const RANGE& r)
{
	Set(r);
	return *this;
}

inline
Range::operator bool() const
{
	return IsEmpty();
}

inline
bool Range::operator==(const Range& r) const
{
	return Compare(r) == 0;
}

inline
bool Range::operator!=(const Range& r) const
{
	return Compare(r) != 0;
}

inline
int Range::operator<(const Range& r) const
{
	return IsEmpty() ? 0 : (r.IsEmpty() ? 1 : ((_start == r._start) ? _stop < r._stop : _start < r._start));
}

inline
Range& Range::operator-=(const Range& r)
{
	return Set(*this - r);
}

inline
Range& Range::operator+=(const Range& r)
{
	return Set(*this + r);
}

inline
Range Range::operator&=(const Range& r)
{
	return Set(*this & r);
}

inline
Range& Range::OffsetBy(size_type ofs)
{
	_start += ofs;
	_stop += ofs;
	return *this;
}

inline
Range Range::Offset(size_type ofs) const
{
	return Range(*this).OffsetBy(ofs);
}

inline
bool Range::IsInRange(size_type idx) const
{
	return idx >= _start && idx < _stop;
}

inline
const Range& Range::CopyTo(RANGE& dest) const
{
	dest = *this;
	return *this;
}

} // namespace
