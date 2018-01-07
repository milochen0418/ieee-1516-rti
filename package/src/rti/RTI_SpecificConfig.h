/***********************************************************************
 IEEE 1516.1 High Level Architecture Interface Specification C++ API
 File: RTI_SpecificConfig.h
 ***********************************************************************/

// Purpose: This file contains definitions that are used to isolate
// platform configuration issues.

#ifndef RTI_SpecificConfig_h
#define RTI_SpecificConfig_h

#include <sys/types.h> // for size_t
#include <stdint.h>
#include <string>

// macro to support Windows library export mechanism
#define RTI_EXPORT /* nothing */

//
// When and how to use inline methods is left up to the RTI implementor
//

#ifdef RTI_DEBUG_ON
#undef RTI_USE_INLINE
#define RTI_INLINE /* inline */
#else
#define RTI_USE_INLINE
#define RTI_INLINE inline
#endif // RTI_DEBUG_ON

#ifdef __GNUC__
#ifndef RTI_TEMPLATES_REQUIRE_SOURCE
#define RTI_TEMPLATES_REQUIRE_SOURCE
#endif
#endif // __GNUC__

using namespace std;
#define RTI_wstring	std::wstring

inline RTI_wstring StrtoWStr(char *str)
{
	wchar_t wcs[100];
	setlocale(LC_CTYPE, "");
	mbstowcs(wcs, str, 99);
	return wstring(wcs);
};

inline RTI_wstring StrtoWStr(string str)
{
	wchar_t wcs[100];
	setlocale(LC_CTYPE, "");
	mbstowcs(wcs, str.c_str(), 99);
	return wstring(wcs);
};

inline size_t WStrtoStr(char *dest, const wstring &src, int size)
{
	setlocale(LC_CTYPE, "");
	return wcstombs(dest, src.c_str(), size);
};


#endif // RTI_SpecificConfig_h

/*
 * vim: ts=4 sts=4 sw=4
 */
