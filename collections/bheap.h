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
#ifndef _BIN_HEAP_H_
#define _BIN_HEAP_H_
#ifdef __cplusplus
extern "C" {
#endif 
#include "types.h"
#include "vector.h"
#include "alloc.h"
#define parent_of( index )         ((index) >> 1)       /* index / 2 */
#define left_child_of( index )     (((index) << 1) + 0) /* 2 * index */
#define right_child_of( index )    (((index) << 1) + 1) /* 2 * index + 1 */
#include "bheap-template.h"

typedef boolean (*heap_serialize_function)   ( void *p_array );
typedef boolean (*heap_unserialize_function) ( void *p_array );
typedef boolean (*heap_element_function)     ( void *data );
typedef int     (*heap_compare_function)     ( const void *p_data_left, const void *p_data_right );


typedef struct bheap {
	alloc_function  alloc;
	free_function   free;

	heap_compare_function compare;
	lc_vector_t  heap;
	byte*     tmp;
} bheap_t;


boolean         bheap_create      ( bheap_t* p_bheap, size_t element_size, size_t size, 
                                    heap_compare_function compare_callback, heap_element_function destroy_callback,
                                    alloc_function alloc, free_function free );
void            bheap_destroy     ( bheap_t* p_bheap );
void*           bheap_peek        ( bheap_t* p_bheap );
boolean         bheap_push        ( bheap_t* p_bheap, void* data );
boolean         bheap_pop         ( bheap_t* p_bheap );
size_t          bheap_size        ( const bheap_t* p_bheap );
void            bheap_clear       ( bheap_t* p_bheap );
void            bheap_reheapify   ( bheap_t* p_bheap );

void            heap_make         ( lc_vector_t* heap, heap_compare_function compare, void* swap_buffer );
void            heap_push         ( lc_vector_t* heap, heap_compare_function compare, void* swap_buffer );
void            heap_pop          ( lc_vector_t* heap, heap_compare_function compare, void* swap_buffer );


/*
 *  Pointer Binary Heap
 */
typedef struct pbheap {
	heap_compare_function compare;
	lc_pvector_t  heap;
} pbheap_t;

boolean         pbheap_create      ( pbheap_t* p_bheap, size_t size, heap_compare_function compare_callback,
                                     alloc_function alloc, free_function free );
void            pbheap_destroy     ( pbheap_t* p_bheap );
void*           pbheap_peek        ( pbheap_t* p_bheap );
boolean         pbheap_push        ( pbheap_t* p_bheap, void* data );
boolean         pbheap_pop         ( pbheap_t* p_bheap );
size_t          pbheap_size        ( const pbheap_t* p_bheap );
void            pbheap_clear       ( pbheap_t* p_bheap );
void            pbheap_reheapify   ( pbheap_t* p_bheap );

void            pheap_make         ( lc_pvector_t* heap, heap_compare_function compare );
void            pheap_push         ( lc_pvector_t* heap, heap_compare_function compare );
void            pheap_pop          ( lc_pvector_t* heap, heap_compare_function compare );


#ifdef __cplusplus
}
#endif 
#endif /* _BIN_HEAP_H_ */
