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
#include <lhash-table.h>

#define IP_HASHING_TEST

void    lhash_table_debug ( lc_lhash_table_t* p_table );
boolean data_destroy      ( void *data );

#if defined(IP_HASHING_TEST)

size_t  ip_hash      ( const void *ip );
int     ip_compare   ( const char *left, const char *right );

static const char *IPs[] = {
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

int main( int argc, char *argv[] )
{
	lc_lhash_table_t table;
	boolean result;
	int i;
	unsigned int L = 2;

	result = lhash_table_create( &table, sizeof(char *), 68, (lhash_table_hash_function) ip_hash, (lhash_table_compare_function) ip_compare, malloc, free );
	assert( result );

	srand( 0 );


	while( L-- > 0 )
	{
		for( i = 0; IPs[ i ]; i++ )
		{
			const char *ip = IPs[ i ];

			result = lhash_table_insert( &table, &ip );
			assert( result );
			printf( "   Added (%03d): %-16s      %4.1f  (%03ld) ", i, ip, lhash_table_load_factor(&table), lhash_table_size(&table) );

			lhash_table_debug( &table );

			#ifdef TEST_REHASH
			if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
			{
				printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
			}
			#endif

			printf( "\n" );
		}

		for( i = 0; IPs[ i ]; i++ )
		{
			const char *ip = IPs[ i ];
			void *found_ip = NULL;

			if( lhash_table_find( &table, &ip, &found_ip ) )
			{
				result = lhash_table_remove( &table, found_ip );
				assert( result );
				printf( " Removed (%03d): %-16s      %4.1f  (%03ld) ", i, ip, lhash_table_load_factor(&table), lhash_table_size(&table) );

				lhash_table_debug( &table );

				#ifdef TEST_REHASH
				if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
				{
					printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
				}
				#endif

				printf( "\n" );
			}
		}
	} /* end while */

	lhash_table_destroy( &table );
	return 0;
}
#else
#define TEST_REHASH
#define MAX              80
size_t  int_hash     ( const int *num );
int     int_compare  ( const int *left, const int *right );

int data_set[] = {
	 3,   49,  213,   11, -132,  18,   5,
	84,  334,  723,   96,  -66,  19,   4,
	 2,  -45, -456,   28,  518, -87, -61,
  	 3,   41,  435, -550, -412,  69,  84,
	-5,  505,  612,  -43,   92, 391, 171,
	 0
};

int main( int argc, char *argv[] )
{
	lc_lhash_table_t table;
	boolean result;
	int i;

	result = lhash_table_create( &table, sizeof(int), 1, (lhash_table_hash_function) int_hash, (lhash_table_compare_function) int_compare );
	assert( result );

	srand( 0 );

	for( i = 0; data_set[ i ]; i++ )
	{
		const int *p_num = &data_set[ i ];

		result = lhash_table_insert( &table, p_num );
		assert( result );
		printf( "   Added (%03d): %16d      %4.1lf  (%03ld) ", i, *p_num, lhash_table_load_factor(&table), lhash_table_size(&table) );

		lhash_table_debug( &table );

		#ifdef TEST_REHASH
		if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
		{
			printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
		}
		#endif

		printf( "\n" );
	}

	for( i = 0; i < MAX; i++ )
	{
		int num = 1 + (rand() % 500);

		if( i % 2 )
		{
			result = lhash_table_insert( &table, &num );
			assert( result );
			printf( "   Added (%03d): %16d      %4.1lf  (%03ld) ", i, num, lhash_table_load_factor(&table), lhash_table_size(&table) );
			lhash_table_debug( &table );
			#ifdef TEST_REHASH
			if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
			{
				printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
			}
			#endif
			printf( "\n" );
		}
		else
		{
			int *p_found = NULL;

			if( lhash_table_find( &table, &num, (void **) &p_found ) )
			{
				result = lhash_table_remove( &table, &num );
				assert( result );
				printf( " Removed (%03d): %16d      %4.1lf  (%03ld) ", i, num, lhash_table_load_factor(&table), lhash_table_size(&table) );
				lhash_table_debug( &table );
				#ifdef TEST_REHASH
				if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
				{
					printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
				}
				#endif
				printf( "\n" );
			}
		}
	}

	for( i = 0; data_set[ i ]; i++ )
	{
		const int *p_num = &data_set[ i ];
		int *p_found_num = NULL;

		if( lhash_table_find( &table, p_num, (void **) &p_found_num ) )
		{
			result = lhash_table_remove( &table, p_found_num );
			assert( result );
			printf( " Removed (%03d): %16d      %4.1lf  (%03ld) ", i, *p_num, lhash_table_load_factor(&table), lhash_table_size(&table) );

			lhash_table_debug( &table );

			#ifdef TEST_REHASH
			if( lhash_table_rehash( &table, LHASH_TABLE_LOAD_FACTOR ) )
			{
				printf( " ---> Rehashed (size = %ld, table_size = %ld)", lhash_table_size(&table), lhash_table_table_size(&table) );
			}
			#endif

			printf( "\n" );
		}
	}

	lhash_table_destroy( &table );
	return 0;
}
#endif

boolean data_destroy( void *data )
{
	free( data );
	return TRUE;
}

size_t ip_hash( const void *data )
{
	unsigned short count = 0;
	size_t hash = 0;
	char ip[ 24 ];
	char *token;
	char *in_ip;

	assert( data );

	in_ip = *((char **) data);
	strncpy( ip, in_ip, sizeof(ip) );
	ip[ sizeof(ip) - 1 ] = '\0';

	token = strtok( (char *) ip, "." );

	while( token != NULL )
	{
		int part = atoi( token );

		hash |= (part << 8 * (3 - count));

		token = strtok( NULL, "." );
	}

	return hash;
}

int ip_compare( const char *left, const char *right )
{
	return strcmp( left, right );
}

size_t int_hash( const int *num )
{
	return ((*num & 0x37) << 16) + (*num & 0x7);
}

int int_compare( const int *left, const int *right )
{
	return *left - *right;
}

