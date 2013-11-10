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
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <wctype.h>
#include <assert.h>
#ifndef WIN32
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include "tstring.h"
#include "flat-db.h"

static boolean  flatdb_create_empty_database ( flatdb_t db, uint16_t max_tables, uint16_t max_records );
static flatdb_t flatdb_create_temporary      ( const flatdb_t source_db );
static boolean  flatdb_file_exists           ( const tchar *filename );
static boolean  flatdb_load_file             ( flatdb_t db );
static boolean  flatdb_next_id               ( flatdb_t db, flat_id_t table_id, const flat_record *p_record, flat_id_t *p_next_record_id );
static boolean  file_copy                    ( FILE *dst, FILE *src );
static boolean  record_lock                  ( flatdb_t db, offset_t position, size_t object_size, short type /* F_RDLCK, F_WRLCK, F_UNLCK */ );
static boolean  record_unlock                ( flatdb_t db, offset_t position, size_t object_size );



#define alloc_db( )                 ((flatdb_t) malloc( sizeof(flatdb) ))
#define destroy_db( db )            free(db)
#define alloc_record( p_table )     ((flat_record *) malloc( (p_table)->record_size ))
#define destroy_record( p_record )  free(p_record)

#define flatdb_tables_size( db )    (flatdb_max_tables(db) * sizeof(flat_table))
#define flatdb_indices_size( db )   (flatdb_max_tables(db) * flatdb_max_records(db) * sizeof(offset_t))

#define flatdb_table_position( db, table_id )              (sizeof((db)->header) + sizeof(flat_table) * (table_id))
#define flatdb_index_position( db, table_id, record_id )   (sizeof((db)->header) + flatdb_tables_size(db) + ((table_id * flatdb_max_records(db) + record_id) * sizeof(offset_t)))
#define flatdb_record_position( db, table_id, record_id )  (flatdb_index_get(db, table_id, record_id))



flatdb_t flatdb_open( const tchar *filename )
{
	flatdb_t db = NULL;

	if( flatdb_file_exists( filename ) )
	{
		#ifdef WIN32
		FILE *p_file = _wfopen( filename, "rb+" );
		#else
		FILE *p_file = fopen( (char *) filename, "rb+" );
		#endif

		if( p_file )
		{
			db = alloc_db( );

			if( !db )
			{
				goto failed;
			}

			db->filename = tstrdup( filename );
			db->file     = p_file;

			flockfile( db->file );
			if( !flatdb_load_file( db ) )
			{
				goto failed;
			}
			funlockfile( db->file );
		}
	}
	else
	{
		#ifdef FLDB_CREATE_DB_WHEN_NONEXISTANT
		db = flatdb_create( filename, FLDB_MAX_TABLES, FLDB_MAX_RECORDS );
		#else
		goto failed;
		#endif
	}

	return db;

failed:
	#ifdef FLDB_CREATE_DB_WHEN_NONEXISTANT
	funlockfile( db->file );
	#else
	if( db ) funlockfile( db->file );
	#endif
	flatdb_close( &db );
	assert( db == NULL );
	return db;
}

flatdb_t flatdb_create( const tchar *filename, uint16_t max_tables, uint16_t max_records )
{
	flatdb_t db  = NULL;

	#ifdef WIN32
	FILE *p_file = _wfopen( filename, file_mode );
	#else
	FILE *p_file = fopen( (char *) filename, "wb+" );
	#endif

	if( p_file )
	{
		db = alloc_db( );

		if( !db )
		{
			goto failed;
		}

		db->filename = tstrdup( filename );
		db->file     = p_file;

		flockfile( db->file );

		if( !flatdb_create_empty_database( db, max_tables, max_records ) )
		{
			goto failed;
		}
		funlockfile( db->file );
	}
	return db;

failed:
	funlockfile( db->file );
	flatdb_close( &db );
	assert( db == NULL );
	return db;
}

flatdb_t flatdb_create_temporary( const flatdb_t source_db )
{
	flatdb_t db = NULL;

	FILE *p_file = tmpfile( );

	if( p_file )
	{
		db = alloc_db( );

		if( !db )
		{
			goto failed;
		}

		db->filename = NULL;
		db->file     = p_file;

		flockfile( db->file );

		if( !flatdb_create_empty_database( db, flatdb_max_tables(source_db), flatdb_max_records(source_db) ) )
		{
			goto failed;
		}
		funlockfile( db->file );
	}

	return db;

failed:
	funlockfile( db->file );
	flatdb_close( &db );
	assert( db == NULL );
	return db;
}

void flatdb_close( flatdb_t *p_db )
{
	if( p_db && *p_db )
	{
		flatdb_t db = *p_db;

		free( db->comparers );
		free( db->hashers );
		free( db->indices );
		free( db->tables );
		if( db->filename ) free( db->filename );
		fclose( db->file );
		free( db );

		*p_db = NULL;
	}
}

uint16_t flatdb_max_tables( flatdb_t db )
{
	if( db )
	{
		return db->header.max_tables;
	}

	return 0;
}

uint16_t flatdb_max_records( flatdb_t db )
{
	if( db )
	{
		return db->header.max_records;
	}

	return 0;
}

const tchar* flatdb_filename( flatdb_t db )
{
	if( db )
	{
		return db->filename;
	}

	return NULL;
}
#if 1
boolean flatdb_shrink( flatdb_t db )
{
	boolean result = TRUE;
	flat_id_t table_id;
	flatdb_t temp_db = flatdb_create_temporary( db );
	void *new_tables;
	void *new_indices;

	if( !temp_db )
	{
		result = FALSE;
		goto done;
	}

	for( table_id = 0; result && table_id < flatdb_max_tables(db); table_id++ )
	{
		flat_record* p_record;
		flat_table* p_old_table = flatdb_table_get(      db, table_id );
		flat_table* p_table     = flatdb_table_get( temp_db, table_id );

		p_table->base            = p_old_table->base;
		p_table->record_size     = p_old_table->record_size;
		p_table->reserved        = 0;
		p_table->first_record    = 0L;
		p_table->deleted_record  = 0L;
		p_table->count           = 0;
		#ifdef _FLAT_TABLE_INCLUDE_NAME
		strncpy( p_table->name, p_old_table->name, FLDB_MAX_TABLE_NAME );
		#endif

		flatdb_table_save( temp_db, table_id );

		p_record = flatdb_record_first( db, table_id );

		while( p_record )
		{
			flatdb_record_add( temp_db, table_id, p_record );
			p_record = flatdb_record_next( db, table_id, p_record );
		}
	}

	#ifdef WIN32
	#error "File truncating needs to be implemented for Windows."
	#else
	if( ftruncate( fileno(db->file), 0L ) < 0 )
	{
		result = FALSE;
		goto done;
	}
	#endif

	/* Swap the tables */
	new_tables      = temp_db->tables;
	temp_db->tables = db->tables;
	db->tables      = new_tables;

	/* Swap the indices */
	new_indices      = temp_db->indices;
	temp_db->indices = db->indices;
	db->indices      = new_indices;

	if( !file_copy( db->file, temp_db->file ) )
	{
		result = FALSE;
		goto done;
	}

done:
	/* The temporary file in temp_db will be deleted
	 * upon close
	 */
	flatdb_close( &temp_db );
	return result;
}
#else
boolean flatdb_shrink( flatdb_t old_db )
{
	boolean result = TRUE;
	flat_id_t table_id;
	const char *filename = tmpnam( NULL );
	flatdb_t new_db      = flatdb_create( filename, flatdb_max_tables(old_db), flatdb_max_records(old_db) );

	/*memcpy( &new_db->tables, &old_db->tables, sizeof(new_db->tables) );*/

	for( table_id = 0; result && table_id < flatdb_max_tables(old_db); table_id++ )
	{
		flat_table* p_old_table = flatdb_table_get( old_db, table_id );
		flat_table* p_table     = flatdb_table_get( new_db, table_id );

		p_table->base            = p_old_table->base;
		p_table->record_size     = p_old_table->record_size;
		p_table->reserved        = 0;
		p_table->first_record    = 0L;
		p_table->deleted_record  = 0L;
		p_table->count           = 0;
		#ifdef _FLAT_TABLE_INCLUDE_NAME
		strncpy( p_table->name, p_old_table->name, FLDB_MAX_TABLE_NAME );
		#endif

		flatdb_table_save( new_db, table_id );


		flat_record* p_record = flatdb_record_first( old_db, table_id );

		while( p_record )
		{
			flatdb_record_add( new_db, table_id, p_record );
			p_record = flatdb_record_next( old_db, table_id, p_record );
		}
	}

	flatdb_close( &new_db );

	fclose( old_db->file );

	if( remove( old_db->filename ) )
	{
		return FALSE;
	}

	if( rename( filename, old_db->filename ) )
	{
		return FALSE;
	}

	old_db->file = fopen( old_db->filename, "rb+" );
	assert( old_db->file );

	if( !old_db->file )
	{
		return FALSE;
	}

	result = flatdb_load_file( old_db );

	return result;
}
#endif

boolean record_lock( flatdb_t db, offset_t position, size_t object_size, short type /* = F_RDLCK, F_WRLCK */ )
{
	struct flock lock;

	lock.l_type   = type;
	lock.l_whence = SEEK_SET;
	lock.l_start  = position;
	lock.l_len    = object_size;

	return fcntl( fileno(db->file), F_SETLKW, &lock ) >= 0;
}

boolean record_unlock( flatdb_t db, offset_t position, size_t object_size )
{
	struct flock lock;

	lock.l_type   = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start  = position;
	lock.l_len    = object_size;

	return fcntl( fileno(db->file), F_SETLKW, &lock ) >= 0;
}

boolean flatdb_read( flatdb_t db, offset_t position, flat_object *p_obj, size_t object_size )
{
	boolean result = FALSE;

	if( db )
	{
		if( record_lock( db, position, object_size, F_RDLCK ) )
		{
			if( fseek( db->file, position, SEEK_SET ) == 0 )
			{
				size_t items_read = fread( p_obj, object_size, 1, db->file );
				if( items_read == 1 )
				{
					result = TRUE;
				}
			}

			record_unlock( db, position, object_size );
		}
	}

	return result;
}

boolean flatdb_write( flatdb_t db, offset_t position, const flat_object *p_obj, size_t object_size )
{
	boolean result = FALSE;

	if( db )
	{
		if( record_lock( db, position, object_size, F_WRLCK ) )
		{
			if( fseek( db->file, position, SEEK_SET ) == 0 )
			{
				size_t items_written = fwrite( p_obj, object_size, 1, db->file );
				if( items_written == 1 )
				{
					result = TRUE;
				}
			}

			record_unlock( db, position, object_size );
		}
	}

	return result;
}

boolean flatdb_file_exists( const tchar *filename )
{
	boolean result = FALSE;
	FILE *f;

	f = fopen( (char *) filename, "rb" );

	if( f )
	{
		fclose( f );
		result = TRUE;
	}

	return result;
}

boolean flatdb_load_file( flatdb_t db )
{
	boolean result = TRUE;

	memset( &db->header, 0, sizeof(db->header) );

	if( fread( &db->header, sizeof(db->header), 1, db->file ) != 1 )
	{
		result = FALSE;
		goto done;
	}


	if( db->tables )
	{
		free( db->tables );
	}

	if( db->indices )
	{
		free( db->indices );
	}

	if( db->hashers )
	{
		free( db->hashers );
	}

	if( db->comparers )
	{
		free( db->comparers );
	}

	db->tables    = malloc( flatdb_max_tables(db) * sizeof(flat_table) );
	db->indices   = malloc( flatdb_max_tables(db) * flatdb_max_records(db) * sizeof(offset_t) );
	db->hashers   = malloc( flatdb_max_tables(db) * sizeof(flat_hasher) );
	db->comparers = malloc( flatdb_max_tables(db) * sizeof(flat_comparer) );

	memset( db->tables, 0, flatdb_max_tables(db) * sizeof(flat_table) );
	memset( db->indices, 0, flatdb_max_tables(db) * flatdb_max_records(db) * sizeof(offset_t) );
	memset( db->hashers, 0, flatdb_max_tables(db) * sizeof(flat_hasher) );


	if( memcmp( db->header.marker, FLDB_MARKER, 4 ) != 0 )
	{
		/* Not a FLDB file */
		result = FALSE;
		goto done;
	}

	if( db->header.max_tables > FLDB_MAX_TABLES )
	{
		result = FALSE;
		goto done;
	}

	/* // this is a pointless check -- it's always false
	if( db->header.max_records > FLDB_MAX_RECORDS )
	{
		result = FALSE;
		goto done;
	}
	*/

	if( fread( db->tables, flatdb_max_tables(db) * sizeof(flat_table), 1, db->file ) != 1 )
	{
		result = FALSE;
		goto done;
	}

	if( fread( db->indices, flatdb_indices_size(db), 1, db->file ) != 1 )
	{
		result = FALSE;
		goto done;
	}

	/* The db->hashers are not stored on disk. They
 	 * must be set at run-time.
     */

done:
	return result;
}

boolean flatdb_create_empty_database( flatdb_t db, uint16_t max_tables, uint16_t max_records )
{
	boolean result = TRUE;
	flat_id_t table_id;
	flatdb_header *p_header;

	if( !db || !db->file )
	{
		result = FALSE;
		goto done;
	}

	if( max_tables > FLDB_MAX_TABLES || max_tables <= 0 )
	{
		max_tables = FLDB_MAX_TABLES;
	}

	/* // this is a pointless check -- it's always false
	if( max_records > FLDB_MAX_RECORDS || max_records <= 0 )
	{
		max_records = FLDB_MAX_RECORDS;
	}
	*/

	p_header = &db->header;

	memcpy( p_header->marker, FLDB_MARKER, sizeof(p_header->marker) );
	p_header->version.major = FLDB_MAJOR_VERSION;
	p_header->version.minor = FLDB_MINOR_VERSION;
	p_header->max_tables    = max_tables;
	p_header->max_records   = max_records;

	fseek( db->file, 0, SEEK_SET );

	if( fwrite( p_header, sizeof(flatdb_header), 1, db->file ) != 1 )
	{
		result = FALSE;
		goto done;
	}

	db->tables = malloc( flatdb_max_tables(db) * sizeof(flat_table) );
	if( !db->tables )
	{
		result = FALSE;
		goto done;
	}

	db->indices = malloc( flatdb_max_tables(db) * flatdb_max_records(db) * sizeof(offset_t) );
	if( !db->indices )
	{
		free( db->tables );
		result = FALSE;
		goto done;
	}

	db->hashers = malloc( flatdb_max_tables(db) * sizeof(flat_hasher) );
	if( !db->hashers )
	{
		free( db->tables );
		free( db->indices );
		result = FALSE;
		goto done;
	}

	db->comparers = malloc( flatdb_max_tables(db) * sizeof(flat_comparer) );
	if( !db->comparers )
	{
		free( db->tables );
		free( db->indices );
		result = FALSE;
		goto done;
	}

	for( table_id = 0; table_id < flatdb_max_tables(db); table_id++ )
	{
		offset_t position = ftell( db->file );
		flat_table *p_table;

		if( position < 0 )
		{
			free( db->tables );
			free( db->indices );
			free( db->hashers );
			free( db->comparers );
			result = FALSE;
			goto done;
		}

		p_table = flatdb_table_get( db, table_id );

		/* initialize with defaults */
		p_table->base.flags      = FLDB_UNUSED | FLDB_TABLE_TYPE;
		p_table->base.id         = table_id;
		p_table->record_size     = sizeof(flat_record);
		p_table->reserved        = 0;
		p_table->first_record    = 0L;
		p_table->count           = 0;
		#ifdef _FLAT_TABLE_INCLUDE_NAME
		memset( p_table->name, 0, FLDB_MAX_TABLE_NAME );
		#endif

		if( fwrite( p_table, sizeof(flat_table), 1, db->file ) != 1 )
		{
			free( db->tables );
			free( db->indices );
			free( db->hashers );
			free( db->comparers );
			result = FALSE;
			goto done;
		}
	}

	memset( db->indices, 0, flatdb_indices_size(db) );

	if( fwrite( db->indices, flatdb_indices_size(db), 1, db->file ) != 1 )
	{
		free( db->tables );
		free( db->indices );
		result = FALSE;
		goto done;
	}

done:
	return result;
}

boolean flatdb_next_id( flatdb_t db, flat_id_t table_id, const flat_record *p_record, flat_id_t *p_next_record_id )
{
	flat_hasher hash_func;
	boolean result = FALSE;

	assert( p_next_record_id );
	assert( table_id < flatdb_max_tables(db) );

	hash_func = db->hashers[ table_id ];

	if( hash_func )
	{
		const size_t MAX    = flatdb_max_records( db );
		size_t hash         = hash_func( p_record );
		flat_id_t count;

		for( count = 0; count < MAX && !result; count++ )
		{
			flat_id_t record_id = (hash + count) % MAX;

			if( db->indices[ table_id * flatdb_max_records(db) + record_id ] == 0 )
			{
				*p_next_record_id = record_id;
				result = TRUE;
			}
		}
	}
	else /* fallback on linear search */
	{
		const size_t MAX = flatdb_max_records(db);
		flat_id_t record_id;

		for( record_id = 0; record_id < MAX && !result; record_id++ )
		{
			if( db->indices[ table_id * flatdb_max_records(db) + record_id ] == 0 )
			{
				*p_next_record_id = record_id;
				result = TRUE;
			}
		}
	}

	return result;
}

boolean flatdb_table_create( flatdb_t db, flat_id_t *p_table_id )
{
	boolean result = FALSE;
	flat_id_t table_id;

	if( !db || !p_table_id )
	{
		result = FALSE;
		goto done;
	}

	for( table_id = 0; !result && table_id < flatdb_max_tables(db); table_id++ )
	{
		flat_table *p_table = flatdb_table_get( db, table_id );

		if( flat_object_is(p_table, FLDB_UNUSED) )
		{
			#ifdef _FLAT_TABLE_INCLUDE_NAME
			tstrncpy( p_table->name, name, FLDB_MAX_TABLE_NAME );
			p_table->name[ FLDB_MAX_TABLE_NAME - 1 ] = '\0';
			#endif

			flat_object_unset( p_table, FLDB_UNUSED );
			*p_table_id = table_id;

			assert( flat_object_id(p_table) == table_id );
			result = TRUE;
		}
	}

done:
	return result;
}

boolean flatdb_table_delete( flatdb_t db, flat_id_t table_id )
{
	boolean result = TRUE;

	flat_table *p_table = flatdb_table_get( db, table_id );

 	/* Delete all records that belong to table_id */
	#if 0
	flat_record *p_record = flatdb_record_first( db, table_id );

	while( p_record )
	{
		flat_id_t id = flat_object_id(p_record);
		p_record = flatdb_record_next( db, table_id, p_record );

		flatdb_record_delete( db, table_id, id );
	}
	#endif

 	/* Reset table data and set UNUSED flag. */
	if( result )
	{
		p_table->base.flags      = FLDB_UNUSED | FLDB_TABLE_TYPE;
		p_table->base.id         = table_id;
		p_table->record_size     = 0;
		p_table->reserved        = 0;
		p_table->first_record    = 0L;
		p_table->count           = 0;
		#ifdef _FLAT_TABLE_INCLUDE_NAME
		memset( p_table->name, 0, FLDB_MAX_TABLE_NAME );
		#endif

		flatdb_table_save( db, table_id );
	}

	/* Shrink file to physically remove deleted records. */
	result = flatdb_shrink( db );

	return result;
}

boolean flatdb_table_save( flatdb_t db, flat_id_t table_id )
{
	return flatdb_write( db,
			flatdb_table_position(db, table_id),
			(const flat_object *) &db->tables[ table_id ],
			sizeof(flat_table) );
}

flat_table* flatdb_table_get( flatdb_t db, flat_id_t table_id )
{
	flat_table *p_table;
	assert( table_id < flatdb_max_tables(db) );
	p_table = &db->tables[ table_id ];
	return p_table;
}

boolean flatdb_record_add( flatdb_t db, flat_id_t table_id, flat_record *p_record )
{
	boolean result      = FALSE;
	flat_table *p_table = flatdb_table_get( db, table_id );
	flat_id_t next_record_id;
	offset_t start_position;
	offset_t next;
	offset_t prev;

	if( p_table->count >= flatdb_max_records(db) )
	{
		/* Cannot add anymore records to this table */
		result = FALSE;
		goto done;
	}


	/* Reuse any previously deleted record */
	if( p_table->deleted_record > 0L )
	{
		flat_record deleted_record;
		offset_t next_deleted_record;

		flatdb_read( db, p_table->deleted_record, (flat_object *) &deleted_record, sizeof(flat_record) );

		assert( flat_object_is( &deleted_record, FLDB_UNUSED ) );
		assert( flat_object_id( &deleted_record ) != 0 );

		start_position      = p_table->deleted_record;
		next_deleted_record = deleted_record.next;

		if( next_deleted_record > 0 )
		{
			p_table->deleted_record = next_deleted_record;
		}

		/* Reuse record id */
		assert( flatdb_index_get( db, table_id, flat_object_id(&deleted_record) ) == start_position );
		assert( flat_object_id(p_record) < flatdb_max_records(db) );
	}
	else if( flatdb_next_id( db, table_id, p_record, &next_record_id ) )
	{
		/* Jump to the end of the file and find where to place the record */
		fseek( db->file, 0L, SEEK_END );
		start_position = ftell( db->file );

		/* Assign new record id */
		p_record->base.id = next_record_id;
	}
	else
	{
		/* This case should be handled above.
         * Cannot add anymore records to this table
		 */
		result = FALSE;
		goto done;
	}

	flat_object_set( p_record, FLDB_RECORD_TYPE );

	/* We must copy the next/prev offsets because iterating
 	 * depends on these values remaining consistent, but
 	 * inserting a new record requires these values be
 	 * changed.
 	 */
	next = p_record->next;
	prev = p_record->prev;

	if( p_table->first_record == 0L )
	{
		/* Adding first record */
		p_table->first_record = start_position;
		p_record->next = 0L;
		p_record->prev = 0L;
	}
	else
	{
		flat_record former_first_record;

		flatdb_read( db, p_table->first_record, (flat_object *) &former_first_record, sizeof(flat_record) );

		former_first_record.prev = start_position;

		flatdb_write( db, p_table->first_record, (const flat_object *) &former_first_record, sizeof(flat_record) );

		p_record->next = p_table->first_record;
		p_record->prev = 0L;
		p_table->first_record = start_position;
	}

	result = flatdb_write( db, start_position, (const flat_object *) p_record, p_table->record_size );

	/* restore next/prev so that we don't
 	 * mess up iterating.
 	 */
	p_record->next = next;
	p_record->prev = prev;

	if( result )
	{
		flatdb_index_update( db, table_id, flat_object_id(p_record), start_position );
		p_table->count++;
		flatdb_table_save( db, table_id );
	}

done:
	return result;
}

boolean flatdb_record_delete( flatdb_t db, flat_id_t table_id, flat_id_t record_id )
{
	boolean result = FALSE;
	flat_table *p_table;
	offset_t record_pos;

	assert( table_id < flatdb_max_tables(db) );
	assert( record_id < flatdb_max_records(db) );

	p_table = flatdb_table_get( db, table_id );
	record_pos = flatdb_record_position( db, table_id, record_id );

	if( record_pos )
	{
		flat_record *p_record = alloc_record( p_table );
		if( !flatdb_read( db, record_pos, (flat_object *) p_record, sizeof(flat_record) ) )
		{
			destroy_record( p_record );
			result = FALSE;
			goto done;
		}

		if( flat_object_not( p_record, FLDB_UNUSED ) )
		{
			flat_record previous_record;

			/* mark the record as deleted (i.e. unused) */
			flat_object_set( p_record, FLDB_UNUSED );

			flatdb_read( db, p_record->prev, (flat_object *) &previous_record, sizeof(flat_record) );
			previous_record.next = p_record->next;
			flatdb_write( db, p_record->prev, (const flat_object *) &previous_record, sizeof(flat_record) );


			p_record->prev = 0L;
			p_record->next = p_table->deleted_record;

			p_table->deleted_record = record_pos;

			/* reset index */
			/*flatdb_index_update( db, table_id, record_id, 0L );*/

			flatdb_write( db, record_pos, (const flat_object *) p_record, sizeof(flat_record) );
			flatdb_table_save( db, table_id );
			result = TRUE;
		}

		destroy_record( p_record );
	}

done:
	return result;
}

flat_record* flatdb_record_get( flatdb_t db, flat_id_t table_id, flat_id_t record_id )
{
	flat_table *p_table;
	offset_t record_pos;

	assert( table_id < flatdb_max_tables(db) );
	assert( record_id < flatdb_max_records(db) );

	p_table = flatdb_table_get( db, table_id );
	record_pos = flatdb_record_position( db, table_id, record_id );

	if( record_pos )
	{
		flat_record *p_record = alloc_record( p_table );
		if( flatdb_read( db, record_pos, (flat_object *) p_record, p_table->record_size ) )
		{
			return p_record;
		}
		else
		{
			destroy_record( p_record );
		}
	}

	return NULL;
}

boolean flatdb_record_save( flatdb_t db, flat_id_t table_id, flat_record *p_record )
{
	flat_table *p_table = flatdb_table_get( db, table_id );

	return flatdb_write( db,
			flatdb_record_position( db, table_id, flat_object_id(p_record) ),
			(const flat_object *) p_record,
			p_table->record_size );
}

#ifndef FLDB_NO_COPY_ON_SEARCH
boolean flatdb_record_search( flatdb_t db, flat_id_t table_id, flat_record *p_record, flat_id_t *p_id )
#else
boolean flatdb_record_search( flatdb_t db, flat_id_t table_id, const flat_record *p_record, flat_id_t *p_id )
#endif
{
	flat_hasher hash_func;
	flat_comparer compare_func;
	boolean result;
	flat_table *p_table;

	assert( table_id < flatdb_max_tables(db) );

	hash_func    = db->hashers[ table_id ];
	compare_func = db->comparers[ table_id ];
	result       = FALSE;
	#ifndef FLDB_NO_COPY_ON_SEARCH
	p_table      = flatdb_table_get( db, table_id );
	#endif

	if( hash_func )
	{
		const size_t MAX    = flatdb_max_records( db );
		size_t hash         = hash_func( p_record );
		flat_id_t count;

		for( count = 0; count < MAX && !result; count++ )
		{
			flat_id_t record_id    = (hash + count) % MAX;
			flat_record *p_current = flatdb_record_get( db, table_id, record_id ); /* allocates memory */

			if( p_current )
			{
				if( compare_func( p_current, p_record ) == 0 )
				{
					#ifndef FLDB_NO_COPY_ON_SEARCH
					memcpy( p_record, p_current, p_table->record_size );
					#endif

					*p_id  = flat_object_id( p_current );
					result = TRUE;
				}
				else
				{
					destroy_record( p_current );
				}
			}
		}
	}
	else /* fallback on linear search */
	{
		flat_record *p_current = flatdb_record_first( db, table_id );

		while( p_current && !result )
		{
			if( compare_func( p_current, p_record ) == 0 )
			{
				#ifndef FLDB_NO_COPY_ON_SEARCH
				memcpy( p_record, p_current, p_table->record_size );
				#endif

				*p_id  = flat_object_id( p_current );
				result = TRUE;
			}
			else
			{
				p_current = flatdb_record_next( db, table_id, p_current );
			}
		}
	}

	return result;

}

void flatdb_record_hasher( flatdb_t db, flat_id_t table_id, flat_hasher hash_func )
{
	assert( table_id < flatdb_max_tables(db) );
	db->hashers[ table_id ] = hash_func;
}

void flatdb_record_comparer( flatdb_t db, flat_id_t table_id, flat_comparer compare_func )
{
	assert( table_id < flatdb_max_tables(db) );
	db->comparers[ table_id ] = compare_func;
}

flat_record* flatdb_record_first( flatdb_t db, flat_id_t table_id )
{
	flat_table *p_table;
	offset_t record_pos;

	assert( table_id < flatdb_max_tables(db) );

	p_table = flatdb_table_get( db, table_id );
	record_pos = p_table->first_record;

	if( record_pos )
	{
		flat_record *p_record = alloc_record( p_table );
		if( flatdb_read( db, record_pos, (flat_object *) p_record, p_table->record_size ) )
		{
			return p_record;
		}
		else
		{
			destroy_record( p_record );
		}
	}

	return NULL;
}

flat_record* flatdb_record_next( flatdb_t db, flat_id_t table_id, flat_record *p_record )
{
	flat_table *p_table;
	offset_t record_pos;

	assert( table_id < flatdb_max_tables(db) );

	p_table = flatdb_table_get( db, table_id );
	record_pos = p_record->next;

	if( record_pos )
	{
		if( flatdb_read( db, record_pos, (flat_object *) p_record, p_table->record_size ) )
		{
			return p_record;
		}
	}

	destroy_record( p_record );
	return NULL;
}

flat_record* flatdb_record_prev( flatdb_t db, flat_id_t table_id, flat_record *p_record )
{
	flat_table *p_table;
	offset_t record_pos;

	assert( table_id < flatdb_max_tables(db) );

	p_table = flatdb_table_get( db, table_id );
	record_pos = p_record->prev;

	if( record_pos )
	{
		if( flatdb_read( db, record_pos, (flat_object *) p_record, p_table->record_size ) )
		{
			return p_record;
		}
	}

	destroy_record( p_record );
	return NULL;
}

boolean flatdb_index_update( flatdb_t db, flat_id_t table_id, flat_id_t record_id, offset_t offset )
{
	boolean result = FALSE;
	assert( table_id < flatdb_max_tables(db) );
	assert( record_id < flatdb_max_records(db) );

	db->indices[ table_id * flatdb_max_records(db) + record_id ] = offset;

	fseek( db->file, flatdb_index_position(db, table_id, record_id), SEEK_SET );

	if( fwrite( &db->indices[ table_id * flatdb_max_records(db) + record_id ], sizeof(offset_t), 1, db->file ) == 1 )
	{
		result = TRUE;
	}

	return result;
}

offset_t flatdb_index_get( flatdb_t db, flat_id_t table_id, flat_id_t record_id )
{
	assert( table_id < flatdb_max_tables(db) );
	assert( record_id < flatdb_max_records(db) );

	return db->indices[ table_id * flatdb_max_records(db) + record_id ];
}


boolean file_copy( FILE *dst, FILE *src )
{
	static byte buffer[ 4096 ];
	boolean result = TRUE;
	offset_t dst_position;
	offset_t src_position;

	if( !dst || !src )
	{
		result = FALSE;
		goto done;
	}

	memset( buffer, 0, sizeof(buffer) );
	dst_position = ftell( dst );
	src_position = ftell( src );

	if( fseek( dst, 0L, SEEK_SET ) )
	{
		result = FALSE;
		goto done;
	}
	if( fseek( src, 0L, SEEK_SET ) )
	{
		result = FALSE;
		goto done;
	}

	while( !feof(src) )
	{
		size_t bytes_read = fread( buffer, sizeof(byte), sizeof(buffer), src );

		if( bytes_read > 0 )
		{
			size_t bytes_written = fwrite( buffer, sizeof(byte), bytes_read, dst );

			if( bytes_written != bytes_read )
			{
				result = FALSE;
				goto done;
			}
		}
	}

	fseek( dst, dst_position, SEEK_SET );
	fseek( src, src_position, SEEK_SET );

done:
	return result;
}
