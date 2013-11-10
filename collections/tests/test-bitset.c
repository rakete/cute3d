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
#include <stdio.h>
#include <assert.h>
#include <bitset.h>

void print_bits( );

lc_bitset_t bits;

int main( int argc, char *argv[] )
{
	int i;
	boolean r = TRUE;

	r = bitset_create( &bits, 8 );
	assert( r );


	printf( "bits = %ld\n\n", bitset_bits( &bits ) );
	printf( "0123456789\n");


	bitset_set( &bits, 0 );
	bitset_set( &bits, 1 );
	bitset_set( &bits, 2 );
	bitset_set( &bits, 3 );
	bitset_set( &bits, 4 );
	bitset_set( &bits, 5 );
	bitset_set( &bits, 6 );
	bitset_set( &bits, 7 );

	print_bits( );



	if( bitset_test( &bits, 7 ) )
	{
		printf( "7th bit is set\n" );
	}


	r = bitset_resize( &bits, 64 );
	assert( r );

	bitset_set( &bits, 9 );
	bitset_unset( &bits, 8 );
	bitset_set( &bits, 37 );
	bitset_set( &bits, 38 );

	print_bits( );

	r = bitset_resize( &bits, 128 );
	assert( r );


	for( i = 64; i < bitset_bits(&bits); i++ )
	{
		bitset_set( &bits, i );
	}

	print_bits( );

	bitset_destroy( &bits );
	return 0;
}

void print_bits( )
{
	char *s = bitset_string( &bits );

	printf( "%s\n", s );
	free( s );
}
