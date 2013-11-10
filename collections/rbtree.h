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
#ifndef _RBTREE_H_
#define _RBTREE_H_
#ifdef __cplusplus
extern "C" {
#endif 

//#include "libcollections-config.h"
#include "types.h"
#include "alloc.h"

typedef int     (*rbtree_compare_function) ( const void *p_data_left, const void *p_data_right );
typedef boolean (*rbtree_element_function) ( void *p_data );


typedef struct lc_rbnode {   
	struct lc_rbnode *parent;
	struct lc_rbnode *left;
	struct lc_rbnode *right;
	boolean is_red;
	void *data;
} lc_rbnode_t;

#ifdef EXTERN_RBNIL
extern lc_rbnode RBNIL;
#endif

typedef struct lc_rbtree {
	lc_rbnode_t* root;
	size_t    size;	
	rbtree_compare_function _compare;
	rbtree_element_function _destroy;

	alloc_function  _alloc;
	free_function   _free;
} lc_rbtree_t;

typedef lc_rbnode_t* rbtree_iterator_t;


lc_rbtree_t* rbtree_create_ex   ( rbtree_element_function destroy, rbtree_compare_function compare, alloc_function alloc, free_function free );
void      rbtree_create      ( lc_rbtree_t* p_tree, rbtree_element_function destroy, rbtree_compare_function compare, alloc_function alloc, free_function free );
void      rbtree_destroy     ( lc_rbtree_t* p_tree );
void      rbtree_copy        ( lc_rbtree_t const *p_srcTree, lc_rbtree_t* p_dstTree );
boolean   rbtree_insert      ( lc_rbtree_t* p_tree, const void *data );
boolean   rbtree_remove      ( lc_rbtree_t* p_tree, const void *data );
boolean   rbtree_search      ( const lc_rbtree_t* p_tree, const void *data );
void      rbtree_clear       ( lc_rbtree_t* p_tree );
boolean   rbtree_serialize   ( lc_rbtree_t* p_tree, size_t element_size, FILE *file );
boolean   rbtree_unserialize ( lc_rbtree_t* p_tree, size_t element_size, FILE *file );

void    rbtree_alloc_set   ( lc_rbtree_t* p_tree, alloc_function alloc );
void    rbtree_free_set    ( lc_rbtree_t* p_tree, free_function free );

lc_rbnode_t* rbnode_minimum     ( lc_rbnode_t *t );
lc_rbnode_t* rbnode_maximum     ( lc_rbnode_t *t );
lc_rbnode_t* rbnode_successor   ( const lc_rbnode_t *t );
lc_rbnode_t* rbnode_predecessor ( const lc_rbnode_t *t );

rbtree_iterator_t rbtree_begin ( const lc_rbtree_t* p_tree );
rbtree_iterator_t rbtree_end   ( );

#ifdef _DEBUG_RBTREE 
boolean rbtree_verify_tree ( lc_rbtree_t* p_tree );
void    rbtree_print       ( const lc_rbtree_t* p_tree );
#endif

#define rbtree_size( p_tree )        ((p_tree)->size)
#define rbtree_is_empty( p_tree )    ((p_tree)->size <= 0)
#define rbtree_next( p_node )        (rbnode_successor( (p_node) ))
#define rbtree_previous( p_node )    (rbnode_predecessor( (p_node) ))
#define rbtree_root( p_tree )        ((p_tree)->root)

#ifdef __cplusplus
}
#endif 
#endif /* _RBTREE_H_ */
