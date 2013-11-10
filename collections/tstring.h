/*
 * Copyright (C) 2010 by Joseph A. Marrero.  http://www.manvscode.com/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#ifndef _TSTRING_H_
#define _TSTRING_H_

#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif


#if defined(WIN32) || defined(WIN64)
#if defined(UNICODE)
	#define wcsncmp     _wcsncmp
	#define wcscasecmp  _wcsnicmp
#else
	#define strcasecmp  _stricmp
	#define strncasecmp _strnicmp
#endif
#endif 

#ifndef wcsdup
wchar* wcsdup( const wchar* s );
#endif

#if defined(UNICODE)
	typedef wchar_t      tchar;
	#ifndef TEXT
	#define TEXT(str)    L##str
	#endif
	#define tstrdup      wcsdup
	#define tstrlen      wcslen
	#define tstrcmp      wcscmp
	#define tstrcoll     wcscoll
	#define tstrncmp     wcsncmp
	#define tstrcasecmp  wcscasecmp
	#define tstrncasecmp wcsncasecmp
	#define tstrcpy      wcscpy
	#define tstrncpy     wcsncpy
	#define tstrcat      wcscat
	#define tstrncat     wcsncat
	#define tstrspn      wcsspn
	#define tstrcspn     wcscspn
	#define tstrpbrk     wcspbrk
	#define tstrtok      wcstok
	#define tstrchr      wcschr
	#define tstrstr      wcsstr
	#define tchar_lower  towlower
	#define tchar_upper  towupper
	#define tprintf      wprintf
	#define tfprintf     fwprintf
	#define tsprintf     #error "There is no wide character equivalent to vsprintf"
	#define tsnprintf    swprintf
	#define tvprintf     vwprintf
	#define tvfprintf    vfwprintf
	#define tvsprintf    vswprintf
	#define tvsnprintf   vswprintf /* intentionally missing n */
	#define tmemcpy      wmemcpy
	#if defined(WIN32)
		#define _tmain       wmain  /* main() is wmain() */
	#else
		#define _tmain       main  
	#endif
#else
	typedef char         tchar;
	#ifndef TEXT
	#define TEXT(str)    str
	#endif
	#define tstrdup      strdup
	#define tstrlen      strlen
	#define tstrcmp      strcmp
	#define tstrcoll     strcoll
	#define tstrncmp     strncmp
	#define tstrcasecmp  strcasecmp
	#define tstrncasecmp strncasecmp
	#define tstrcpy      strcpy
	#define tstrncpy     strncpy
	#define tstrcat      strcat
	#define tstrncat     strncat
	#define tstrspn      strspn
	#define tstrcspn     strcspn
	#define tstrpbrk     strpbrk
	#define tstrtok      strtok
	#define tstrchr      strchr
	#define tstrstr      strstr
	#define tchar_lower  tolower
	#define tchar_upper  toupper
	#define tprintf      printf
	#define tfprintf     fprintf
	#define tsprintf     sprintf
	#define tsnprintf    snprintf
	#define tvprintf     vprintf
	#define tvfprintf    vfprintf
	#define tvsprintf    vsprintf
	#define tvsnprintf   vsnprintf
	#define tmemcpy      memcpy
	#define _tmain       main  
#endif
	

	
#define ascii_to_unicode     mbsrtowcs /*  char* to wchar_t*  */
#define unicode_to_ascii     wcstombs  /*  wchar_t* to char*  */

#ifndef _T
#define _T(str)          TEXT(str)
#endif
#define TERM_BEG(str)    (str[0] = '\0')
#define TERM_END(str)    (str[sizeof(str) - 1] = '\0')

typedef struct lc_tstring {
	size_t length;
	tchar* s;
} lc_tstring_t;


boolean tstring_create       ( lc_tstring_t* p_string, const tchar *s );
void    tstring_destroy      ( const lc_tstring_t* p_string );
int     tstring_compare      ( const lc_tstring_t* left, const lc_tstring_t* right );
int     tstring_ncompare     ( const lc_tstring_t* left, const lc_tstring_t* right, size_t n );
int     tstring_casecompare  ( const lc_tstring_t* left, const lc_tstring_t* right );
int     tstring_ncasecompare ( const lc_tstring_t* left, const lc_tstring_t* right, size_t n );
void    tstring_tolower      ( lc_tstring_t* p_string );
void    tstring_toupper      ( lc_tstring_t* p_string );
boolean tstring_copy         ( lc_tstring_t* p_result, const lc_tstring_t* p_string );
boolean tstring_ncopy        ( lc_tstring_t* p_result, const lc_tstring_t* p_string, size_t n );
boolean tstring_assign       ( lc_tstring_t* p_result, const tchar *p_string );
boolean tstring_concatenate  ( lc_tstring_t* p_result, const lc_tstring_t* s );
boolean tstring_nconcatenate    ( lc_tstring_t* p_result, const lc_tstring_t* p_string, size_t n );
boolean tstring_sconcatenate    ( lc_tstring_t* p_string, const tchar *s );
void    tstring_format       ( lc_tstring_t* p_string, const tchar *format, ... );
size_t  tstring_rtrim        ( lc_tstring_t* p_string );
size_t  tstring_ltrim        ( lc_tstring_t* p_string );
size_t  tstring_trim         ( lc_tstring_t* p_string );

#define tstring_get( p_string )            ((p_string)->s)
#define tstring_string( p_string )         ((p_string)->s)
#define tstring_length( p_string )         ((p_string)->length)
#define tstring_size( p_string )           (sizeof(tchar) * ((p_string)->length + 1))

size_t ltrim   ( tchar* s, const tchar* delimeters );
size_t rtrim   ( tchar* s, const tchar* delimeters );
size_t trim    ( tchar* s, const tchar* delimeters );
tchar* tstrtolower ( tchar* s );
tchar* tstrtoupper ( tchar* s );

#ifdef __cplusplus
} /* external C linkage */
#endif
#endif /* _TSTRING_H_ */
