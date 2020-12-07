/*

Copyright (c) ScanFrame 2005.

All Rights Reserved.

$Source: /cppsrc/com/gii/gen/range.cpp,v $
$Author: avo $
$Revision: 1.9 $

Identification:
  COM_GII_GEN

Initial Author:
  1998/03/08 AVO

Purpose:
  Implementation of TRange and related functions

*/

#include <string>
#include <stdio.h>
#include "mcvector.h"
#include "genutils.h"
#include "dbgutils.h"
// Import our own interface
#include "range.h"
//---------------------------------------------------------------------------
//
//
TRange& TRange::Set(long start, long stop, long id)
{
  Start = start;
  Stop = stop;
  Id = id;
  Normalize();
  return *this;
}
//---------------------------------------------------------------------------
//
//
TRange& TRange::Set(const RANGE& r)
{
  *(RANGE*)this = r;
  Normalize();
  return *this;
}
//---------------------------------------------------------------------------
//
//
void TRange::Normalize()
{ // Check if the order is right
  if (Stop < Start)
  { // Notify of
    RTTI_NOTIFY(DO_DEFAULT, *this << " Normalized" );
    // Swap start and stop.
    swap_it(Start, Stop);
  }
}
//---------------------------------------------------------------------------
//
//
int TRange::Compare(const TRange& r) const
{
  register int Start2 = r.Start;
  register int Stop2  = r.Stop;

  // going for 0
  if ((Stop == Stop2) && (Start == Start2))
    return 0;

  // going for 5 is this empty
  if (Stop == Start)
    return 5;

  // going for -5 is other empty
  if (Stop2 == Start2)
    return -5;

  // going for -3 or -4
  if (Stop <= Start2)
     return ((Stop == r.Start) ? -3 : -4);

  // going for 3 or 4
  if (Stop2 <= Start)
    return ((Stop2 == Start) ? 3 : 4);

  // go for -1, 1
  if (Start == Start2)
    return ((Stop < Stop2) ? -1 : 1);

  // go for -1, 1
  if (Stop == Stop2)
    return ((Start < Start2) ? 1 : -1);

  // going for -1, 2
  if (Start > Start2)
    return ((Stop <= Stop2) ? -1 : 2);

  // going for 1, -2
  if (Start2 > Start)
    return ((Stop >= Stop2) ? 1 : -2);

  RTTI_NOTIFY(DO_DEFAULT, *this << "Compare" << r << " Failure!");
  return INT_MAX;
}
//---------------------------------------------------------------------------
//
//
TRange TRange::operator & (const TRange& r) const
{
  TRange rng;
  // Get the largset start value.
  rng.Start = (r.Start > Start) ? r.Start : Start;
  // Get the smallest stop value.
  rng.Stop = (r.Stop < Stop) ? r.Stop : Stop;
  // Set the Id to this Id
  rng.Id = Id;
  // Check for an empty range when Start is smaller or equal then Stop.
  if (rng.Start >= rng.Stop)
    // Clear the range because it should be empty.
    rng.Clear();
  return rng;
}
//---------------------------------------------------------------------------
//
//
TRange TRange::operator - (const TRange& r) const
{
  switch (Compare(r))
  {
    case 0:
    case -1:
    default:
      return TRange();

    case 1:
      // Check if the the range limits are the same because
      // that will result into a single result instead of two
      if (Start==r.Start)
        return TRange(r.Stop+1, Stop, Id);
      if (Stop==r.Stop)
        return TRange(Start, r.Start-1, Id);
      // Not
      RTTI_NOTIFY(DO_DEFAULT, *this << "operator-" << r << " Failed. Should Split In Two.");
      return *this;

    case 2:
      return TRange(Start, r.Stop, Id);

    case -2:
      return TRange(r.Start, Stop, Id);

    case -3:
    case 3:
    case -4:
    case 4:
    case -5:
    case 5:
      return *this;

  }
}
//---------------------------------------------------------------------------
//
//
TRange TRange::operator + (const TRange& r) const
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
     //RTTI_NOTIFY(DO_DEFAULT, " Inproper use 'operator +()' on " << *this << "+" << r);
    case 2:
    case 3:
      return TRange(r.Start, Stop, Id);

    case -4:
      //RTTI_NOTIFY(DO_DEFAULT , " Inproper use 'operator +()' on " << *this << "+" << r);
    case -2:
    case -3:
      return TRange(Start, r.Stop, Id);

    default:
      return TRange();
  }
}
//---------------------------------------------------------------------------
/*
//
//
int TRange::ExclusiveOr(const TRange& r, TRange& d1, TRange& d2) const
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
//------------------------------------------------------------------------------

/*
$Log: range.cpp,v $
Revision 1.9  2005/10/05 19:04:54  avo
Added some comment for help.

Revision 1.8  2005/09/25 16:07:28  avo
Extensive renaming session.

Revision 1.7  2003/07/18 19:16:51  avo
Replaced CLASS with RTTI notify macro's

Revision 1.6  2002/03/04 12:54:20  avo
Fixed loosing of ID when anding a range.

Revision 1.5  2001/06/12 15:59:46  ARJAN
Fixed slash direction for precompiled header for faster compiling.

Revision 1.4  2000/11/22 13:36:22  ARJAN
Added bit-wise and operator that returns the common range of both ranges.

Revision 1.3  2000/05/16 14:39:10  ARJAN
Upgrade of compiler to BCB5

Revision 1.2  1999/12/01 12:41:54  ARJAN
Added gen/pch.h for faster compilation.

Revision 1.1  1999/11/18 15:00:06  ARJAN
Initial import of common library files

*/
