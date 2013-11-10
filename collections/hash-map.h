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
#ifndef _HASH_MAP_H_
#define _HASH_MAP_H_
#ifdef __cplusplus
extern "C" {
#endif 

#include <stddef.h>
#include "types.h"
#include "alloc.h"

/* Each size is a prime number */
#ifndef HASH_MAP_SIZE_SMALL 
#define HASH_MAP_SIZE_SMALL         (101)
#endif
#ifndef HASH_MAP_SIZE_MEDIUM 
#define HASH_MAP_SIZE_MEDIUM        (1031)
#endif
#ifndef HASH_MAP_SIZE_LARGE
#define HASH_MAP_SIZE_LARGE         (5153)
#endif
#ifndef HASH_MAP_SIZE_EXTRA_LARGE
#define HASH_MAP_SIZE_EXTRA_LARGE   (7919)
#endif

#ifndef HASH_MAP_DEFAULT_LOAD_FACTOR
#define HASH_MAP_DEFAULT_LOAD_FACTOR     (0.7)
#endif

#ifndef HASH_MAP_THRESHOLD
#define HASH_MAP_THRESHOLD               (0.7)
#endif



typedef size_t  (*hash_map_hash_function)    ( const void *key );
typedef boolean (*hash_map_element_function) ( void *key, void *value );
typedef int     (*hash_map_compare_function) ( const void* __restrict left, const void* __restrict right );

struct lc_hash_map_node;
typedef struct lc_hash_map_node lc_hash_map_node_t;

struct lc_hash_map_list;
typedef struct lc_hash_map_list lc_hash_map_list_t;

typedef struct lc_hash_map {
	size_t           size;
	size_t           table_size;
	lc_hash_map_list_t* table;

	hash_map_hash_function    hash;
	hash_map_compare_function compare; 	
	hash_map_element_function destroy; 	

	alloc_function  alloc;
	free_function   free;
} lc_hash_map_t;

boolean   hash_map_create      ( lc_hash_map_t *p_map, size_t table_size, 
                                 hash_map_hash_function hash_function, hash_map_element_function destroy, 
                                 hash_map_compare_function compare, 
                                 alloc_function alloc, free_function free );
void      hash_map_destroy     ( lc_hash_map_t* p_map );
boolean   hash_map_insert      ( lc_hash_map_t* __restrict p_map, const void* __restrict key, const void* __restrict value );
boolean   hash_map_remove      ( lc_hash_map_t* __restrict p_map, const void* __restrict key );
boolean   hash_map_find        ( const lc_hash_map_t* __restrict p_map, const void* __restrict key, void ** __restrict value );
void      hash_map_clear       ( lc_hash_map_t *p_map );
boolean   hash_map_resize      ( lc_hash_map_t *p_map, size_t new_size );
boolean   hash_map_rehash      ( lc_hash_map_t *p_map, float load_factor );
boolean   hash_map_serialize   ( lc_hash_map_t *p_map, size_t key_size, size_t value_size, FILE *file );
boolean   hash_map_unserialize ( lc_hash_map_t *p_map, size_t key_size, size_t value_size, FILE *file );
void      hash_map_alloc_set   ( lc_hash_map_t *p_map, alloc_function alloc );
void      hash_map_free_set    ( lc_hash_map_t *p_map, free_function free );

#define   hash_map_size(p_map)         ((p_map)->size)
#define   hash_map_table_size(p_map)   ((p_map)->table_size)
#define   hash_map_load_factor(p_map)  (hash_map_size(p_map) / ((float) hash_map_table_size(p_map)))


typedef struct lc_hash_map_iter {
	const lc_hash_map_t* map;
	size_t            index;
	lc_hash_map_node_t*  current;
	void*             key;
	void*             value;
} lc_hash_map_iterator_t;

void    hash_map_iterator      ( const lc_hash_map_t* p_map, lc_hash_map_iterator_t* iter );
boolean hash_map_iterator_next ( lc_hash_map_iterator_t* iter );
void*   hash_map_iterator_key  ( lc_hash_map_iterator_t* iter );
void*   hash_map_iterator_value( lc_hash_map_iterator_t* iter );

#ifdef __cplusplus
}
#endif 
#endif /* _HASH_MAP_H_ */
