/*

Copyright (c) ScanFrame 2005.
All Rights Reserved.

$Source: /cppsrc/com/gii/gen/range.h,v $
$Author: avo $
$Revision: 1.6 $

Identification:
	COM_GII_GEN

Initial Author:
	AVO 19991116

Purpose:
	Interface of a class that handles a single range.

*/

#ifndef GII_RANGE_H
#define GII_RANGE_H

#include <iostream>
#include "mcvector.h"
//---------------------------------------------------------------------------
// Forward definition
class TRange;
//---------------------------------------------------------------------------
//
// Struct that contains the data members of class 'TRange'.
//
		// Set byte alignment to 1 byte.
//
#pragma pack(push,1)
typedef struct tagRANGE
{
	// Start position of the range or interval.
	long Start;
	// Stop position of the range or interval.
	long Stop;
	// Identifier of this range.
	long Id;
} RANGE;
//
// Restore the pack option.
#pragma pack(pop)
//---------------------------------------------------------------------------
//
// Class to manage integer ranges.
//
class TRange :private RANGE
{
	public:
		//
		typedef mcvector<TRange> TVector;
		typedef mciterator<TRange> TIterator;

		//
		// Constructors
		//
		// Default constructor
		TRange();
		// Copy constructor
		TRange(const TRange& r);
		// Constructor
		TRange(const RANGE& r);
		// Constructor
		TRange(long start, long stop, long id = 0);
		// Swaps start and stop if the order is wrong
		void Normalize();
		// Initialize data members
		TRange& Set(long start, long stop, long id = 0);
		// Initialize instance with other instance
		TRange& Set(const TRange& r);
		TRange& Set(const RANGE& r);
		// Copies the current instance to RANGE base struct.
		const TRange& CopyTo(RANGE& dest) const;
		//
		// Information functions/operators
		//
		// const function to access the start of the range.
		long GetStart() const;
		// const function to access the stop of the range.
		long GetStop() const;
		// clears the range to an empty state.
		void Clear();
		// returns the size of the range minimum is 1.
		long GetSize() const;
		// return true if the range is empty.
		bool IsEmpty() const;
		// returns if the passed range have some overlap.
		bool IsOverlapped(const TRange& r)const;
		// returns true if the passed range is part of this range.
		bool IsPartOfThis(const TRange& r) const;
		// returns true if this range is part of the passed range.
		bool IsPartOfOther(const TRange& r) const;
		// returns if the passed range an extension of this one.
		bool IsExtension(const TRange& r) const;
		// returns true if the passed range can be added to this range so that
		// result is one range
		bool IsMergeable(const TRange& r) const;
		// Check if idx is within this range
		bool IsInRange(long idx) const;
		// operator for testing content of this instance
		operator int() const;
		/* Base function for comparing ranges
			 '-' token of 'this' range
			 '=' token of the 'other' range
				0 = same range                           |------------|
																								 |============|

				1 = other range is part of this range    |--------------|
																										|=======|

			 -1 = this range is part of other range       |-------|
																								 |==============|

				2 = this range start beyond other start      |---------|
						this range stop beyond other stop    |=========|

			 -2 = this range start before other start  |---------|
						this range stop before other stop        |=========|

				3 = this range extends other                    |-----|
						this range beyond other range        |=====|

			 -3 = this range extends other             |-----|
						this range before other range               |=====|

				4 = ranges do not touch                           |-----|
						this range beyond other range        |=====|

			 -4 = ranges do not touch                  |-----|
						this range before other range                 |=====|

				5 = this range is empty                    ||
																								|=====|

			 -5 = other range is empty                |-----|
																									 ||

		*/
		int Compare(const TRange& other) const;
		// returns the onwer id of this range
		long GetId() const;
		// Set the onwer id of this range
		TRange& SetId(long id);

		// This virual assignment operator allow derived classes to
		// copy their data members
		TRange& operator=(const TRange& r);
		TRange& operator=(const RANGE& r);

		//
		// Compare operators
		//

		// operators that return true or false
		// if the range start and stop members are the same
		int operator ==(const TRange& r) const;
		int operator !=(const TRange& r) const;

		// Operator used for sorting
		// Depends only on the start position first and then the stop postion.
		// Empty ranges are always larger then non empty ones to be able to
		// move empty ones to the end of a vector.
		int operator <(const TRange& r) const;

		// And operator which is a subset of both range where elements
		// of the one also exists in the other range.
		TRange operator &(const TRange& r) const;
		TRange operator &=(const TRange& r);

		//
		// Functions/binary-operators that return or sizes
		//

		// Returns a new range which is a sub set of the two ranges
		TRange operator - (const TRange& r) const;
		TRange& operator -=(const TRange& r);

		// returns a new range which is a super set of the two ranges
		// the two ranges must atleast extend each other in order
		// to perform this operation otherwise the result is a range of (MAX_LONG,LONG_MIN)
		TRange operator + (const TRange& r) const;
		TRange& operator +=(const TRange& r);

		// Shifts the range using the passed offset.
		TRange& OffsetBy(long ofs);
		TRange Offset(long ofs) const;

	private:
		// operators are not implementedand private to prevent usage
		int operator >(const TRange& r) const;
		int operator <=(const TRange& r) const;
		int operator >=(const TRange& r) const;

	friend std::ostream& operator <<(std::ostream& os, const TRange& r);
	friend std::istream& operator >>(std::istream& is, TRange& r);

};
//---------------------------------------------------------------------------
//
inline
std::ostream& operator <<(std::ostream& os, const TRange& r)
{
	return os << '(' << r.Start << ',' << r.Stop << ',' << r.Id << ')';
}
//---------------------------------------------------------------------------
//
inline
std::istream& operator >>(std::istream& is, TRange& r)
{
	char c; return is >> c >> r.Start >> c >> r.Stop >> c >> r.Id >> c;
}
//---------------------------------------------------------------------------
//
inline
TRange::TRange()
{
	Clear();
}
//---------------------------------------------------------------------------
//
inline
TRange::TRange(const TRange& r)
{
	Set(r);
}
//---------------------------------------------------------------------------
//
inline
TRange::TRange(const RANGE& r)
{
	Set(r);
}
//---------------------------------------------------------------------------
//
inline
TRange::TRange(long start, long stop, long id)
{
	Set(start, stop, id);
}
//---------------------------------------------------------------------------
//
inline
long TRange::GetStart() const
{
	return Start;
}
//---------------------------------------------------------------------------
//
inline
long TRange::GetStop() const
{
	return Stop;
}
//---------------------------------------------------------------------------
//
inline
long TRange::GetId() const
{
	return Id;
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::SetId(long id)
{
	Id = id;
	return *this;
}
//---------------------------------------------------------------------------
//
inline
void TRange::Clear()
{
	Start = 0;
	Stop = 0;
	Id = 0;
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::Set(const TRange& r)
{
	*(tagRANGE*)this = *(tagRANGE*)&r;
	return *this;
}
//---------------------------------------------------------------------------
//
inline
long TRange::GetSize() const
{
	return Stop - Start;
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsEmpty() const
{
	return (Stop==Start);
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsMergeable(const TRange& r) const
{
	return abs(Compare(r))!=4;
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsOverlapped(const TRange& r) const
{
	return abs(Compare(r))<=2;
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsExtension(const TRange& r) const
{
	return abs(Compare(r))==3;
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsPartOfOther(const TRange& r) const
{
	int res = Compare(r);
	return (res==-1 || res==0);
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsPartOfThis(const TRange& r) const
{
	int res = Compare(r);
	return (res==1 || res==0);
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::operator=(const TRange& r)
{
	return Set(r);
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::operator=(const RANGE& r)
{
	return Set(r);
}
//---------------------------------------------------------------------------
//
inline
TRange::operator int() const
{
	return IsEmpty();
}
//---------------------------------------------------------------------------
//
inline
int TRange::operator ==(const TRange& r) const
{
	return Compare(r) == 0;
}
//---------------------------------------------------------------------------
//
inline
int TRange::operator !=(const TRange& r) const
{
	return Compare(r) != 0;
}
//---------------------------------------------------------------------------
//
inline
int TRange::operator <(const TRange& r) const
{
	return IsEmpty() ? 0 : (r.IsEmpty() ? 1 : ((Start==r.Start) ? Stop<r.Stop : Start<r.Start));
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::operator -=(const TRange& r)
{
	return Set(*this - r);
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::operator += (const TRange& r)
{
	return Set(*this + r);
}
//---------------------------------------------------------------------------
//
inline
TRange TRange::operator &=(const TRange& r)
{
	return Set(*this & r);
}
//---------------------------------------------------------------------------
//
inline
TRange& TRange::OffsetBy(long ofs)
{
	Start += ofs;
	Stop += ofs;
	return *this;
}
//---------------------------------------------------------------------------
//
inline
TRange TRange::Offset(long ofs) const
{
	return TRange(*this).OffsetBy(ofs);
}
//---------------------------------------------------------------------------
//
inline
bool TRange::IsInRange(long idx) const
{
	return idx >= Start && idx < Stop;
}
//---------------------------------------------------------------------------
//
inline
const TRange& TRange::CopyTo(RANGE& dest) const
{
	dest = *this;
	return *this;
}
//---------------------------------------------------------------------------
#endif // GII_RANGE_H
