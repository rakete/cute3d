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
#ifndef _MACROS_H_
#define _MACROS_H_

#include <limits.h>

/* All macro's evaluate to compile-time constants */

/* *** helper macros *** */

/* turn a numeric literal into a hex constant
 * (avoids problems with leading zeroes)
 * 8-bit constants max value 0x11111111, always fits in unsigned long
 */
#define HEX__(n) 0x##n##LU

/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0) \
	+((x&0x000000F0LU)?2:0) \
	+((x&0x00000F00LU)?4:0) \
	+((x&0x0000F000LU)?8:0) \
	+((x&0x000F0000LU)?16:0) \
	+((x&0x00F00000LU)?32:0) \
	+((x&0x0F000000LU)?64:0) \
	+((x&0xF0000000LU)?128:0)

/* *** user macros *** */

/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))

/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
			+ B8(dlsb))

		/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
	+ ((unsigned long)B8(db2)<<16) \
	+ ((unsigned long)B8(db3)<<8) \
	+ B8(dlsb))

/* Sample usage
 *
 * B8(01010101) = 85
 * B16(10101010,01010101) = 43605
 * B32(10000000,11111111,10101010,01010101) = 2164238933
 */

#define is_enabled( flag, flags )   (((flags) & (flag)) != 0)
#define is_disabled( flag, flags )  (((flags) & (flag)) == 0)
#define is_set( flag, flags )       (is_enabled(flag, flags))

#define toggle_bit( flag, flags )   ((flags) ^= (flag))
#define set_bit( flag, flags )      ((flags) |= (flag))
#define unset_bit( flag, flags )    ((flags) &= ~(flag))




inline unsigned int fast_abs( unsigned int v )
{
	#ifdef HAVE_SIGN_EXTENDING_BITSHIFT
	int const mask = v >> sizeof(int) * CHAR_BIT - 1;
	#else
	int const mask = -((unsigned)v >> sizeof(int) * CHAR_BIT - 1);
	#endif
	return (v + mask) ^ mask;
}


#define max( x, y )              ((x) ^ (((x) ^ (y)) & -((x) < (y))))
#define min( x, y )              ((y) ^ (((x) ^ (y)) & -((x) < (y))))
#define is_power_of_2( x )       (((x) & ((x) - 1)) == 0)

#define integer_ceiling( x, y )  (((x) + (y) - 1) / (y))

inline int next_power_of_2( int v ) 
{
	int r = v;
	r--;
	r |= r >> 1;
	r |= r >> 2;
	r |= r >> 4;
	r |= r >> 8;
	r |= r >> 16;
	r++;
	return r;
}



#define set_if( condition, mask, flags )   ((flags) ^= (-(condition) ^ (flags)) & (mask))
#define set_if2( condition, mask, flags )  ((flags) = (flags) & ~(mask) | (-(condition) & (mask))) /* superscalar version */

#endif /* _MACROS_H_ */
