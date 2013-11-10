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
#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <wchar.h>

#ifndef NULL
	#define NULL ((void *) 0)
#endif




#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
	#include <stdint.h>
	typedef wchar_t     wchar;
	#define __inline    inline
	#define __restrict  restrict
#else /* Not C99 compliant */
	typedef signed char         int8_t;
	typedef unsigned char       uint8_t;
	typedef signed short        int16_t;
	typedef unsigned short      uint16_t;
	typedef signed int          int32_t;
	typedef unsigned int        uint32_t;
	typedef signed long         int64_t;
	typedef unsigned long       uint64_t;
	#define __inline
	#define __restrict
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#ifndef boolean
typedef unsigned char    boolean; /* FALSE = 0, otherwise TRUE */
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1 
#endif
#else
#ifndef boolean
typedef bool             boolean; /* FALSE = 0, otherwise TRUE */
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE  true 
#endif
#endif
#ifndef byte
typedef unsigned char    byte;
#endif
#if !defined(_SSIZE_T)
#define _SSIZE_T
typedef long ssize_t;
#endif
typedef uint16_t         ushort;
typedef uint32_t         uint;
typedef uint32_t         ui32;
typedef int32_t          i32;
typedef uint16_t         ui16;
typedef int16_t          i16;
typedef float            f32;
typedef double           f64;
typedef unsigned char    uc8;

#endif /* _TYPES_H_ */
