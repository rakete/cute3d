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
#ifndef _VECTOR_H_
#define _VECTOR_H_
#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stddef.h>
#include <string.h>
//#include "libcollections-config.h"
#include "types.h"
#include "alloc.h"
#include "vector-template.h"

#ifndef VECTOR_INITIAL_ARRAY_SIZE
#define VECTOR_INITIAL_ARRAY_SIZE    250
#endif

#ifndef VECTOR_GROWTH_FACTOR
#if 1
#define VECTOR_GROWTH_FACTOR	1.5
#else
#define VECTOR_GROWTH_FACTOR	2
#endif
#endif

typedef boolean (*vector_serialize_function)   ( void *p_array );
typedef boolean (*vector_unserialize_function) ( void *p_array );
typedef boolean (*vector_element_function)     ( void *data );

typedef struct lc_vector {
	alloc_function  alloc;
	free_function   free;
	size_t element_size;
	size_t array_size;
	size_t size;
	#if defined(VECTOR_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
	vector_element_function destroy;
	#endif

	byte*  array;
} lc_vector_t;

/*
 * vector - A growable array of elements.
 */
#if defined(VECTOR_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
boolean      vector_create      ( lc_vector_t *p_vector, size_t element_size, 
                                  size_t size, vector_element_function destroy_callback, 
                                  alloc_function alloc, free_function free );
#else
boolean      vector_create      ( lc_vector_t *p_vector, size_t element_size, 
                                  size_t size, alloc_function alloc, free_function free );
#endif
void         vector_destroy     ( lc_vector_t *p_vector );
void*        vector_pushx       ( lc_vector_t *p_vector );
boolean      vector_push        ( lc_vector_t *p_vector, void *data );
boolean      vector_pop         ( lc_vector_t *p_vector );
boolean      vector_resize      ( lc_vector_t *p_vector, size_t new_size );
void         vector_clear       ( lc_vector_t *p_vector );
boolean      vector_serialize   ( lc_vector_t *p_vector, FILE *file, vector_serialize_function func );
boolean      vector_unserialize ( lc_vector_t *p_vector, FILE *file, vector_unserialize_function func );

#define vector_element_size( p_vector )  ((p_vector)->element_size)
#define vector_array( p_vector )         ((p_vector)->array)
#define vector_array_size( p_vector )    ((p_vector)->array_size)
#define vector_size( p_vector )          ((p_vector)->size)
#define vector_is_empty( p_vector )      ((p_vector)->size <= 0)
#define vector_peek( p_vector )          (vector_get(p_vector, vector_size(p_vector) - 1))

#if (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)) || defined(_DEBUG_VECTOR)
#include <string.h>
#include <assert.h>

#define vector_get( p_vector, index ) \
	(void*)(vector_array(p_vector) + vector_element_size(p_vector) * (index))

#define vector_set( p_vector, index, p_data ) \
	memcpy( vector_array(p_vector) + (vector_size(p_vector) * vector_element_size(p_vector)), p_data, vector_element_size(p_vector) )

#else
#include <assert.h>
static __inline void* vector_get( lc_vector_t *p_vector, size_t index )
{
	assert( index >= 0 );
	assert( index < vector_size(p_vector) );
	return (void*)(vector_array(p_vector) + vector_element_size(p_vector) * (index));
}

static __inline void vector_set( lc_vector_t *p_vector, size_t index, void *data )
{
	assert( data != NULL );
	assert( index >= 0 );
	assert( index < vector_size(p_vector) );
	memcpy( vector_array(p_vector) + (vector_size(p_vector) * vector_element_size(p_vector)), data, vector_element_size(p_vector) );
}
#endif


/*
 * pvector - A growable array of pointers.
 * pvector does not own the pointers.
 */
typedef struct pvector {
	alloc_function  alloc;
	free_function   free;
	size_t array_size;
	size_t size;

	void** array;
} lc_pvector_t;

boolean      pvector_create     ( lc_pvector_t *p_vector, size_t size, alloc_function alloc, free_function free );
void         pvector_destroy    ( lc_pvector_t *p_vector );
boolean      pvector_push       ( lc_pvector_t *p_vector, void *data );
boolean      pvector_pop        ( lc_pvector_t *p_vector );
boolean      pvector_resize     ( lc_pvector_t *p_vector, size_t new_size );
void         pvector_clear      ( lc_pvector_t *p_vector );

#define pvector_array( p_vector )       ((p_vector)->array)
#define pvector_array_size( p_vector )  ((p_vector)->array_size)
#define pvector_size( p_vector )        ((p_vector)->size)
#define pvector_is_empty( p_vector )    ((p_vector)->size <= 0)
#define pvector_peek( p_vector )        (pvector_get(p_vector, pvector_size(p_vector) - 1))

#if (!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)) || defined(_DEBUG_VECTOR)
#include <assert.h>

#define pvector_get( p_vector, index ) \
	(p_vector)->array[ index ]

#define pvector_set( p_vector, index, p_data ) \
	(p_vector)->array[ index ] = (p_data)

#else 
#include <assert.h>
static __inline void* pvector_get( lc_pvector_t *p_vector, size_t index )
{
	assert( index >= 0 );
	assert( index < pvector_size(p_vector) );
	return p_vector->array[ index ];
}

static __inline void pvector_set( lc_pvector_t *p_vector, size_t index, void *data )
{
	assert( p_vector && data );
	assert( index >= 0 );
	assert( index < pvector_size(p_vector) );
	p_vector->array[ index ] = data;
}
#endif

/*
 * stack - A first in, last out data structure.
 */
typedef lc_vector_t lc_stack_t;

#define stack_create         vector_create   
#define stack_destroy        vector_destroy 
#define stack_push           vector_push    
#define stack_pop            vector_pop     
#define stack_element_size   vector_element_size 
#define stack_array          vector_array
#define stack_array_size     vector_array_size
#define stack_size           vector_size
#define stack_is_empty       vector_is_empty
#define stack_peek           vector_peek

typedef lc_pvector_t lc_pstack_t;

#define pstack_create         pvector_create   
#define pstack_destroy        pvector_destroy 
#define pstack_push           pvector_push    
#define pstack_pop            pvector_pop     
#define pstack_element_size   pvector_element_size 
#define pstack_array          pvector_array
#define pstack_array_size     pvector_array_size
#define pstack_size           pvector_size
#define pstack_is_empty       pvector_is_empty
#define pstack_peek           pvector_peek

#ifdef __cplusplus
}
#endif 
#endif /* _VECTOR_H_ */
