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
	TCsfField fields[size];
	GetCsfFields(s, fields, fldnr + 1, sep, strdelim);
	return s.substr(fields[fldnr].Pos, fields[fldnr].Len);
}

std::string GetCsfField(int fldnr, const char* s, char sep, char strdelim)
{
	constexpr size_t size = 100;
	if (fldnr >= size || fldnr < 0)
	{
		return "";
	}
	TCsfField fields[size];
	GetCsfFields(s, fields, fldnr + 1, sep, strdelim);
	return std::string(s, fields[fldnr].Pos, fields[fldnr].Len);
}

int GetCsfFields(const char* s, strings& strings, int max, char sep, char strdelim)
{
	auto* fields = new TCsfField[max];
	int count = GetCsfFields(s, fields, max, sep, strdelim);
	for (int i = 0; i < count; i++)
	{
		strings.insert(strings.end(), std::string(s, fields[i].Pos, fields[i].Len));
	}
	delete[] fields;
	return count;
}

int GetCsfFields(const char* s, TCsfField* fld, int sz, char sep, char strdelim)
{
	memset(fld, 0, sizeof(TCsfField) * sz);
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
					fld[f].Len = l;
					fld[f].Pos += i - l;
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
				if (s[i] == strdelim)
				{
					d = !d;
					// Skip
					if (d)
					{
						fld[f].Pos--;
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
		fld[f].Len = l;
		fld[f].Pos += i - l;
		f++;
	}
	return f;
}

}