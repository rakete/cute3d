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
#include "tree-map.h"

/* Typical leaf node (always black) */
static lc_tree_map_node_t TREE_MAP_NODE_NIL = { (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL, (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL, (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL, FALSE, NULL };

#if defined(TREE_MAP_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
	#define DESTROY_CHECK( code ) \
		if( p_map->destroy ) \
		{ \
			code \
		}
#else
	#define DESTROY_CHECK( code ) \
		code
#endif


#define tree_map_node_init( p_node, p_key, p_value, p_parent, p_left, p_right, color ) \
	(p_node)->parent = p_parent; \
	(p_node)->left   = p_left; \
	(p_node)->right  = p_right; \
	(p_node)->key    = (void *) p_key; \
	(p_node)->value  = (void *) p_value; \
	(p_node)->is_red = color;


static lc_tree_map_node_t* tree_map_node_find       ( lc_tree_map_t *p_map, const void *key );


static __inline void tree_map_left_rotate( lc_tree_map_t *p_map, lc_tree_map_node_t *x )
{
	lc_tree_map_node_t *y;

	assert( x->right != &TREE_MAP_NODE_NIL );

	y        = x->right;
	x->right = y->left;

	if( y->left != &TREE_MAP_NODE_NIL )
	{
		y->left->parent = x;
	}

	y->parent = x->parent;

	if( x->parent == &TREE_MAP_NODE_NIL )
	{
		p_map->root = y;
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

static __inline void tree_map_right_rotate( lc_tree_map_t *p_map, lc_tree_map_node_t *x )
{
	lc_tree_map_node_t *y;

	assert( x->left != &TREE_MAP_NODE_NIL );

	y       = x->left;
	x->left = y->right;

	if( y->right != &TREE_MAP_NODE_NIL )
	{
		y->right->parent = x;
	}

	y->parent = x->parent;

	if( x->parent == &TREE_MAP_NODE_NIL )
	{
		p_map->root = y;
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

static __inline void tree_map_insert_fixup( lc_tree_map_t *p_map, lc_tree_map_node_t ** t )
{
	lc_tree_map_node_t *y = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;

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
					tree_map_left_rotate( p_map, (*t) );
				}
				/* case 3 - uncle is black and t is on the left */
				(*t)->parent->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				tree_map_right_rotate( p_map, (*t)->parent->parent );
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
					tree_map_right_rotate( p_map, (*t) );
				}
				/* case 3 - uncle is black and t is on the right */
				(*t)->parent->is_red = FALSE;
				(*t)->parent->parent->is_red = TRUE;
				tree_map_left_rotate( p_map, (*t)->parent->parent );
			}
		}
	}

	p_map->root->is_red = FALSE;
}

static __inline void tree_map_delete_fixup( lc_tree_map_t *p_map, lc_tree_map_node_t ** t )
{
	lc_tree_map_node_t *w = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;

	while( (*t) != p_map->root && (*t)->is_red == FALSE )
	{
		if( (*t) == (*t)->parent->left )
		{
			w = (*t)->parent->right;

			if( w->is_red )
			{
				w->is_red            = FALSE;
				(*t)->parent->is_red = TRUE;
				tree_map_left_rotate( p_map, (*t)->parent );
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
					tree_map_right_rotate( p_map, w );
					w = (*t)->parent->right;
				}

				w->is_red            = (*t)->parent->is_red;
				(*t)->parent->is_red = FALSE;
				w->right->is_red     = FALSE;
				tree_map_left_rotate( p_map, (*t)->parent );
				(*t) = p_map->root;
			}
		}
		else
		{
			w = (*t)->parent->left;

			if( w->is_red )
			{
				w->is_red = FALSE;
				(*t)->parent->is_red = TRUE;
				tree_map_right_rotate( p_map, (*t)->parent );
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
					tree_map_left_rotate( p_map, w );
					w = (*t)->parent->left;
				}

				w->is_red = (*t)->parent->is_red;
				(*t)->parent->is_red = FALSE;
				w->left->is_red = FALSE;
				tree_map_right_rotate( p_map, (*t)->parent );
				(*t) = p_map->root;
			}
		}
	}

	(*t)->is_red = FALSE;
}

lc_tree_map_node_t *tree_map_node_minimum( lc_tree_map_node_t *t )
{
	while( t->left != &TREE_MAP_NODE_NIL ) { t = t->left; }
	return t;
}

lc_tree_map_node_t *tree_map_node_maximum( lc_tree_map_node_t *t )
{
	while( t->right != &TREE_MAP_NODE_NIL ) { t = t->right; }
	return t;
}

lc_tree_map_node_t *tree_map_node_successor( lc_tree_map_node_t *t )
{
	lc_tree_map_node_t *y;

	if( t->right != &TREE_MAP_NODE_NIL )
	{
		return tree_map_node_minimum( t->right );
	}

	y = t->parent;

	while( y != &TREE_MAP_NODE_NIL && t == y->right )
	{
		t = y;
		y = y->parent;
	}

	return y;
}

lc_tree_map_node_t *tree_map_node_predecessor( lc_tree_map_node_t *t )
{
	lc_tree_map_node_t *y;

	if( t->left != &TREE_MAP_NODE_NIL )
	{
		return tree_map_node_maximum( t->left );
	}

	y = t->parent;

	while( y != &TREE_MAP_NODE_NIL && t == y->left )
	{
		t = y;
		y = y->parent;
	}

	return y;
}

lc_tree_map_t* tree_map_create_ex( tree_map_element_function destroy, tree_map_compare_function compare, alloc_function alloc, free_function free )
{
	lc_tree_map_t *p_map = (lc_tree_map_t *) malloc( sizeof(lc_tree_map_t) );
	assert( p_map );

	if( p_map )
	{
		tree_map_create( p_map, destroy, compare, alloc, free );
	}

	return p_map;
}

void tree_map_create( lc_tree_map_t *p_map, tree_map_element_function destroy, tree_map_compare_function compare, alloc_function alloc, free_function free )
{
	assert( p_map );
#ifndef TREE_MAP_CHECK_FOR_DESTROY
	assert( destroy );
#endif
	assert( compare );
	p_map->root    = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;
	p_map->size    = 0;
	p_map->compare = compare;
	p_map->destroy = destroy;
	p_map->_alloc  = alloc;
	p_map->_free   = free;
}

void tree_map_destroy( lc_tree_map_t *p_map )
{
	assert( p_map );
	tree_map_clear( p_map );

	#ifdef _DEBUG_TREE_MAP
	p_map->root    = NULL;
	p_map->size    = 0;
	p_map->compare = NULL;
	p_map->destroy = NULL;
	#endif
}

/*
void tree_map_copy( lc_tree_map_t const *p_srcTree, lc_tree_map_t *p_dstTree )
{
	lc_tree_map_node_t *p_node = NULL;

	if( p_srcTree != p_dstTree )
	{
		tree_map_clear( p_dstTree );

		p_dstTree->compare = p_srcTree->compare;
		p_dstTree->destroy = p_srcTree->destroy;

		// insert all of the nodes into this tree; m_NumberOfNodes should be equal after this...
		for( p_node = tree_map_minimum(p_srcTree); p_node != &TREE_MAP_NODE_NIL; p_node = tree_map_node_successor(p_node) )
		{
			tree_map_insert( p_dstTree, p_node->key, p_node->value );
		}
	}
}
*/

boolean tree_map_insert( lc_tree_map_t *p_map, const void *key, const void *value )
{
	lc_tree_map_node_t *y       = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;
	lc_tree_map_node_t *x       = p_map->root;

	lc_tree_map_node_t *newNode = (lc_tree_map_node_t *) p_map->_alloc( sizeof(lc_tree_map_node_t) );
	if( !newNode ) return FALSE;

	/* Find where to insert the new node--y points the parent. */
	while( x != &TREE_MAP_NODE_NIL )
	{
		y = x;
		if( p_map->compare( key, x->key ) < 0 )
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}

	/* Insert the new node */
	if( y == &TREE_MAP_NODE_NIL )
	{
		p_map->root = newNode;
	}
	else
	{
		if( p_map->compare( key, y->key ) < 0 )
		{
			y->left = newNode;
		}
		else
		{
			y->right = newNode;
		}
	}

	tree_map_node_init( newNode, key, value, y, (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL, (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL, TRUE );
	tree_map_insert_fixup( p_map, &newNode );
	p_map->size++;

	return TRUE;
}

boolean tree_map_remove( lc_tree_map_t *p_map, const void *key )
{
	lc_tree_map_node_t *t = tree_map_node_find( p_map, key );
	lc_tree_map_node_t *x;
	lc_tree_map_node_t *y;
	boolean y_is_red = FALSE;

	if( t == NULL ) return FALSE; /* item is not even in the tree! */

	if( t->left == &TREE_MAP_NODE_NIL || t->right == &TREE_MAP_NODE_NIL )
	{
		/* t has less than two children and can be deleted directly. */
		y = t;
	}
	else  /* t has two children... */
	{
		/* t has two children and must be switched out with it's  */
		/* successor. We will free the successor node and t's data. */
		y = tree_map_node_successor( t );
	}

	if( y->left != &TREE_MAP_NODE_NIL )
	{
		x = y->left;
	}
	else
	{
		x = y->right;
	}

	x->parent = y->parent;

	if( y->parent == &TREE_MAP_NODE_NIL )
	{
		p_map->root = x;
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
			p_map->destroy( t->key, t->value );
		);

		t->key   = y->key;
		t->value = y->value;

		y_is_red = y->is_red;

		p_map->_free( y );
	}
	else
	{
		DESTROY_CHECK(
			p_map->destroy( y->key, y->value );
		);
		y_is_red = y->is_red;
		p_map->_free( y );
	}

	if( y_is_red == FALSE ) /* y is black */
	{
		tree_map_delete_fixup( p_map, &x );
	}

	p_map->size--;

	return TRUE;
}

boolean tree_map_find( const lc_tree_map_t *p_map, const void *key, void **value )
{
	lc_tree_map_node_t *x = p_map->root;

	while( x != &TREE_MAP_NODE_NIL )
	{
		if( p_map->compare( key, x->key ) == 0 )
		{
			*value = x->value;
			return TRUE; /* found it */
		}
		else if( p_map->compare( key, x->key ) < 0 )
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

void tree_map_clear( lc_tree_map_t *p_map )
{
	lc_tree_map_node_t *x;
	lc_tree_map_node_t *y;

	assert( p_map );

	x = p_map->root;
	y = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;

	while( p_map->size > 0 )
	{
		y = x;

		/* find minimum... */
		while( y->left != &TREE_MAP_NODE_NIL )
		{
			y = y->left;
		}

		if( y->right == &TREE_MAP_NODE_NIL )
		{
			y->parent->left = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;
			x = y->parent;

			/* free... */
			DESTROY_CHECK(
				p_map->destroy( y->key, y->value );
			);

			p_map->_free( y );

			p_map->size--;
		}
		else
		{
			x = y->right;
			x->parent = y->parent;

			if( x->parent == &TREE_MAP_NODE_NIL )
			{
				p_map->root = x;
			}
			else
			{
				y->parent->left = x;
			}
			/* free... */
			DESTROY_CHECK(
				p_map->destroy( y->key, y->value );
			);

			p_map->_free( y );
			p_map->size--;
		}

		#ifdef _DEBUG_TREE_MAP
		y->key   = NULL;
		y->value = NULL;
		#endif
	}

	/* reset the root and current pointers */
	p_map->root = (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;
}

boolean tree_map_serialize( lc_tree_map_t *p_map, size_t key_size, size_t value_size, FILE *file )
{
	boolean result = TRUE;
	size_t count;
	lc_tree_map_iterator_t itr;

	if( !p_map )
	{
		result = FALSE;
		goto done;
	}

	count = tree_map_size(p_map);
	if( fwrite( &count, sizeof(size_t), 1, file ) != 1 )
	{
		result = FALSE;
		goto done;
	}

	for( itr = tree_map_begin( p_map );
	     itr != tree_map_end( ) && !ferror(file);
	     itr = tree_map_next( itr ) )
	{
		if( fwrite( itr->key, key_size, 1, file ) != 1 )
		{
			result = FALSE;
			goto done;
		}

		if( fwrite( itr->value, value_size, 1, file ) != 1 )
		{
			result = FALSE;
			goto done;
		}
	}

done:
	return result;
}

boolean tree_map_unserialize( lc_tree_map_t *p_map, size_t key_size, size_t value_size, FILE *file )
{
	boolean result = TRUE;
	size_t count = 0;

	if( !p_map )
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
		void *p_key;
		void *p_val;

		p_key = p_map->_alloc( key_size );

		if( fread( p_key, key_size, 1, file ) != 1 )
		{
			p_map->_free( p_key );

			result = FALSE;
			goto done;
		}

		p_val = p_map->_alloc( value_size );

		if( fread( p_val, value_size, 1, file ) != 1 )
		{
			p_map->_free( p_key );
			p_map->_free( p_val );

			result = FALSE;
			goto done;
		}

		tree_map_insert( p_map, p_key, p_val );
		count--;
	}

done:
	return result;
}


void tree_map_alloc_set( lc_tree_map_t *p_map, alloc_function alloc )
{
	assert( p_map );
	assert( alloc );
	p_map->_alloc = alloc;
}

void tree_map_free_set( lc_tree_map_t *p_map, free_function free )
{
	assert( p_map );
	assert( free );
	p_map->_free = free;
}

/* ------------------------------------- */

lc_tree_map_node_t *tree_map_node_find( lc_tree_map_t *p_map, const void *key )
{
	lc_tree_map_node_t *x = p_map->root;

	while( x != &TREE_MAP_NODE_NIL )
	{
		if( p_map->compare( key, x->key ) == 0 )
		{
			return x;
		}
		else if( p_map->compare( key, x->key ) < 0 )
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

lc_tree_map_iterator_t tree_map_begin( const lc_tree_map_t *p_map )
{
    return tree_map_node_minimum( p_map->root );
}

lc_tree_map_iterator_t tree_map_end( )
{
	return (lc_tree_map_node_t *) &TREE_MAP_NODE_NIL;
}



#ifdef _DEBUG_TREE_MAP
static boolean tree_map_node_verify_tree  ( lc_tree_map_t *p_map, lc_tree_map_node_t *t );
static void    padding             ( char ch, int n );
static void    structure           ( const lc_tree_map_node_t *root, int level );

boolean tree_map_verify_tree( lc_tree_map_t *p_map )
{
	/* check if root is black... */
	if( p_map->root->is_red )
	{
		return FALSE;
	}

	return tree_map_node_verify_tree( p_map, p_map->root );
}

boolean tree_map_node_verify_tree( lc_tree_map_t *p_map, lc_tree_map_node_t *t )
{
	if( t == &TREE_MAP_NODE_NIL )
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

	return tree_map_node_verify_tree( p_map, t->left ) &&
	       tree_map_node_verify_tree( p_map, t->right );
}

inline void padding( char ch, int n )
{
	int i;

	for( i = 0; i < n; i++ )
	{
		putchar( ch );
	}
}

inline void structure( const lc_tree_map_node_t *root, int level )
{
	int i;

	if( root == &TREE_MAP_NODE_NIL )
	{
		padding( '\t', level );
		puts( "~" );
	}
	else
	{
		structure( root->right, level + 1 );
		padding( '\t', level );
		printf( "%d-", *( (int *) root->key ) );
		printf( "%c\n", root->is_red ? 'R' : 'B' );
		structure( root->left, level + 1 );
	}
}

void tree_map_print( const lc_tree_map_t *p_map )
{
	structure( p_map->root, 0 );
}
#endif

