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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "bitset.h"

boolean bitset_create( lc_bitset_t* p_bitset, size_t bits )
{
	size_t size = bits_to_bytes( bits );

	assert( p_bitset );

	p_bitset->bit_size = bits;
	p_bitset->array    = malloc( sizeof(byte) * size );

	memset( p_bitset->array, 0, sizeof(byte) * size );
	assert( p_bitset->array );

	return p_bitset->array != NULL;
}

void bitset_destroy( lc_bitset_t* p_bitset )
{
	assert( p_bitset );
	free( p_bitset->array );

	#ifdef _DEBUG_BITSET
	p_array->array    = NULL;
	p_array->bit_size = 0L;
	#endif
}

void bitset_clear( lc_bitset_t* p_bitset )
{
	assert( p_bitset );
	memset( p_bitset->array, 0, sizeof(byte) * bits_to_bytes(p_bitset->bit_size) );
}

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
void bitset_set( lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	assert( bit < p_bitset->bit_size );
	p_bitset->array[ bit_to_index(bit) ] |= (0x01 << (bit % CHAR_BIT));
}

void bitset_unset( lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	assert( bit < p_bitset->bit_size );
	p_bitset->array[ bit_to_index(bit) ] &= ~(0x01 << (bit % CHAR_BIT));
}

boolean bitset_test( const lc_bitset_t* p_bitset, size_t bit )
{
	assert( p_bitset );
	return p_bitset->array[ bit_to_index(bit) ] & (0x01 << (bit % CHAR_BIT));
}
#endif


boolean bitset_resize( lc_bitset_t* p_bitset, size_t bits )
{
	size_t size;
	size_t new_size;

	assert( p_bitset );

	size     = bits_to_bytes( p_bitset->bit_size );
	new_size = bits_to_bytes( bits );

	if( size != new_size )
	{
		p_bitset->bit_size = bits;
		p_bitset->array    = realloc( p_bitset->array, sizeof(byte) * new_size );

		memset( p_bitset->array + size, 0, sizeof(byte) * (new_size - size) );
	}

	return p_bitset->array != NULL;
}

char *bitset_string( lc_bitset_t* p_bitset )
{
	char *result = (char *) malloc( p_bitset->bit_size + 1 );

	if( result )
	{
		size_t i;

		for( i = 0; i < p_bitset->bit_size; i++ )
		{
			result[ i ] = bitset_test(p_bitset, i) ? '1' : '0';
		}

		result[ i ] = '\0';
	}

	return result;
}
