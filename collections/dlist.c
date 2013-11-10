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
#include "dlist.h"

#if defined(DLIST_DESTROY_CHECK) || defined(DESTROY_CHECK_ALL)
	#define DESTROY_CHECK( code ) \
		if( p_list->destroy ) \
		{ \
			code \
		}
#else
	#define DESTROY_CHECK( code ) \
		code
#endif


void dlist_create( lc_dlist_t* p_list, dlist_element_function destroy_callback, alloc_function alloc, free_function free )
{
	assert( p_list );

	p_list->head    = NULL;
	p_list->tail    = NULL;
	p_list->size    = 0;
	p_list->destroy = destroy_callback;

	p_list->alloc = alloc;
	p_list->free  = free;
}

void dlist_destroy( lc_dlist_t* p_list )
{
	dlist_clear( p_list );

	#ifdef _DLIST_DEBUG
	p_list->head    = NULL;
	p_list->tail    = NULL;
	p_list->size    = 0;
	p_list->destroy = NULL;
	#endif
}

boolean dlist_insert_front( lc_dlist_t* p_list, const void *data ) /* O(1) */
{
	lc_dlist_node_t* p_node;
	assert( p_list );

	p_node = p_list->alloc( sizeof(lc_dlist_node_t) );
	assert( p_node );

	if( p_node != NULL )
	{
		if( p_list->head )
		{
			p_list->head->prev = p_node;
		}

		p_node->data = (void *) data;
		p_node->next = p_list->head;
		p_node->prev = NULL;

		if( p_node->next == NULL )
		{
			p_list->tail = p_node;
		}

		p_list->head = p_node;
		p_list->size++;
		return TRUE;
	}

	return FALSE;
}

boolean dlist_remove_front( lc_dlist_t* p_list ) /* O(1) */
{
	lc_dlist_node_t* p_node;
	boolean result = TRUE;

	assert( p_list );
	assert( dlist_size(p_list) >= 1 );

	p_node = p_list->head->next;

	if( p_node )
	{
		p_node->prev = NULL;

		if( p_node->next == NULL )
		{
			p_list->tail = p_node;
		}
	}
	else
	{
		p_list->tail = NULL;
	}

	DESTROY_CHECK(
		result = p_list->destroy( p_list->head->data );
	);

	p_list->free( p_list->head );

	p_list->head = p_node;
	p_list->size--;

	return result;
}

boolean dlist_insert_back( lc_dlist_t* p_list, const void *data ) /* O(1) */
{
	lc_dlist_node_t* p_node;
	assert( p_list );

	p_node = p_list->alloc( sizeof(lc_dlist_node_t) );
	assert( p_node );

	if( p_node != NULL )
	{
		if( p_list->tail )
		{
			p_list->tail->next = p_node;
		}
		else
		{
			p_list->head = p_node;
		}

		p_node->data = (void *) data;
		p_node->next = NULL;
		p_node->prev = p_list->tail;

		p_list->tail = p_node;
		p_list->size++;
		return TRUE;
	}

	return FALSE;
}

boolean dlist_remove_back( lc_dlist_t* p_list ) /* O(1) */
{
	lc_dlist_node_t* p_node;
	boolean result = TRUE;

	assert( p_list );
	assert( dlist_size(p_list) >= 1 );

	p_node = p_list->tail->prev;

	if( p_node )
	{
		p_node->next = NULL;

		if( p_node->prev == NULL )
		{
			p_list->head = p_node;
		}
	}
	else
	{
		p_list->head = NULL;
	}

	DESTROY_CHECK(
		result = p_list->destroy( p_list->tail->data );
	);

	p_list->free( p_list->tail );

	p_list->tail = p_node;
	p_list->size--;

	return result;
}

boolean dlist_insert_next( lc_dlist_t* p_list, lc_dlist_node_t* p_front_node, const void *data ) /* O(1) */
{
	assert( p_list );
	assert( p_front_node );

	if( p_front_node )
	{
		lc_dlist_node_t* p_node = p_list->alloc( sizeof(lc_dlist_node_t) );
		assert( p_node );

		if( p_node != NULL )
		{
			p_node->data = (void *) data;
			p_node->next = p_front_node->next;
			p_node->prev = p_front_node;

			if( p_front_node->next )
			{
				p_front_node->next->prev = p_node;
			}
			else
			{
				p_list->tail = p_node;
			}

			p_front_node->next = p_node;
			p_list->size++;
			return TRUE;
		}

		return FALSE;
	}

	return dlist_insert_front( p_list, data );
}

boolean dlist_remove_next( lc_dlist_t* p_list, lc_dlist_node_t* p_front_node ) /* O(1) */
{
	assert( p_list );
	assert( dlist_size(p_list) >= 1 );

	if( p_front_node )
	{
		boolean result;
		lc_dlist_node_t* p_node;
		lc_dlist_node_t* p_new_next;

		assert( p_front_node->next );
		result     = TRUE;
		p_node     = p_front_node->next;
		p_new_next = p_node->next;

		if( p_new_next )
		{
			p_new_next->prev = p_front_node;
		}
		else
		{
			p_list->tail = p_front_node;
		}

		DESTROY_CHECK(
			result = p_list->destroy( p_node->data );
		);

		p_list->free( p_node );

		p_front_node->next = p_new_next;
		p_list->size--;

		return result;
	}

	return dlist_remove_front( p_list );
}

void dlist_clear( lc_dlist_t* p_list )
{
	while( dlist_head(p_list) )
	{
		dlist_remove_front( p_list );
	}
}

void dlist_alloc_set( lc_dlist_t* p_list, alloc_function alloc )
{
	assert( p_list );
	assert( alloc );
	p_list->alloc = alloc;
}

void dlist_free_set( lc_dlist_t* p_list, free_function free )
{
	assert( p_list );
	assert( free );
	p_list->free = free;
}

lc_dlist_iterator_t dlist_begin( const lc_dlist_t* p_list )
{
	assert( p_list );
	return p_list->head;
}

lc_dlist_iterator_t dlist_rbegin( const lc_dlist_t* p_list )
{
	assert( p_list );
	return p_list->tail;
}

lc_dlist_iterator_t dlist_next( const lc_dlist_iterator_t iter )
{
	assert( iter );
	return iter->next;
}

lc_dlist_iterator_t dlist_previous( const lc_dlist_iterator_t iter )
{
	assert( iter );
	return iter->prev;
}

