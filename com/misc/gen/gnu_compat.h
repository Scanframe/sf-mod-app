#pragma once

#if IS_WIN

// Create ecvt_r() not present in when building for Windows.
inline
int ecvt_r (double value, int ndigit, int* decpt, int* sign, char* buf, size_t len)
{
	return _ecvt_s(buf, len, value, ndigit, decpt, sign);
}

#endif
