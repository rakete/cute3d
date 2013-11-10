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
#include <math.h>
#include <time.h>
#include <bheap.h>

#define SIZE  		100

#define MAX_HEAP

static int     int_compare( const int* __restrict left, const int* __restrict right );

DECLARE_BHEAP_TYPE( num, int )
IMPLEMENT_BHEAP_TYPE( num, int )


int main( int argc, char *argv[] )
{
	int i;
	bheap_num_t heap;

	time_t t = time(NULL);
	srand( t );

	bheap_num_create( &heap, 1, int_compare );

	printf( "seed = %ld\n", t );

	for( i = 0; i < SIZE; i++ )
	{
		int num = (rand() % SIZE) + (rand() % (SIZE / 5))* pow(-1.0, i);
		bheap_num_push( &heap, &num );
	}

	for( i = 0; i < 0.25 * SIZE; i++ )
	{
		bheap_num_pop( &heap );
	}

	for( i = 0; i < SIZE; i++ )
	{
		int num = (rand() % SIZE) + (rand() % (SIZE / 3))* pow(-1.0, i);
		bheap_num_push( &heap, &num );
	}

	for( i = 0; i < 0.25 * SIZE; i++ )
	{
		bheap_num_pop( &heap );
	}

	printf( "   ----- Sorted Output -----\n" );
	while( bheap_num_size(&heap) > 0 )
	{
		int* p_num = bheap_num_peek( &heap );
		printf( "%10d (size = %02ld) \n", *p_num, bheap_num_size(&heap) );

		bheap_num_pop( &heap );
	}

	bheap_num_destroy( &heap );
	return 0;
}


int int_compare( const int* __restrict p_left, const int* __restrict p_right )
{
	#ifdef MAX_HEAP
	return (*p_left) - (*p_right); /* max-heap */
	#else
	return (*p_right) - (*p_left); /* min-heap */
	#endif
}

