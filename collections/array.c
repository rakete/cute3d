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
#include "array.h"
#ifdef _DEBUG_VECTOR
#include <string.h>
#endif

boolean array_create( lc_array_t* p_array, size_t element_size, size_t size, alloc_function alloc, free_function free )
{
	assert( p_array );

	p_array->element_size = element_size;
	p_array->size         = size;
	p_array->alloc        = alloc;
	p_array->free         = free;
	p_array->arr          = p_array->alloc( array_element_size(p_array) * array_size(p_array) );

	#ifdef _DEBUG_VECTOR
	memset( p_array->arr, 0, array_element_size(p_array) * array_size(p_array) );
	#endif

	assert( p_array->arr );

	return p_array->arr != NULL;
}

void array_destroy( lc_array_t* p_array )
{
	assert( p_array );

	p_array->free( p_array->arr );

	#ifdef _DEBUG_VECTOR
	p_array->element_size = 0L;
	p_array->arr          = NULL;
	p_array->size         = 0L;
	#endif
}

boolean array_resize( lc_array_t* p_array, size_t new_size )
{
	boolean result = TRUE;

	if( array_size(p_array) != new_size )
	{
		p_array->size  = new_size;
		p_array->arr   = realloc( p_array->arr, p_array->element_size * array_size(p_array) );

		result = p_array->arr != NULL;
	}

	return result;
}

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L) /* Not C99 */
void* array_element( lc_array_t* p_array, size_t index )
{
	assert( index >= 0 );
	assert( index < array_size(p_array) );
	return (void*)(array_base(p_array) + array_element_size(p_array) * (index));
}
#endif

boolean array_serialize( lc_array_t* p_array, FILE* file, array_serialize_function func )
{
	if( fwrite( &p_array->size, sizeof(size_t), 1, file ) == 1 )
	{
		if( !func )
		{
			if( fwrite( p_array->arr, p_array->element_size, p_array->size, file ) == p_array->size )
			{
				return TRUE;
			}
		}
		else
		{
			/* User passed serialization function */
			return func( p_array->arr );
		}
	}

	return FALSE;
}

boolean array_unserialize( lc_array_t* p_array, FILE* file, array_unserialize_function func )
{
	size_t new_size = 0;

	if( fread( &new_size, sizeof(size_t), 1, file ) == 1 )
	{
		array_resize( p_array, new_size );

		if( !func )
		{
			if( fread( p_array->arr, p_array->element_size, p_array->size, file ) == p_array->size )
			{
				return TRUE;
			}
		}
		else
		{
			/* User passed unserialization function */
			return func( p_array->arr );
		}
	}

	return FALSE;
}

