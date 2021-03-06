#include "csf.h"

namespace sf
{

std::string GetCsfField(int fldnr, const std::string& s, char sep, char strdelim)
{
	constexpr size_t size = 100;
	if (fldnr >= size || fldnr < 0)
	{
		return "";
	}
	CsfField fields[size];
	GetCsfFields(s, fields, fldnr + 1, sep, strdelim);
	return s.substr(fields[fldnr]._pos, fields[fldnr]._len);
}

std::string GetCsfField(int fldnr, const char* s, char sep, char strdelim)
{
	constexpr size_t size = 100;
	if (fldnr >= size || fldnr < 0)
	{
		return "";
	}
	CsfField fields[size];
	GetCsfFields(s, fields, fldnr + 1, sep, strdelim);
	return std::string(s, fields[fldnr]._pos, fields[fldnr]._len);
}

int GetCsfFields(const char* s, strings& strings, int max, char sep, char str_delim)
{
	auto* fields = new CsfField[max];
	int count = GetCsfFields(s, fields, max, sep, str_delim);
	for (int i = 0; i < count; i++)
	{
		strings.insert(strings.end(), std::string(s, fields[i]._pos, fields[i]._len));
	}
	delete[] fields;
	return count;
}

int GetCsfFields(const char* s, CsfField* fld, int sz, char sep, char str_delim)
{
	memset(fld, 0, sizeof(CsfField) * sz);
	bool d = false;
	int f = 0;
	int l = 0;
	int i = 0;
	// Can only continue when
	if (sz && fld && s)
	{
		for (; s[i]; i++)
		{
			// When the character is a separator
			if (s[i] == sep && !d)
			{
				// First field is empty.
				if (i == 0)
				{
					f++;
				}
				else
				{
					fld[f]._len = l;
					fld[f]._pos += i - l;
					f++;
					l = 0;
					// Check if end of array has been reached and break the for loop.
					if (f >= sz)
					{
						break;
					}
				}
			}
			else
			{
				// Toggle delimiter flag when flag is encountered.
				if (s[i] == str_delim)
				{
					d = !d;
					// Skip
					if (d)
					{
						fld[f]._pos--;
					}
				}
				else
				{
					// Count the number of characters for the field.
					l++;
				}
			}
		}
	}
	// Check for characters after last separator;
	if (l)
	{
		fld[f]._len = l;
		fld[f]._pos += i - l;
		f++;
	}
	return f;
}

}