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
#include "rbtree.h"

#ifdef EXTERN_RBNIL
	/* Typical leaf node (always black).
	 * This cannot be const because the parent pointer is manipulated.
	 */
	lc_rbnode_t RBNIL = { (lc_rbnode_t *) &RBNIL, (lc_rbnode_t *) &RBNIL, (lc_rbnode_t *) &RBNIL, FALSE, NULL };
#else
	/* Typical leaf node (always black) */
	static lc_rbnode_t RBNIL = { (lc_rbnode_t *) &RBNIL, (lc_rbnode_t *) &RBNIL, (lc_rbnode_t *) &RBNIL, FALSE, NULL };
#endif

#if defined(RBTREE_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
	#define DESTROY_CHECK( code ) \
		if( p_tree->_destroy ) \
		{ \
			code \
		}
#else
	#define DESTROY_CHECK( code ) \
		code
#endif


#define nil_init( p_node ) \
	(p_node)->parent = p_node; \
	(p_node)->left   = p_node; \
	(p_node)->right  = p_node; \
	(p_node)->data   = NULL; \
	(p_node)->is_red = FALSE;

#define rbnode_init( p_node, p_data, p_parent, p_left, p_right, color ) \
	(p_node)->parent = p_parent; \
	(p_node)->left   = p_left; \
	(p_node)->right  = p_right; \
	(p_node)->data   = (void *) p_data; \
	(p_node)->is_red = color;


static lc_rbnode_t* rbnode_search       ( lc_rbtree_t* p_tree, const void *key );


static __inline void rbtree_left_rotate( lc_rbtree_t* p_tree, lc_rbnode_t *x )
{
	lc_rbnode_t *y;
	assert( x->right != &RBNIL );

	y        = x->right;
	x->right = y->left;

	if( y->left != &RBNIL )
	{
		y->left->parent = x;
	}

	y->parent = x->parent;

	if( x->parent == &RBNIL )
	{
		p_tree->root = y;
	}
	else if( x == x->parent->left )
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->left   = x;
	x->parent = y;
}

static __inline void rbtree_right_rotate( lc_rbtree_t* p_tree, lc_rbnode_t *x )
{
	lc_rbnode_t *y;
	assert( x->left != &RBNIL );

	y       = x->left;
	x->left = y->right;

	if( y->right != &RBNIL )
	{
		y->right->parent = x;
	}

	y->parent = x->parent;

	if( x->parent == &RBNIL )
	{
		p_tree->root = y;
	}
	else if( x == x->parent->right ) {
		x->parent->right = y;
	}
	else
	{
		x->parent->left = y;
	}

	y->right  = x;
	x->parent = y;
}

static __inline void rbtree_insert_fixup( lc_rbtree_t* p_tree, lc_rbnode_t ** t )
{
	lc_rbnode_t *y = (lc_rbnode_t *) &RBNIL;

	while( (*t)->parent->is_red )
	{
		if( (*t)->parent == (*t)->parent->parent->left ) /* is parent on left side of grandparent... */
		{
			y = (*t)->parent->parent->right; /* y is uncle */
			/* case 1 */
			if( y->is_red ) /* uncle is red, just recolor */
			{
				(*t)->parent->is_red = FALSE;
				y->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				(*t) = (*t)->parent->parent;
			}
			else
			{
				/* case 2 - uncle is black and t is on the right */
				if( (*t) == (*t)->parent->right )
				{
					(*t) = (*t)->parent;
					rbtree_left_rotate( p_tree, (*t) );
				}
				/* case 3 - uncle is black and t is on the left */
				(*t)->parent->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				rbtree_right_rotate( p_tree, (*t)->parent->parent );
			}
		}
		else  /* parent is on right side of grandparent... */
		{
			y = (*t)->parent->parent->left; /* y is uncle */
			/* case 1 */
			if( y->is_red ) /* uncle is red */
			{
				(*t)->parent->is_red = FALSE;
				y->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				(*t) = (*t)->parent->parent;
			}
			else
			{
				/* case 2 - uncle is black and t is on the left */
				if( (*t) == (*t)->parent->left )
				{
					(*t) = (*t)->parent;
					rbtree_right_rotate( p_tree, (*t) );
				}
				/* case 3 - uncle is black and t is on the right */
				(*t)->parent->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				rbtree_left_rotate( p_tree, (*t)->parent->parent );
			}
		}
	}

	p_tree->root->is_red = FALSE;
}

static __inline void rbtree_delete_fixup( lc_rbtree_t* p_tree, lc_rbnode_t ** t )
{
	lc_rbnode_t *w = (lc_rbnode_t *) &RBNIL;

	while( (*t) != p_tree->root && (*t)->is_red == FALSE )
	{
		if( (*t) == (*t)->parent->left )
		{
			w = (*t)->parent->right;

			if( w->is_red )
			{
				w->is_red            = FALSE;
				(*t)->parent->is_red = TRUE;
				rbtree_left_rotate( p_tree, (*t)->parent );
				w = (*t)->parent->right;
			}

			if( w->left->is_red == FALSE && w->right->is_red == FALSE )
			{
				w->is_red = TRUE;
				(*t) = (*t)->parent;
			}
			else {
				if( w->right->is_red == FALSE )
				{
					w->left->is_red = FALSE;
					w->is_red       = TRUE;
					rbtree_right_rotate( p_tree, w );
					w = (*t)->parent->right;
				}

				w->is_red            = (*t)->parent->is_red;
				(*t)->parent->is_red = FALSE;
				w->right->is_red     = FALSE;
				rbtree_left_rotate( p_tree, (*t)->parent );
				(*t) = p_tree->root;
			}
		}
		else
		{
			w = (*t)->parent->left;

			if( w->is_red )
			{
				w->is_red = FALSE;
				(*t)->parent->is_red = TRUE;
				rbtree_right_rotate( p_tree, (*t)->parent );
				w = (*t)->parent->left;
			}

			if( w->right->is_red == FALSE && w->left->is_red == FALSE )
			{
				w->is_red = TRUE;
				(*t) = (*t)->parent;
			}
			else
			{
				if( w->left->is_red == FALSE )
				{
					w->right->is_red = FALSE;
					w->is_red = TRUE;
					rbtree_left_rotate( p_tree, w );
					w = (*t)->parent->left;
				}

				w->is_red = (*t)->parent->is_red;
				(*t)->parent->is_red = FALSE;
				w->left->is_red = FALSE;
				rbtree_right_rotate( p_tree, (*t)->parent );
				(*t) = p_tree->root;
			}
		}
	}

	(*t)->is_red = FALSE;
}

lc_rbnode_t *rbnode_minimum( lc_rbnode_t *t )
{
	while( t->left != &RBNIL ) { t = t->left; }
	return t;
}

lc_rbnode_t *rbnode_maximum( lc_rbnode_t *t )
{
	while( t->right != &RBNIL ) { t = t->right; }
	return t;
}

lc_rbnode_t *rbnode_successor( const lc_rbnode_t *t )
{
	lc_rbnode_t *y;

	if( t->right != &RBNIL )
	{
		return rbnode_minimum( t->right );
	}

	y = t->parent;

	while( y != &RBNIL && t == y->right )
	{
		t = y;
		y = y->parent;
	}

	return y;
}

lc_rbnode_t *rbnode_predecessor( const lc_rbnode_t *t )
{
	lc_rbnode_t *y;

	if( t->left != &RBNIL )
	{
		return rbnode_maximum( t->left );
	}

	y = t->parent;

	while( y != &RBNIL && t == y->left )
	{
		t = y;
		y = y->parent;
	}

	return y;
}

lc_rbtree_t* rbtree_create_ex( rbtree_element_function destroy, rbtree_compare_function compare, alloc_function alloc, free_function free )
{
	lc_rbtree_t* p_tree = (lc_rbtree_t*)  malloc( sizeof(lc_rbtree_t) );

	if( p_tree )
	{
		rbtree_create( p_tree, destroy, compare, alloc, free );
	}

	return p_tree;
}

void rbtree_create( lc_rbtree_t* p_tree, rbtree_element_function destroy, rbtree_compare_function compare, alloc_function alloc, free_function free )
{
	assert( p_tree );
	#ifndef RBTREE_CHECK_FOR_DESTROY
	assert( destroy );
	#endif
	assert( compare );
	p_tree->root     = (lc_rbnode_t *) &RBNIL;
	p_tree->size     = 0;
	p_tree->_compare = compare;
	p_tree->_destroy = destroy;


	p_tree->_alloc = malloc;
	p_tree->_free  = free;
}

void rbtree_destroy( lc_rbtree_t* p_tree )
{
	assert( p_tree );
	rbtree_clear( p_tree );

	#ifdef _DEBUG_RBTREE
	p_tree->root     = NULL;
	p_tree->size     = 0;
	p_tree->_compare = NULL;
	p_tree->_destroy = NULL;
	#endif
}

/*
void rbtree_copy( lc_rbtree_t const *p_srcTree, lc_rbtree_t* p_dstTree )
{
	lc_rbnode_t *p_node = NULL;

	if( p_srcTree != p_dstTree )
	{
		rbtree_clear( p_dstTree );

		p_dstTree->_compare = p_srcTree->compare;
		p_dstTree->_destroy = p_srcTree->destroy;

		// insert all of the nodes into this tree; m_NumberOfNodes should be equal after this...
		for( p_node = rbtree_minimum(p_srcTree); p_node != &RBNIL; p_node = rbnode_successor(p_node) )
		{
			rbtree_insert( p_dstTree, p_node->data );
		}
	}
}
*/

boolean rbtree_insert( lc_rbtree_t* p_tree, const void *key )
{

	lc_rbnode_t *y;
	lc_rbnode_t *x;
	lc_rbnode_t *newNode;

	assert( p_tree );
	y   = (lc_rbnode_t *) &RBNIL;
	x   = p_tree->root;
	newNode = p_tree->_alloc( sizeof(lc_rbnode_t) );
	if( !newNode ) return FALSE;

	/* Find where to insert the new node--y points the parent. */
	while( x != &RBNIL )
	{
		y = x;
		if( p_tree->_compare( key, x->data ) < 0 )
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}

	/* Insert the new node. */
	if( y == &RBNIL )
	{
		p_tree->root = newNode;
	}
	else
	{
		if( p_tree->_compare( key, y->data ) < 0 )
		{
			y->left = newNode;
		}
		else
		{
			y->right = newNode;
		}
	}

	rbnode_init( newNode, key, y, (lc_rbnode_t *) &RBNIL, (lc_rbnode_t *) &RBNIL, TRUE );
	rbtree_insert_fixup( p_tree, &newNode );
	p_tree->size++;

	return TRUE;
}

boolean rbtree_remove( lc_rbtree_t* p_tree, const void *key )
{
	lc_rbnode_t *t;
	lc_rbnode_t *x;
	lc_rbnode_t *y;

	boolean y_is_red = FALSE;

	assert( p_tree );

	t = rbnode_search( p_tree, key );

	if( t == NULL ) return FALSE; /* item is not even in the tree! */

	if( t->left == &RBNIL || t->right == &RBNIL )
	{
		/* t has less than two children and can be deleted directly. */
		y = t;
	}
	else  /* t has two children... */
	{
		/* t has two children and must be switched out with it's
		 * successor. We will free the successor node and t's data.
		 */
		y = rbnode_successor( t );
	}

	if( y->left != &RBNIL )
	{
		x = y->left;
	}
	else
	{
		x = y->right;
	}

	x->parent = y->parent;

	if( y->parent == &RBNIL )
	{
		p_tree->root = x;
	}
	else
	{
		if( y == y->parent->left )
		{
			y->parent->left = x;
		}
		else
		{
			y->parent->right = x;
		}
	}

	if( y != t )
	{
		DESTROY_CHECK(
			p_tree->_destroy( t->data );
		);

		t->data = y->data;
		y_is_red = y->is_red;
		p_tree->_free( y );
	}
	else
	{
		DESTROY_CHECK(
			p_tree->_destroy( y->data );
		);

		y_is_red = y->is_red;
		p_tree->_free( y );
	}

	if( y_is_red == FALSE ) /* y is black */
	{
		rbtree_delete_fixup( p_tree, &x );
	}

	p_tree->size--;

	return TRUE;
}

boolean rbtree_search( const lc_rbtree_t* p_tree, const void *key )
{
	lc_rbnode_t *x;
	assert( p_tree );

	x = p_tree->root;

	while( x != &RBNIL )
	{
		if( p_tree->_compare( key, x->data ) == 0 )
		{
			return TRUE; /* found it */
		}
		else if( p_tree->_compare( key, x->data ) < 0 )
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}

	return FALSE;
}

void rbtree_clear( lc_rbtree_t* p_tree )
{
	lc_rbnode_t *x;
	lc_rbnode_t *y;

	assert( p_tree );

	x = p_tree->root;
	y = (lc_rbnode_t *) &RBNIL;

	while( p_tree->size > 0 )
	{
		y = x;

		/* find minimum... */
		while( y->left != &RBNIL )
		{
			y = y->left;
		}

		if( y->right == &RBNIL )
		{
			y->parent->left = (lc_rbnode_t *) &RBNIL;
			x = y->parent;

			/* free... */
			DESTROY_CHECK(
				p_tree->_destroy( y->data );
			);

			p_tree->_free( y );

			p_tree->size--;
		}
		else
		{
			x = y->right;
			x->parent = y->parent;

			if( x->parent == &RBNIL )
			{
				p_tree->root = x;
			}
			else
			{
				y->parent->left = x;
			}
			/* free... */
			DESTROY_CHECK(
				p_tree->_destroy( y->data );
			);

			p_tree->_free( y );

			p_tree->size--;
		}

		#ifdef _DEBUG_RBTREE
		y->data = NULL;
		#endif
	}

	/* reset the root and current pointers */
	p_tree->root = (lc_rbnode_t *) &RBNIL;
}

/* ------------------------------------- */

lc_rbnode_t *rbnode_search( lc_rbtree_t* p_tree, const void *key )
{
	lc_rbnode_t *x;

	assert( p_tree );
	x = p_tree->root;

	while( x != &RBNIL )
	{
		if( p_tree->_compare( key, x->data ) == 0 )
		{
			return x;
		}
		else if( p_tree->_compare( key, x->data ) < 0 )
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}

	return NULL;
}

rbtree_iterator_t rbtree_begin( const lc_rbtree_t* p_tree )
{
	assert( p_tree );
    return rbnode_minimum( p_tree->root );
}

rbtree_iterator_t rbtree_end( )
{
	return (lc_rbnode_t *) &RBNIL;
}

boolean rbtree_serialize( lc_rbtree_t* p_tree, size_t element_size, FILE *file )
{
	boolean result = TRUE;
	size_t count;
	rbtree_iterator_t itr;

	assert( p_tree );

	if( !p_tree )
	{
		result = FALSE;
		goto done;
	}

	count = rbtree_size(p_tree);
	if( fwrite( &count, sizeof(size_t), 1, file ) != 1 )
	{
		result = FALSE;
		goto done;
	}


	for( itr = rbtree_begin( p_tree );
	     itr != rbtree_end( ) && !ferror(file);
	     itr = rbtree_next( itr ) )
	{
		if( fwrite( itr->data, element_size, 1, file ) != 1 )
		{
			result = FALSE;
			goto done;
		}
	}

done:
	return result;
}

boolean rbtree_unserialize( lc_rbtree_t* p_tree, size_t element_size, FILE *file )
{
	boolean result = TRUE;
	size_t count = 0;

	assert( p_tree );

	if( !p_tree )
	{
		result = FALSE;
		goto done;
	}

	if( fread( &count, sizeof(size_t), 1, file ) != 1 )
	{
		result = FALSE;
		goto done;
	}

	while( count > 0 && feof(file) == 0 )
	{
		void *p_data = p_tree->_alloc( element_size );

		if( fread( p_data, element_size, 1, file ) != 1 )
		{
			p_tree->_free( p_data );

			result = FALSE;
			goto done;
		}

		rbtree_insert( p_tree, p_data );
		count--;
	}

done:
	return result;
}


void rbtree_alloc_set( lc_rbtree_t* p_tree, alloc_function alloc )
{
	assert( p_tree );
	assert( alloc );
	p_tree->_alloc = alloc;
}

void rbtree_free_set( lc_rbtree_t* p_tree, free_function free )
{
	assert( p_tree );
	assert( free );
	p_tree->_free = free;
}


#ifdef _DEBUG_RBTREE
static boolean rbnode_verify_tree  ( lc_rbtree_t* p_tree, lc_rbnode_t *t );
static void    padding             ( char ch, int n );
static void    structure           ( const lc_rbnode_t *root, int level );

boolean rbtree_verify_tree( lc_rbtree_t* p_tree )
{
	assert( p_tree );

	/* check if root is black... */
	if( p_tree->root->is_red )
	{
		return FALSE;
	}

	return rbnode_verify_tree( p_tree, p_tree->root );
}

boolean rbnode_verify_tree( lc_rbtree_t* p_tree, lc_rbnode_t *t )
{
	assert( p_tree );

	if( t == &RBNIL )
	{
		return TRUE;
	}

	if( t->is_red )
	{
		if( t->left->is_red || t->right->is_red )
		{
			return FALSE;
		}
	}

	return rbnode_verify_tree( p_tree, t->left ) &&
	       rbnode_verify_tree( p_tree, t->right );
}

inline void padding( char ch, int n )
{
	int i;

	for( i = 0; i < n; i++ )
	{
		putchar( ch );
	}
}

inline void structure( const lc_rbnode_t *root, int level )
{
	int i;

	if( root == &RBNIL )
	{
		padding( '\t', level );
		puts( "~" );
	}
	else
	{
		structure( root->right, level + 1 );
		padding( '\t', level );
		printf( "%d-", *( (int *) root->data ) );
		printf( "%c\n", root->is_red ? 'R' : 'B' );
		structure( root->left, level + 1 );
	}
}

void rbtree_print( const lc_rbtree_t* p_tree )
{
	structure( p_tree->root, 0 );
}
#endif

