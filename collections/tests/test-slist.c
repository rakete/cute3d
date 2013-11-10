#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <slist.h>
#include <tstring.h>
#include <alloc.h>


static boolean national_park_destroy( void *element );
static void print_national_parks( void );
static void delete_random_park( void );
static void insert_random_park( void );

const tchar* national_parks[] = {
	_T("Acadia"),
	_T("American Samoa"),
	_T("Arches"),
	_T("Badlands"),
	_T("Big Bend"),
	_T("Biscayne"),
	_T("Black Canyon of the Gunnison"),
	_T("Bryce Canyon"),
	_T("Canyonlands"),
	_T("Capitol Reef"),
	_T("Carlsbad Caverns"),
	_T("Channel Islands"),
	_T("Congaree"),
	_T("Crater Lake"),
	_T("Cuyahoga Valley"),
	_T("Death Valley"),
	_T("Denali"),
	_T("Dry Tortugas"),
	_T("Everglades"),
	_T("Gates of the Arctic"),
	_T("Glacier"),
	_T("Glacier Bay"),
	_T("Grand Canyon"),
	_T("Grand Teton"),
	_T("Great Basin"),
	_T("Great Sand Dunes"),
	_T("Great Smoky Mountains"),
	_T("Guadalupe Mountains"),
	_T("Haleakala"),
	_T("Hawai'i Volcanoes"),
	_T("Hot Springs"),
	_T("Isle Royale"),
	_T("Joshua Tree"),
	_T("Katmai"),
	_T("Kenai Fjords"),
	_T("Kobuk Valley"),
	_T("Lake Clark"),
	_T("Lassen Volcanic"),
	_T("Mammoth Cave"),
	_T("Mesa Verde"),
	_T("Mount Rainier"),
	_T("North Cascades"),
	_T("Olympic"),
	_T("Petrified Forest"),
	_T("Redwood"),
	_T("Rocky Mountain"),
	_T("Saguaro"),
	_T("Sequoia and Kings Canyon"),
	_T("Shenandoah"),
	_T("Theodore Roosevelt"),
	_T("Virgin Islands"),
	_T("Voyageurs"),
	_T("Wind Cave"),
	_T("Wrangell-St. Elias"),
	_T("Yellowstone"),
	_T("Yosemite"),
	_T("Zion"),
	NULL
};

lc_slist_t list;


int main( int argc, char *argv[] )
{
	size_t i;

	time_t ts = time(NULL);

	tprintf( _T("seed: %ld\n"), ts );
	srand( ts );

	slist_create( &list, national_park_destroy, malloc, free );

	for( i = 0; national_parks[ i ] != NULL; i++ )
	{
		const tchar *national_park = national_parks[ i ];
		lc_tstring_t *p_string = alloc_type( lc_tstring_t );
		tstring_create( p_string, national_park );

		slist_insert_front( &list, p_string );
	}

	tprintf( _T("Initial Size: %ld\n"), slist_size(&list) );


	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	delete_random_park( );
	insert_random_park( );
	insert_random_park( );
	insert_random_park( );
	insert_random_park( );
	insert_random_park( );
	insert_random_park( );

	print_national_parks( );

	slist_destroy( &list );

	tprintf( _T("====================================\n") );
	return 0;
}

boolean national_park_destroy( void *element )
{
	lc_tstring_t *p_string = element;

	tstring_destroy( p_string );
	free( p_string );
	return TRUE;
}

void print_national_parks( void )
{
	size_t i;
	lc_slist_iterator_t iter;

	#if defined(UNICODE)
	tprintf( _T("   # %30ls\n====================================\n"), _T("National Park") );
	#else
	tprintf( _T("   # %30s\n====================================\n"), _T("National Park") );
	#endif

	i = 1;
	for( iter = slist_begin(&list); iter != slist_end( ); iter = slist_next(iter) )
	{
		const lc_tstring_t *p_string  = iter->data;

		const tchar *national_park = tstring_string( p_string );
		#if defined(UNICODE)
		tprintf( _T("%4ld %30ls\n"), i++, national_park );
		#else
		tprintf( _T("%4ld %30s\n"), i++, national_park );
		#endif
	}
}

void delete_random_park( void )
{
	lc_slist_iterator_t iter;
	size_t r = rand() % slist_size( &list );
	size_t i = 0;

	if( r + 1 >= slist_size(&list) )
	{
		r--;
	}

	for( iter = slist_begin(&list); iter != slist_end( ); iter = slist_next(iter) )
	{
		if( i++ == r )
		{
			printf( "Deleting %ld\n", r + 1 );
			slist_remove_next( &list, iter );
			break;
		}
	}
}

void insert_random_park( void )
{
	lc_slist_iterator_t iter;
	size_t r = rand() % slist_size( &list );
	size_t i = 0;

	for( iter = slist_begin(&list); iter != slist_end( ); iter = slist_next(iter) )
	{
		if( i++ == r )
		{
			const tchar *national_park = national_parks[ i ];
			lc_tstring_t *p_string = alloc_type( lc_tstring_t );
			tstring_create( p_string, national_park );

			tstring_sconcatenate( p_string, _T(" (Copy)") );

			printf( "Inserting %ld\n", r + 1 );
			slist_insert_next( &list, iter, p_string );
			break;
		}
	}
}
