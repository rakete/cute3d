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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <rbtree.h>

int compare( const void *num1, const void *num2 )
{
	return *((int*) num1) - *((int*)num2);
}

boolean destroy( void *num )
{
	free( num );
	num = NULL;
	return TRUE;
}

#if 0
#include "asciitree.h"
void print_tree( lc_rbtree_t *tree )
{
	print_ascii_tree( tree->root );
}
#endif

#define RANDOMIZE 1


int main( int argc, char *argv[] )
{
	lc_rbtree_t tree;
	int i;

	rbtree_create( &tree, destroy, compare, malloc, free );

	srand( time(NULL) );


	for( i = 0; i < 30; i++ )
	{
		int *num = (int*) malloc( sizeof(int) );

		do {
			#ifdef RANDOMIZE
			*num = rand( ) % 100;
			#else
			*num = i + 1;
			#endif
		} while( rbtree_search( &tree, num ) );

		printf( "Inserting %d\n", *num );
		rbtree_insert( &tree, (void *) num );
		//rbtree_print( &tree );

		//printf( "\n----------------------------\n" );
	}

	//print_tree( &tree );
	printf("------------ DONE INSERTING -----------\n" );

	FILE *file = tmpfile( );
	if( file )
	{
		rbtree_serialize( &tree, sizeof(int), file );
		size_t size = rbtree_size( &tree );
		printf( "  Serialized %ld items.\n", rbtree_size(&tree) );
		rbtree_clear( &tree );
		assert( rbtree_size( &tree) == 0 );
		fseek( file, 0, SEEK_SET );
		rbtree_unserialize( &tree, sizeof(int), file );
		printf( "Unserialized %ld items.\n", rbtree_size(&tree) );
		assert( size == rbtree_size( &tree ) );
		fclose( file );
	}
	printf("----------- DONE SERIALIZING ----------\n" );

	//boolean isGood = rbtree_verify_tree( &tree );
	//printf( "Tree is %s\n", isGood ? "good" : "bad" );

	printf( "Tree size = %ld \n", tree.size );

	i = 11;
	printf( "Removing %d.\n", i );
	rbtree_remove( &tree, &i );
	printf( "Tree size = %ld \n", tree.size );
	//print_tree( &tree );
	printf("------------- DONE REMOVING -----------\n" );

	i = 30;
	printf( "Removing %d.\n", i );
	rbtree_remove( &tree, &i );
	printf( "Tree size = %ld \n", tree.size );
	//print_tree( &tree );
	printf("------------- DONE REMOVING -----------\n" );

	i = 56;
	printf( "Removing %d.\n", i );
	rbtree_remove( &tree, &i );
	printf( "Tree size = %ld \n", tree.size );
	//print_tree( &tree );
	printf("------------- DONE REMOVING -----------\n" );

	i = 92;
	printf( "Removing %d.\n", i );
	rbtree_remove( &tree, &i );
	printf( "Tree size = %ld \n", tree.size );
	//print_tree( &tree );
	printf("------------- DONE REMOVING -----------\n" );

	i = 49;
	printf( "Removing %d.\n", i );
	rbtree_remove( &tree, &i );
	printf( "Tree size = %ld \n", tree.size );
	//print_tree( &tree );
	printf("------------- DONE REMOVING -----------\n" );


	printf( "Clearing the tree.\n" );
	rbtree_clear( &tree );
	printf( "Tree size = %ld \n", tree.size );

	rbtree_destroy( &tree );
	return 0;
}
