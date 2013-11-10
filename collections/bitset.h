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
#ifndef _BITSET_H_
#define _BITSET_H_

#include <limits.h>
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct lc_bitset {
	byte*  array;
	size_t bit_size;
} lc_bitset_t;

#if CHAR_BIT == 8
#define bits_to_bytes(bits)                (((bits) + 7) >> 3)
#define bit_to_index(bit)                  ((bit) >> 3)
#else
#define bits_to_bytes(bits)                (((bits) + CHAR_BIT - 1) / CHAR_BIT)
#define bit_to_index(bit)                  ((bit) / CHAR_BIT )
#endif


boolean        bitset_create  ( lc_bitset_t* p_bitset, size_t bits );
void           bitset_destroy ( lc_bitset_t* p_bitset );
void           bitset_clear   ( lc_bitset_t* p_bitset );
boolean        bitset_resize  ( lc_bitset_t* p_bitset, size_t bits );
char*          bitset_string  ( lc_bitset_t* p_bitset );
#define        bitset_bits(p_bitset)              ((p_bitset)->bit_size)

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
void    bitset_set     ( lc_bitset_t* p_bitset, size_t bit );
void    bitset_unset   ( lc_bitset_t* p_bitset, size_t bit );
boolean bitset_test    ( const lc_bitset_t* p_bitset, size_t bit );
#else
static __inline void bitset_set( lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	assert( bit < p_bitset->bit_size );
	p_bitset->array[ bit_to_index(bit) ] |= (0x01 << (bit % CHAR_BIT));
}

static __inline void bitset_unset( lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	assert( bit < p_bitset->bit_size );
	p_bitset->array[ bit_to_index(bit) ] &= ~(0x01 << (bit % CHAR_BIT));
}

static __inline boolean bitset_test( const lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	return p_bitset->array[ bit_to_index(bit) ] & (0x01 << (bit % CHAR_BIT));
}
#endif

#ifdef __cplusplus
} /* external C linkage */
#endif
#endif /* _BITSET_H_ */
