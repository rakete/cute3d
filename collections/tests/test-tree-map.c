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
#include <string.h>
#include <assert.h>
#include <time.h>
#include <tree-map.h>


boolean ip_destroy  ( void *key, void *value );


static const char *ips[] = {
	"85.162.151.163",  "252.241.190.83",  "82.107.121.249",  "226.233.19.20",
	"31.142.81.45",    "39.87.8.86",      "208.212.5.167",   "117.119.130.82",
	"237.74.153.27",   "82.106.61.88",    "74.36.213.54",    "149.173.142.104",
	"181.53.60.95",    "108.221.140.196", "226.61.50.17",    "207.89.180.180",
	"39.61.35.206",    "150.249.167.223", "102.224.30.252",  "140.123.14.44",
	"143.66.48.202",   "206.123.159.188", "135.177.184.209", "187.214.138.236",
	"14.99.21.46",     "58.164.220.60",   "167.32.6.122",    "221.52.27.147",
	"136.236.245.76",  "104.214.104.150", "142.111.9.160",   "65.201.71.147",
	"151.207.172.219", "5.81.243.137",    "14.44.113.121",   "89.107.96.13",
	"108.225.89.213",  "99.85.185.193",   "85.113.68.66",    "230.151.186.11",
	"113.254.103.104", "85.118.80.91",    "79.99.34.194",    "216.168.206.2",
	"106.69.10.167",   "133.77.26.67",    "207.88.96.231",   "4.194.100.151",
	"166.154.104.156", "153.245.92.200",  "13.189.155.242",  "224.202.34.0",
	"44.45.16.176",    "185.227.222.82",  "214.137.60.190",  "188.218.77.160",
	"125.99.245.53",   "196.24.216.209",  "244.209.85.117",  "41.213.29.119",
	"127.85.130.45",   "160.58.184.224",  "148.119.67.116",  "69.81.209.16",
	"26.67.52.199",    "3.222.91.14",     "16.12.5.25",      "124.98.1.34",
	NULL
};

#define MAX 40

int main( int argc, char *argv[] )
{
	lc_tree_map_t map;
	unsigned int i;
	boolean result;
	lc_tree_map_iterator_t itr;

	srand( time(NULL) );

	tree_map_create( &map, ip_destroy, (tree_map_compare_function) strcmp, malloc, free );

	for( i = 0; ips[ i ]; i++ )
	{
		char *ip = strdup( ips[ i ] );
		boolean *p_sent = malloc( sizeof(boolean) );
		*p_sent = FALSE;

		result = tree_map_insert( &map, ip, p_sent );
		assert( result );

		printf( "         Added (%03d): %-16s      (%02ld)", i, ip, tree_map_size(&map) );

		printf( "\n" );
	}

	for( i = 0; i < MAX; i++ )
	{
		const char *ip = ips[ rand() % MAX ];
		boolean *is_sent  = NULL;

		if( tree_map_find( &map, ip, (void **) &is_sent ) )
		{

			if( *is_sent == FALSE )
			{
				*is_sent = TRUE;

				printf( "     Message Sent To: %-16s\n", ip );
			}
			else
			{
				printf( "     Already Sent To: %-16s\n", ip );
			}
		}
	}

	printf( "\n\n" );

	i = 0;

	for( itr = tree_map_begin( &map );
		 itr != tree_map_end( );
		 itr = tree_map_next( itr ) )
	{
		printf( "%5u  %16s => %s\n", i, (char* ) itr->key, *((boolean*) itr->value) ? "true" : "false" );
		i++;
	}
	printf( "\n\n" );

	for( i = 0; ips[ i ]; i++ )
	{
		const char *ip = ips[ i ];
		boolean *is_sent  = NULL;

		tree_map_find( &map, ip, (void **) &is_sent );

		printf( "       Removed (%03d): %-16s       (%02ld)   %s", i, ip, tree_map_size(&map), is_sent && *is_sent ? "sent" : "not sent" );

		result = tree_map_remove( &map, ip );
		assert( result );


		printf( "\n" );
	}



	tree_map_destroy( &map );
	return 0;
}

boolean ip_destroy( void *key, void *value )
{
	free( key );
	free( value );
	return TRUE;
}
