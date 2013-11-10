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
#ifndef _FLATDB_H_
#define _FLATDB_H_
#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <limits.h>
#include "tstring.h"
#include "types.h"

#ifndef FLDB_MARKER
#define FLDB_MARKER               ("\xF1\x47\xDB\x00")
#endif

#define  FLDB_MAJOR_VERSION       (0)
#define  FLDB_MINOR_VERSION       (1)
#define  FLDB_MAX_TABLES          (8)
#define  FLDB_MAX_RECORDS         (USHRT_MAX)

#ifdef _FLAT_TABLE_INCLUDE_NAME
#ifndef  FLDB_MAX_TABLE_NAME
#define  FLDB_MAX_TABLE_NAME      (20)
#endif
#endif

#define  FLDB_NONE           (0x00000000)
#define  FLDB_TABLE_TYPE     (0x00000001)
#define  FLDB_RECORD_TYPE    (0x00000002)
#define  FLDB_AUX_01         (0x00000004)
#define  FLDB_AUX_02         (0x00000008)
#define  FLDB_AUX_03         (0x00000010)
#define  FLDB_AUX_04         (0x00000020)
#define  FLDB_AUX_05         (0x00000040)
#define  FLDB_AUX_06         (0x00000080)
#define  FLDB_AUX_07         (0x00000100)
#define  FLDB_AUX_08         (0x00000200)
#define  FLDB_AUX_09         (0x00000400)
#define  FLDB_AUX_10         (0x00000800)
#define  FLDB_AUX_11         (0x00001000)
#define  FLDB_AUX_12         (0x00002000)
#define  FLDB_AUX_13         (0x00004000)
#define  FLDB_AUX_14         (0x00008000)
#define  FLDB_AUX_15         (0x00010000)
#define  FLDB_AUX_16         (0x00020000)
#define  FLDB_AUX_17         (0x00040000)
#define  FLDB_AUX_18         (0x00080000)
#define  FLDB_AUX_19         (0x00100000)
#define  FLDB_AUX_20         (0x00200000)
#define  FLDB_AUX_21         (0x00400000)
#define  FLDB_AUX_22         (0x00800000)
#define  FLDB_AUX_23         (0x01000000)
#define  FLDB_AUX_24         (0x02000000)
#define  FLDB_AUX_25         (0x04000000)
#define  FLDB_AUX_26         (0x08000000)
#define  FLDB_AUX_27         (0x10000000)
#define  FLDB_AUX_28         (0x20000000)
#define  FLDB_AUX_29         (0x40000000)
#define  FLDB_UNUSED         (0x80000000)

#define to_flat_object(p_obj)               ((flat_object *) (p_obj))
#define flat_object_is(p_obj, flag)         ((to_flat_object(p_obj)->flags & (flag)) != 0)
#define flat_object_not(p_obj, flag)        ((to_flat_object(p_obj)->flags & (flag)) == 0)
#define flat_object_toggle( p_obj, flag )   (to_flat_object(p_obj)->flags ^= (flag))
#define flat_object_set( p_obj, flag )      (to_flat_object(p_obj)->flags |= (flag))
#define flat_object_unset( p_obj, flag )    (to_flat_object(p_obj)->flags &= ~(flag))
#define flat_object_clear( p_obj )          (to_flat_object(p_obj)->flags = 0)
#define flat_object_id( p_obj )             (to_flat_object(p_obj)->id)

typedef uint16_t flat_id_t;
typedef int64_t  offset_t;
typedef uint32_t flag_t;


#pragma pack(push, 1)
typedef struct _flat_object {
	flag_t    flags;   /* type of object, et cetera */
	flat_id_t id;
} flat_object; /* 6 bytes */

typedef struct _flat_table {
	flat_object base;
	uint32_t    record_size;
	uint32_t    reserved;
	offset_t    first_record;
	offset_t    deleted_record; /* first unused record */
	uint16_t    count;
	#ifdef _FLAT_TABLE_INCLUDE_NAME
	tchar       name[ FLDB_MAX_TABLE_NAME ];
	#endif
} flat_table; /* 34 bytes */

typedef struct _flat_record {
	flat_object base;
	offset_t    next;
	offset_t    prev; /* not used in deleted records */
} flat_record; /* 22 bytes */

typedef struct flatdb_version {
	uint16_t major;
	uint16_t minor;
} flatdb_version;

typedef struct _flatdb_header {
	byte           marker[ 4 ]; /* usually 0xF147DB00 */
	flatdb_version version;
	uint16_t       max_tables;
	uint16_t       max_records;
} flatdb_header; /* 8 bytes */
#pragma pack(pop)

typedef size_t (*flat_hasher)   ( const flat_record *p_record );
typedef int    (*flat_comparer) ( const flat_record *p_left, const flat_record *p_right );

typedef struct _flatdb {
	FILE*          file;	      /* not written to disk */
	tchar*         filename;   /* not written to disk */
	flat_hasher*   hashers;    /* not written to disk */
	flat_comparer* comparers;  /* not written to disk */

	flatdb_header header;
	flat_table*   tables;
	offset_t*     indices;
} flatdb;


typedef flatdb * flatdb_t;

flatdb_t      flatdb_open            ( const tchar *filename );
flatdb_t      flatdb_create          ( const tchar *filename, uint16_t max_tables, uint16_t max_records );
void          flatdb_close           ( flatdb_t *db );
uint16_t      flatdb_max_tables      ( flatdb_t db );
uint16_t      flatdb_max_records     ( flatdb_t db );
const tchar*  flatdb_filename        ( flatdb_t db );
boolean       flatdb_shrink          ( flatdb_t db );
boolean       flatdb_read            ( flatdb_t db, offset_t position, flat_object *p_obj, size_t object_size );
boolean       flatdb_write           ( flatdb_t db, offset_t position, const flat_object *p_obj, size_t object_size );
boolean       flatdb_table_create    ( flatdb_t db, flat_id_t *p_table_id );
boolean       flatdb_table_delete    ( flatdb_t db, flat_id_t table_id );
boolean       flatdb_table_save      ( flatdb_t db, flat_id_t table_id );
flat_table*   flatdb_table_get       ( flatdb_t db, flat_id_t table_id );
boolean       flatdb_record_add      ( flatdb_t db, flat_id_t table_id, flat_record *p_record );
boolean       flatdb_record_delete   ( flatdb_t db, flat_id_t table_id, flat_id_t record_id );
flat_record*  flatdb_record_get      ( flatdb_t db, flat_id_t table_id, flat_id_t record_id ); /* allocates memory */
boolean       flatdb_record_save     ( flatdb_t db, flat_id_t table_id, flat_record *p_record );
#ifndef FLDB_NO_COPY_ON_SEARCH
boolean       flatdb_record_search   ( flatdb_t db, flat_id_t table_id, flat_record *p_record, flat_id_t *p_id );
#else
boolean       flatdb_record_search   ( flatdb_t db, flat_id_t table_id, const flat_record *p_record, flat_id_t *p_id );
#endif
void          flatdb_record_hasher   ( flatdb_t db, flat_id_t table_id, flat_hasher hash_func );
void          flatdb_record_comparer ( flatdb_t db, flat_id_t table_id, flat_comparer compare_func );
flat_record*  flatdb_record_first    ( flatdb_t db, flat_id_t table_id ); /* allocates memory */
flat_record*  flatdb_record_next     ( flatdb_t db, flat_id_t table_id, flat_record *p_record );
flat_record*  flatdb_record_prev     ( flatdb_t db, flat_id_t table_id, flat_record *p_record );
boolean       flatdb_index_update    ( flatdb_t db, flat_id_t table_id, flat_id_t record_id, offset_t offset );
offset_t      flatdb_index_get       ( flatdb_t db, flat_id_t table_id, flat_id_t record_id );

#define       flatdb_create_simple( filename )  flatdb_create(filename, 1, FLDB_MAX_RECORDS )


#ifdef __cplusplus
}
#endif 
#endif /* _FLATDB_H_ */
