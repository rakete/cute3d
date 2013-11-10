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
#ifndef _BHEAP_TEMPLATE_H_
#define _BHEAP_TEMPLATE_H_
#ifdef __cplusplus
extern "C" {
#endif 


#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "types.h"



#define DECLARE_BHEAP_TYPE( name, type ) \
	typedef boolean (*bheap_##name##_serialize_function)   ( type *p_array ); \
	typedef boolean (*bheap_##name##_unserialize_function) ( type *p_array ); \
	typedef int     (*bheap_##name##_compare_function)     ( const type* __restrict p_data_left, const type* __restrict p_data_right ); \
	\
	typedef struct bheap##name {\
		bheap_##name##_compare_function compare;\
		size_t array_size; \
		size_t size; \
		type*  array; \
	} bheap_##name##_t; \
	\
	boolean    bheap_##name##_create     ( bheap_##name##_t* p_bheap, size_t size, bheap_##name##_compare_function compare ); \
	void       bheap_##name##_destroy    ( bheap_##name##_t* p_bheap ); \
	type*      bheap_##name##_peek       ( bheap_##name##_t* p_bheap ); \
	boolean    bheap_##name##_push       ( bheap_##name##_t* __restrict p_bheap, const type* __restrict data ); \
	boolean    bheap_##name##_pop        ( bheap_##name##_t* p_bheap ); \
	void       bheap_##name##_reheapify  ( bheap_##name##_t* p_bheap ); \
 	\
	void       heap_##name##_make        ( type* array, size_t size, bheap_##name##_compare_function compare ); \
	void       heap_##name##_push        ( type* array, size_t size, bheap_##name##_compare_function compare ); \
	void       heap_##name##_pop         ( type* array, size_t size, bheap_##name##_compare_function compare ); \
	\
	static __inline size_t bheap_##name##_size( bheap_##name##_t* p_bheap ) \
	{ \
		assert( p_bheap ); \
		return p_bheap->size; \
	} \
	\
	static __inline void bheap_##name##_clear( bheap_##name##_t* p_bheap ) \
	{ \
		assert( p_bheap ); \
		p_bheap->size = 0;  \
	} 


#define IMPLEMENT_BHEAP_TYPE( name, type ) \
	static __inline void heapify_##name( type* array, size_t size, bheap_##name##_compare_function compare, size_t index ); \
	\
	boolean bheap_##name##_create( bheap_##name##_t* p_bheap, size_t size, bheap_##name##_compare_function compare ) \
	{ \
		assert( p_bheap ); \
		p_bheap->compare      = compare; \
		p_bheap->array_size   = size; \
		p_bheap->size         = 0L; \
		p_bheap->array        = (type*) malloc( sizeof(type) * (p_bheap->array_size) ); \
		assert( p_bheap->array ); \
		return p_bheap->array != NULL; \
	}\
	\
	void bheap_##name##_destroy( bheap_##name##_t* p_bheap ) \
	{ \
		assert( p_bheap ); \
		free( p_bheap->array ); \
	} \
	\
	type* bheap_##name##_peek( bheap_##name##_t* p_bheap ) \
	{ \
		assert( p_bheap && p_bheap->size > 0 ); \
		return &p_bheap->array[ 0 ]; \
	} \
	\
	boolean bheap_##name##_push( bheap_##name##_t* p_bheap, const type* data ) \
	{ \
		boolean result = FALSE; \
		assert( p_bheap ); \
		/* grow the array if needed */ \
		if( p_bheap->size >= p_bheap->array_size ) \
		{ \
			size_t new_size     = 2 * p_bheap->array_size + 1; \
			p_bheap->array_size = new_size; \
			p_bheap->array      = realloc( p_bheap->array, sizeof(type) * p_bheap->array_size ); \
			\
			result = p_bheap->array != NULL; \
		} \
		p_bheap->array[ p_bheap->size ] = *data; \
		p_bheap->size++; \
		heap_##name##_push( p_bheap->array, p_bheap->size, p_bheap->compare ); \
		return result; \
	} \
	\
	boolean bheap_##name##_pop( bheap_##name##_t* p_bheap ) \
	{ \
		boolean result = FALSE; \
		assert( p_bheap ); \
		if( p_bheap->size > 1 ) \
		{ \
			p_bheap->array[ 0 ] = p_bheap->array[ p_bheap->size - 1 ]; \
		} \
		if( p_bheap->size > 0 ) \
		{ \
			p_bheap->size--; \
		} \
		heap_##name##_pop( p_bheap->array, p_bheap->size, p_bheap->compare ); \
		return result; \
	} \
	\
	void bheap_##name##_reheapify( bheap_##name##_t* p_bheap ) \
	{ \
		heap_##name##_make( p_bheap->array, p_bheap->array_size, p_bheap->compare ); \
	} \
	\
	void heap_##name##_make( type* array, size_t size, bheap_##name##_compare_function compare ) \
	{ \
		ssize_t index = parent_of( size - 1 ); \
		while( index >= 0 ) \
		{ \
			heapify_##name( array, size, compare, index ); \
			index--; \
		} \
	} \
	\
	void heap_##name##_push( type* array, size_t size, bheap_##name##_compare_function compare ) \
	{ \
		boolean done = FALSE; \
		size_t index = size - 1; \
		while( !done ) \
		{ \
			size_t parent_index = parent_of( index ); \
			assert( index >= 0 ); \
			assert( parent_index >= 0 ); \
			if( compare( &array[ parent_index ], &array[ index ] ) < 0 ) \
			{ \
				type tmp = array[ parent_index ]; \
				array[ parent_index ] = array[ index ]; \
				array[ index ] = tmp; \
				index = parent_index; \
			} \
			else \
			{ \
				done = TRUE; \
			} \
		} \
	} \
	\
	void heap_##name##_pop( type* array, size_t size, bheap_##name##_compare_function compare ) \
	{ \
		heapify_##name( array, size, compare, 0 ); \
	} \
	\
	void heapify_##name( type* array, size_t size, bheap_##name##_compare_function compare, size_t index ) \
	{ \
		boolean done = FALSE; \
		while( !done && index < size ) \
		{ \
			size_t left_index  = left_child_of( index ); \
			size_t right_index = right_child_of( index ); \
			size_t optimal_idx = index; \
			if( left_index < size && compare( &array[ optimal_idx ], &array[ left_index ] ) < 0 ) \
			{ \
				optimal_idx = left_index; \
			} \
			if( right_index < size && compare( &array[ optimal_idx ], &array[ right_index ] ) < 0 ) \
			{ \
				optimal_idx = right_index; \
			} \
			if( optimal_idx != index ) \
			{ \
				type tmp = array[ index ]; \
				array[ index ] = array[ optimal_idx ]; \
				array[ optimal_idx ] = tmp; \
				index = optimal_idx; \
			} \
			else \
			{ \
				done = TRUE; \
			} \
		} \
	} 


#ifdef __cplusplus
}
#endif 
#endif /* _BHEAP_TEMPLATE_H_ */
