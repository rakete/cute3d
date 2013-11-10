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
#include <assert.h>
#include "types.h"
#include "slist.h"

#if defined(SLIST_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
	#define DESTROY_CHECK( code ) \
		if( p_list->destroy ) \
		{ \
			code \
		}
#else
	#define DESTROY_CHECK( code ) \
		code
#endif


void slist_create( lc_slist_t *p_list, slist_element_function destroy_callback, alloc_function alloc, free_function free )
{
	assert( p_list );

	p_list->head    = NULL;
	p_list->size    = 0;
	p_list->destroy = destroy_callback;

	p_list->alloc = alloc;
	p_list->free  = free;
}

void slist_destroy( lc_slist_t *p_list )
{
	slist_clear( p_list );

	#ifdef _SLIST_DEBUG
	p_list->head    = NULL;
	p_list->size    = 0;
	p_list->destroy = NULL;
	#endif
}

boolean slist_insert_front( lc_slist_t *p_list, const void *data ) /* O(1) */
{
	lc_slist_node_t *p_node;
	assert( p_list );

	p_node = p_list->alloc( sizeof(lc_slist_node_t) );
	assert( p_node );

	if( p_node != NULL )
	{
		p_node->data = (void *) data;
		p_node->next = p_list->head;

		p_list->head = p_node;
		p_list->size++;
		return TRUE;
	}

	return FALSE;
}

boolean slist_remove_front( lc_slist_t *p_list ) /* O(1) */
{
	lc_slist_node_t *p_node;
	boolean result = TRUE;

	assert( p_list );
	assert( slist_size(p_list) >= 1 );

	p_node = p_list->head->next;

	DESTROY_CHECK(
		result = p_list->destroy( p_list->head->data );
	);

	p_list->free( p_list->head );

	p_list->head = p_node;
	p_list->size--;

	return result;
}

boolean slist_insert_next( lc_slist_t *p_list, lc_slist_node_t *p_front_node, const void *data ) /* O(1) */
{
	assert( p_list );
	assert( p_front_node );

	if( p_front_node )
	{
		lc_slist_node_t *p_node = p_list->alloc( sizeof(lc_slist_node_t) );
		assert( p_node );

		if( p_node != NULL )
		{
			p_node->data = (void *) data;
			p_node->next = p_front_node->next;

			p_front_node->next = p_node;
			p_list->size++;
			return TRUE;
		}

		return FALSE;
	}

	return slist_insert_front( p_list, data );
}

boolean slist_remove_next( lc_slist_t *p_list, lc_slist_node_t *p_front_node ) /* O(1) */
{
	assert( p_list );
	assert( slist_size(p_list) >= 1 );

	if( p_front_node )
	{
		boolean result;
		lc_slist_node_t *p_node;
		lc_slist_node_t *p_new_next;

		assert( p_front_node->next );
		result     = TRUE;
		p_node     = p_front_node->next;
		p_new_next = p_node->next;

		DESTROY_CHECK(
			result = p_list->destroy( p_node->data );
		);

		p_list->free( p_node );

		p_front_node->next = p_new_next;
		p_list->size--;

		return result;
	}

	return slist_remove_front( p_list );
}

void slist_clear( lc_slist_t *p_list )
{
	while( slist_head(p_list) )
	{
		slist_remove_front( p_list );
	}
}

void slist_alloc_set( lc_slist_t *p_list, alloc_function alloc )
{
	assert( p_list );
	assert( alloc );
	p_list->alloc = alloc;
}

void slist_free_set( lc_slist_t *p_list, free_function free )
{
	assert( p_list );
	assert( free );
	p_list->free = free;
}

lc_slist_iterator_t slist_begin( const lc_slist_t *p_list )
{
	assert( p_list );
	return p_list->head;
}

lc_slist_iterator_t slist_next( const lc_slist_iterator_t iter )
{
	assert( iter );
	return iter->next;
}
