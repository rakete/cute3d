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
#ifndef _DLIST_H_
#define _DLIST_H_
#ifdef __cplusplus
extern "C" {
#endif 

#include <stddef.h>
//#include "libcollections-config.h"
#include "types.h"
#include "alloc.h"

typedef boolean (*dlist_element_function)( void *element );

typedef struct lc_dlist_node {
	void* data;
	struct lc_dlist_node* next;	
	struct lc_dlist_node* prev;	
} lc_dlist_node_t;

typedef struct lc_dlist {
	lc_dlist_node_t* head;
	lc_dlist_node_t* tail;
	size_t size;
	dlist_element_function destroy;

	alloc_function  alloc;
	free_function   free;
} lc_dlist_t;

typedef lc_dlist_node_t* lc_dlist_iterator_t;

void    dlist_create        ( lc_dlist_t* p_list, dlist_element_function destroy_callback, alloc_function alloc, free_function free );
void    dlist_destroy       ( lc_dlist_t* p_list );
boolean dlist_insert_front  ( lc_dlist_t* p_list, const void *data ); /* O(1) */
boolean dlist_remove_front  ( lc_dlist_t* p_list ); /* O(1) */
boolean dlist_insert_back   ( lc_dlist_t* p_list, const void *data ); /* O(1) */
boolean dlist_remove_back   ( lc_dlist_t* p_list ); /* O(1) */
boolean dlist_insert_next   ( lc_dlist_t* p_list, lc_dlist_node_t* p_front_node, const void *data ); /* O(1) */
boolean dlist_remove_next   ( lc_dlist_t* p_list, lc_dlist_node_t* p_front_node ); /* O(1) */ 
void    dlist_clear         ( lc_dlist_t* p_list ); /* O(N) */

void    dlist_alloc_set     ( lc_dlist_t* p_list, alloc_function alloc );
void    dlist_free_set      ( lc_dlist_t* p_list, free_function free );

lc_dlist_iterator_t dlist_begin    ( const lc_dlist_t* p_list );
lc_dlist_iterator_t dlist_rbegin   ( const lc_dlist_t* p_list );
#define          dlist_end( )   ((lc_dlist_iterator_t)NULL)
lc_dlist_iterator_t dlist_next     ( const lc_dlist_iterator_t iter );
lc_dlist_iterator_t dlist_previous ( const lc_dlist_iterator_t iter );

#define dlist_push               dlist_insert_back
#define dlist_pop                dlist_remove_front

#define dlist_head(p_list)       ((p_list)->head)
#define dlist_front(p_list)      ((p_list)->head)
#define dlist_tail(p_list)       ((p_list)->tail)
#define dlist_back(p_list)       ((p_list)->tail)
#define dlist_size(p_list)       ((p_list)->size)
#define dlist_is_empty(p_list)   ((p_list)->size <= 0)

#ifdef __cplusplus
}
#endif 
#endif /* _DLIST_H_ */
