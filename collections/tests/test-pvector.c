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
#include <vector.h>

typedef struct point {
	double x;
	double y;
	char name;
} point_t;

boolean point_destroy( point_t *pt )
{
	/* vector makes a shallow copy of the
 	 * data on the array. There is nothing
 	 * to free.
 	 */
	free( pt );
	return TRUE;
}

#define SIZE  30

#define create_point( )  ((point_t *) malloc( sizeof(point_t) ))


int main( int argc, char *argv[] )
{
	lc_pvector_t collection;
	int i;
	char names[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	srand( time(NULL) );

	pvector_create( &collection, 1, malloc, free );

	for( i = 0; i < SIZE; i++ )
	{
		point_t *p_pt = create_point( );

		p_pt->x    = ((rand( ) % 100) - 50.0);
		p_pt->y    = ((rand( ) % 100) - 50.0);
		p_pt->name = names[ rand() % (sizeof(names) - 1) ];

		pvector_push( &collection, p_pt );
	}

	for( i = 0; i < 0.25 * SIZE; i++ )
	{
		point_t *p_pt = pvector_peek( &collection );
		point_destroy( p_pt );

		pvector_pop( &collection );
	}

	for( i = 0; i < 0.4 * SIZE; i++ )
	{
		point_t *p_pt = create_point( );

		p_pt->x    = ((rand( ) % 100) - 50.0);
		p_pt->y    = ((rand( ) % 100) - 50.0);
		p_pt->name = names[ rand() % (sizeof(names) - 1) ];

		pvector_push( &collection, p_pt );
	}

	for( i = 0; i < 0.25 * SIZE; i++ )
	{
		point_t *p_pt = pvector_peek( &collection );
		point_destroy( p_pt );

		pvector_pop( &collection );
	}


	for( i = 0; i < pvector_size(&collection); i++ )
	{
		point_t *p_pt = pvector_get( &collection, i );

		printf( "%c = (%3.0f, %3.0f)\n",
			p_pt->name,
			p_pt->x,
			p_pt->y );

		point_destroy( p_pt );
	}

	pvector_destroy( &collection );
	return 0;
}
