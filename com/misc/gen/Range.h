#pragma once

#include <iostream>
#include "TVector.h"
#include "../global.h"

namespace sf
{

/**
 * Set the equal and unequal operators to include the id's.
 * Used in unit tests.
 */
_MISC_DATA extern bool RangeCompareExact;

// Packed structure for binary storage.
#pragma pack(push, 1)

/**
 * Struct that contains the data members of class 'Range'.
 */
struct RANGE
{
	/**
	 * @brief Integer type used for start and stop.
	 */
	typedef uint64_t size_type;
	/**
	 * @brief Integer type used for the ID.
	 */
	typedef int64_t id_type;
	/**
	 * @brief Start position of the range or interval.
	 */
	size_type _start;
	/**
	 * @brief Stop position of the range or interval.
	 */
	size_type _stop;
	/**
	 * @brief Identifier of this range.
	 */
	id_type _id;
};
// Restore the pack option.
#pragma pack(pop)

/**
 * @brief Class to manage 64-bit integer ranges.
 */
class _MISC_CLASS Range :private RANGE
{
	public:
		/**
		 * @brief Integer type used for start and stop.
		 */
		typedef RANGE::size_type size_type;
		/**
		 * @brief Integer type used for the ID.
		 */
		typedef RANGE::id_type id_type;

		/**
		 * @brief Type to contain and manipulate range lists.
		 */
		class _MISC_CLASS Vector :public TVector<Range>
		{
			public:
				/**
				 * @brief Sorts the vector according the Range operator '<'.
				 * @return Itself
				 */
				Vector& sort();

				/**
				 * @brief Sorts the vector according the Range operator '<'.
				 * @return A sorted copy of itself.
				 */
				[[nodiscard]] Vector sort() const;

				/**
				 * @brief Rearranges vector of ranges which means the vector is sorted and ranges are merged if possible.
				 */
				Vector& rearrange();

				/**
				 * @brief Exclude the parts of the ranges in this vector which corresponds with the passed vector.
				 * Removes all the matching parts of in this vector of the passed vector.
				 * The current vector needs to be sorted for it to work.
				 * @return Sorted vector.
				 */
				[[nodiscard]] Vector exclude(const Vector& rel) const;

				/**
				 * @brief Same as #exclude but non const and the vector is modified.
				 * @return This resulting vector.
				 */
				Vector& exclude(const Vector& rl_ex);

				/**
				 * @brief Extracts the covered ranges in the vector using the 'rl' vector and puts them in the 'rl_ex' vector.
				 * @return The resulting vector.
				 * @param rl Range vector to compare with for coverage. (must be rearranged sorted/merged)
				 * @param rl_ex Returns ranges that were covered. (The ID's of the ranges are preserved)
				 * @return Vector containing the range leftover after extraction.
				 */
				Vector extract(const Vector& rl, Vector& rl_ex) const;

				/**
				 * @brief Adds the passed list to this vector and rearranges it.
				 * @return This resulting vector.
				 */
				Vector& merge(const Vector& rl_add);
		};

		/**
		 * @brief Iteration type for lists of ranges.
		 */
		typedef Vector::iter_type Iterator;

		/**
		 * @brief Default constructor.
		 */
		Range();

		/**
		 * @brief Move assignment operator is default.
		 */
		Range& operator= (Range&&) = default;

		/**
		 * @brief Copy constructor.
		 * @param r
		 */
		Range(const Range& r);

		/**
		 * @brief Base class constructor.
		 */
		explicit Range(const RANGE& r);

		/**
		 * @brief Initializing constructor.
		 * @param start Start of the range.
		 * @param stop Start of the range.
		 * @param id Identifier of the range.
		 */
		Range(size_type start, size_type stop, id_type id = 0);

		/**
		 * @brief Swaps start and stop if the order is wrong.
		 */
		void normalize();

		/**
		 * @brief Assigns the data members.
		 * @param start Start of the range.
		 * @param stop Start of the range.
		 * @param id Identifier of the range.
		 * @return Itself
		 */
		Range& assign(size_type start, size_type stop, id_type id = 0);

		/**
		 * @brief InitializeBase instance with other instance.
		 */
		Range& assign(const Range& r);

		/**
		 * @brief InitializeBase instance with other instance.
		 */
		Range& set(const RANGE& r);

		/**
		 * Copies the current instance to RANGE base struct.
		 */
		const Range& copyTo(RANGE& dst) const;

		/**
		 * @brief Const function to access the start of the range.
		 */
		[[nodiscard]] size_type getStart() const;

		/**
		 * @brief Const function to access the stop of the range.
		 */
		[[nodiscard]] size_type getStop() const;

		/**
		 * @brief Clears the range to an empty state.
		 */
		void clear();

		/**
		 * @brief Returns the size of the range minimum is 1.
		 */
		[[nodiscard]] size_type getSize() const;

		/**
		 * @brief Return true if the range is empty.
		 */
		[[nodiscard]] bool isEmpty() const;

		/**
		 * @brief Returns if the passed range have some overlap.
		 */
		[[nodiscard]] bool isOverlapped(const Range& r) const;

		/**
		 * @brief Returns true if the passed range is part of this range.
		 */
		[[nodiscard]] bool isWithinOther(const Range& r) const;

		/**
		 * @brief Returns true if this range is part of the passed range.
		 */
		[[nodiscard]] bool isWithinSelf(const Range& r) const;

		/**
		 * @brief Returns if the passed range an extension of this one.
		 */
		[[nodiscard]] bool isExtension(const Range& r) const;

		/**
		 * @brief Returns true if the passed range can be combined to this range without having to bridge a gap.
		 */
		[[nodiscard]] bool isMergeable(const Range& r) const;

		/**
		 * @brief Splits this range into segments bounded ranges according to the passed segment size.
		 *
		 * The ID field of the range carries the segment index.
		 * @param seg_sz Size of the segment.
		 * @param rl_dst Destination list to receive the range(s) after splitting.
		 * @return Amount ranges it was split into.
		 */
		size_type split(size_type seg_sz, Vector& rl_dst) const
		{
			return split(seg_sz, *this, rl_dst);
		}

		/**
		 * @brief Splits a range into segments bounded ranges according to the passed segment size.
		 *
		 * The ID field of the range carries the segment index.
		 * @param seg_sz Size of the segment.
		 * @param req Requested range.
		 * @param rl_dst Destination list to receive the range(s) after splitting.
		 * @return Amount ranges it was split into.
		 */
		static size_type split(size_type seg_sz, const Range& req, Vector& rl_dst);

		/**
		 * @brief Same as #split but now for a complete vector of ranges.
		 *
		 * @param seg_sz Size of the segment.
		 * @param req Requested ranges.
		 * @param rl List where ranges are appended.
		 * @return Amount ranges the request list was split into.
		 */
		static size_type split(size_type seg_sz, const Vector& req, Vector& rl);

		/**
		 * @brief Check if idx is within this range
		 */
		[[nodiscard]] bool isInRange(size_type idx) const;

		/**
		 * @brief Boolean operator for testing content of this instance.
		 */
		explicit operator bool() const;

		/**
		 * @brief All possible comparison results.<br>
		 *
		 *  '-' token of 'this' range.<br>
		 *  '=' token of the 'other' range.<br>
		 */
		enum ECompare :int
		{
			/**
			 * Failed to compare.
			 */
			cmpError = std::numeric_limits<int>::max(),
			/**
			 * Both ranges ae the same.
			 * ```text
			 *    |------------|
			 *    |============|
			 *```
			 */
			cmpSame = 0,
			/**
			 * The other range fits in this range.
			 * ```text
			 *    |--------------|
			 *       |=======|
			 * ```
			 */
			cmpWithinSelf = 1,
			/**
			 * This range is fits in the other range.
			 * ```text
			 *       |-------|
			 *    |==============|
			 * ```
			 */
			cmpWithinOther = -1,
			/**
			 *  This range overlaps and extends the other.
			 * ```text
			 *        |---------|
			 *    |=========|
			 * ```
			 */
			cmpOverlapsOther = 2,
			/**
			 * The other range overlaps and extends this.
			 * ```text
			 *    |---------|
			 *        |=========|
			 * ```
			 */
			cmpOverlapsSelf = -2,
			/**
			 * This range extends the other exact.
			 * ```text
			 *           |-----|
			 *    |=====|
			 * ```
			 */
			cmpExtendsOther = 3,
			/**
			 * The other range extends this one exact.
			 * ```text
			 *    |-----|
			 *           |=====|
			 * ```
			 */
			cmpExtendsSelf = -3,
			/**
			 * The other range does not overlap and is located before this one.
			 * ```text
			 *             |-----|
			 *    |=====|
			 * ```
			 */
			cmpBeforeSelf = 4,
			/**
			 * The other range does not overlap and is located after this one.
			 * ```text
			 *    |-----|
			 *             |=====|
			 * ```
			 */
			cmpAfterSelf = -4,
			/**
			 *
			 * ```text
			 *      ||
			 *   |=====|
			 * ```
			 */
			cmpSelfEmpty = 5,
			/**
			 * ```text
			 *   |-----|
			 *      ||
			 * ```
			 */
			cmpOtherEmpty = -5,
		};

		/**
		 * @brief Base function for comparing ranges
		 */
		[[nodiscard]] ECompare compare(const Range& other) const;

		/**
		 * @brief Returns the owner id of this range.
		 */
		[[nodiscard]] id_type getId() const;

		/**
		 * @brief Set the owner id of this range.
		 */
		Range& setId(id_type id);

		/**
		 * @brief Assignment operator allow derived classes to copy their data members.
		 */
		Range& operator=(const Range& r);

		/**
		 * @brief Assignment operator allow derived classes to copy their data members.
		 */
		Range& operator=(const RANGE& r);

		/**
		 * @brief Tests if the range start and stop members are the same.
		 */
		bool operator==(const Range& r) const;

		/**
		 * @brief Tests if the range start and stop members are not the same.
		 */
		bool operator!=(const Range& r) const;

		/**
		 * @brief Operator used for sorting.
		 * Depends only on the start position first and then the stop position.
		 * Empty ranges are always larger then non empty ones to be able to
		 * move empty ones to the end of a vector.
		 */
		bool operator<(const Range& r) const;

		/**
		 * @brief And operator which is a subset of both range where elements of the one also exist in the other range.
		 */
		Range operator&(const Range& r) const;

		/**
		 * @brief And operator which is a subset of both range where elements of the one also exist in the other range.
		 */
		Range& operator&=(const Range& r);

		/**
		 * @brief Returns a new range which is a super Set of the two ranges.
		 *
		 * A gap in between is bridged by the resulting.
		 */
		Range operator+(const Range& r) const;

		/**
		 * @brief Returns a new range which is a super Set of the two ranges.
		 *
		 * A gap in between is bridged by the resulting.
		 */
		Range& operator+=(const Range& r);

		/**
	   * @brief Exclude the range in the the other which could result in an single range (0),
	   * an additional second range (1) or in an single empty range (-1).
	   *
		 * @param r Range excluded from this one.
		 * @param rest Set when 2 is returned.
		 * @param cmp Optional compare result.
	   * @return Amount of resulting ranges. Could be -1, 0 or 1.
		 */
		int exclude(const Range& r, Range& rest, Range::ECompare* cmp = nullptr);

		/**
		 * @brief Shifts this range using the passed offset.
		 *
		 * @param ofs Offset
		 * @return Itself
		 */
		Range& offsetBy(size_type ofs);

		/**
		 * @brief Shifts the range using the passed offset.
		 *
		 * @return The shifted range
		 */
		[[nodiscard]] Range offset(size_type ofs) const;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator>(const Range& r) const = delete;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator>(const Range& r) = delete;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator<=(const Range& r) const = delete;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator<=(const Range& r) = delete;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator>=(const Range& r) const = delete;

		/**
		 * @brief Not allowed operator.
		 */
		bool operator>=(const Range& r) = delete;

	private:
		friend std::ostream& operator<<(std::ostream& os, const Range& r);

		friend std::istream& operator>>(std::istream& is, Range& r);
};

/**
 * Output stream operator for a range.
 */
_MISC_FUNC std::ostream& operator<<(std::ostream& os, const Range& r);

/**
 * Input stream operator for a range.
 */
_MISC_FUNC std::istream& operator>>(std::istream& is, Range& r);


/**
 * Output stream operator for a range vector.
 */
_MISC_FUNC std::ostream& operator<<(std::ostream& os, const Range::Vector& rl);

/**
 * Input stream operator for a range vector.
 */
_MISC_FUNC std::istream& operator>>(std::istream& is, Range::Vector& rl);

inline
Range::Range()
	:RANGE()
{
	clear();
}

inline
Range::Range(const Range& r)
	:RANGE(r)
{
	assign(r);
}

inline
Range::Range(const RANGE& r)
	:RANGE()
{
	set(r);
}

inline
Range::Range(size_type start, size_type stop, id_type id)
	:RANGE()
{
	assign(start, stop, id);
}

inline
Range::size_type Range::getStart() const
{
	return _start;
}

inline
Range::size_type Range::getStop() const
{
	return _stop;
}

inline
Range::id_type Range::getId() const
{
	return _id;
}

inline
Range& Range::setId(id_type id)
{
	_id = id;
	return *this;
}

inline
void Range::clear()
{
	_start = 0;
	_stop = 0;
	_id = 0;
}

inline
Range& Range::assign(const Range& r)
{
	*(RANGE*) this = *(RANGE*) &r;
	return *this;
}

inline
Range::size_type Range::getSize() const
{
	return _stop - _start;
}

inline
bool Range::isEmpty() const
{
	return (_stop == _start);
}

inline
Range& Range::operator=(const Range& r)
{
	assign(r);
	return *this;
}

inline
Range& Range::operator=(const RANGE& r)
{
	set(r);
	return *this;
}

inline
Range::operator bool() const
{
	return isEmpty();
}

inline
bool Range::operator==(const Range& r) const
{
	return compare(r) == cmpSame && (!RangeCompareExact || _id == r._id);
}

inline
bool Range::operator!=(const Range& r) const
{
	return !(compare(r) == cmpSame && (!RangeCompareExact || _id == r._id));
}

inline
bool Range::operator<(const Range& r) const
{
	return !isEmpty() && (r.isEmpty() || ((_start == r._start) ? _stop < r._stop : _start < r._start));
}

inline
Range& Range::operator+=(const Range& r)
{
	return assign(*this + r);
}

inline
Range& Range::operator&=(const Range& r)
{
	return assign(*this & r);
}

inline
Range& Range::offsetBy(size_type ofs)
{
	_start += ofs;
	_stop += ofs;
	return *this;
}

inline
Range Range::offset(size_type ofs) const
{
	return Range(*this).offsetBy(ofs);
}

inline
bool Range::isInRange(size_type idx) const
{
	return idx >= _start && idx < _stop;
}

inline
const Range& Range::copyTo(RANGE& dst) const
{
	dst = *this;
	return *this;
}

} // namespace
