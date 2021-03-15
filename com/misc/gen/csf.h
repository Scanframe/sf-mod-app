#pragma once

#include <iostream>
#include "gen_utils.h"
#include "../global.h"

namespace sf
{

/**
 * Holds field position in the string.
 */
struct CsfField
{
	size_t _pos{0};
	size_t _len{0};
};

/**
 * Returns the locations of the fields int the character string in the
 * passed 'fld' array of size 'sz'. The size of the written fields is
 * returned as the function result.
 */
_MISC_FUNC int GetCsfFields(const char* s, CsfField* fld, int sz, char sep = ',', char str_delim = '"');

/**
 * Same but now a constant std::string class is passed.
 */
inline
int GetCsfFields(const std::string& s, CsfField* fld, int sz, char sep = ',', char str_delim = '"')
{
	return GetCsfFields(s.c_str(), fld, sz, sep, str_delim);
}

/**
 * Returns the comma separated string in a list.
 */
_MISC_FUNC int
GetCsfFields(const char* s, strings& strings, int max = 100, char sep = ',', char str_delim = '"');

/**
 * Same but now a constant std::string class is passed.
 */
inline
int GetCsfFields(const std::string& s, strings& strings, int max = 100, char sep = ',', char str_delim = '"')
{
	return GetCsfFields(s.c_str(), strings, max, sep, str_delim);
}

/**
 * Gets a field from string 's' according to the CSF rules.
 */
_MISC_FUNC std::string GetCsfField(int fldnr, const std::string& s, char sep = ',', char strdelim = '"');

/**
 * Gets a field from string 's' according to the CSF rules.
 */
_MISC_FUNC std::string GetCsfField(int fldnr, const char* s, char sep = ',', char strdelim = '"');

}