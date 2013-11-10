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
#include <time.h>
#include <array.h>

#define MAX 20

int main( int argc, char *argv[] )
{
	lc_array_t a;
	int i;

	array_create( &a, sizeof(double), 1 /* initial size */, malloc, free );
	srand( time(NULL) );

	array_resize( &a, MAX );

	for( i = 0; i < MAX; i++ )
	{
		double *p_num = array_elem( &a, i, double );
		*p_num = ((double) rand( )) / RAND_MAX;
	}

	for( i = 0; i < MAX; i++ )
	{
		double *p_num = array_elem( &a, i, double );

		printf( "array[%2d] = %f\n", i, *p_num );
	}

	array_destroy( &a );

	return 0;
}
