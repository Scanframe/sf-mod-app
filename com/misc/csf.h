#pragma once

#include "global.h"
#include <iostream>
#include "genutils.h"

namespace sf
{

/**
 * Holds position in
 */
struct TCsfField
{
	size_t Pos{0};
	size_t Len{0};
};

/**
 * Returns the locations of the fields int the character string in the
 * passed 'fld' array of size 'sz'. The size of the written fields is
 * returned as the function result.
 */
_MISC_FUNC int GetCsfFields(const char* s, TCsfField* fld, int sz, char sep = ',', char strdelim = '"');

/**
 * Same but now a constant std::string class is passed.
 */
inline
int GetCsfFields(const std::string& s, TCsfField* fld, int sz, char sep = ',', char strdelim = '"')
{
	return GetCsfFields(s.c_str(), fld, sz, sep, strdelim);
}

/**
 * Returns the comma separated string in a list.
 */
_MISC_FUNC int
GetCsfFields(const char* s, strings& strings, int max = 100, char sep = ',', char strdelim = '"');

/**
 * Same but now a constant std::string class is passed.
 */
inline
int GetCsfFields(const std::string& s, strings& strings,
	int max = 100, char sep = ',', char strdelim = '"') {return GetCsfFields(s.c_str(), strings, max, sep, strdelim);}

/**
 * Gets a field from string 's' according to the CSF rules.
 */
_MISC_FUNC std::string GetCsfField(int fldnr, const std::string& s, char sep = ',', char strdelim = '"');

/**
 * Gets a field from string 's' according to the CSF rules.
 */
_MISC_FUNC std::string GetCsfField(int fldnr, const char* s, char sep = ',', char strdelim = '"');

}