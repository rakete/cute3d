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
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <flat-db.h>
#include <tstring.h>
#include <types.h>

#define CONTACTS_DAT        _T("./contacts.dat")
#define CONTACT_TYPE        FLDB_AUX_01

typedef struct _contact {
	flat_record base;
	tchar first_name[ 50 ];
	tchar last_name[ 50 ];
	tchar middle_name[ 50 ];

	tchar company_name[ 120 ];

	tchar address1[ 80 ];
	tchar address2[ 80 ];
	tchar city[ 50 ];
	tchar state[ 50 ];
	tchar postal_code[ 12 ];

	tchar phone1[ 20 ];
	tchar phone2[ 20 ];

	tchar website[ 256 ];

	time_t date_of_birth;
} contact;

typedef struct _expense {
	flat_record base;
	time_t      date;
	short       type;
	double      cost;
	tchar       description[ 128 ];
} expense;

#define CONTACTS      (0)
#define EXPENSES      (1)

int main( int argc, char *argv[] )
{
	boolean result = TRUE;
	boolean done   = FALSE;
	flatdb_t db    = NULL;
	char selection;


	do {
		printf( "Menu\n-------------------------\n" );

		printf( "   O - Open database\n" );
		printf( "   C - Create database\n" );
		printf( "   A - Add contact record\n" );
		printf( "   D - Delete contact record\n" );
		printf( "   L - List contacts record\n" );
		printf( "   R - Remove database\n" );
		printf( "   X - Exit\n" );
		printf( "\n-------------------------\n" );
		printf( "Selection? " );
		scanf( "%c", &selection );

		switch( selection )
		{
			case 'o':
			case 'O':
			{
				db = flatdb_open( CONTACTS_DAT );
				#if defined(UNICODE)
				printf( "Loaded: %ls\n", flatdb_filename(db) );
				#else
				printf( "Loaded: %s\n", flatdb_filename(db) );
				#endif
				break;
			}
			case 'a':
			case 'A':
			{
				contact c;
				memset( &c, 0, sizeof(contact) );
				#if 0
				char buffer[ 256 ];
				printf( "    First Name: " );  scanf( "%s", buffer );
				strncpy( c.first_name, buffer, sizeof(c.first_name) - 1 ); c.first_name[ sizeof(c.first_name) - 1 ] = '\0';
				printf( "     Last Name: " );  scanf( "%s", buffer );
				strncpy( c.last_name, buffer, sizeof(c.last_name) - 1 ); c.last_name[ sizeof(c.last_name) - 1 ] = '\0';
				printf( "   Middle Name: " );  scanf( "%s", buffer );
				strncpy( c.middle_name, buffer, sizeof(c.middle_name) - 1 ); c.middle_name[ sizeof(c.middle_name) - 1 ] = '\0';
				printf( "       Company: " );  fgets( buffer, sizeof(buffer), stdin );
				strncpy( c.company_name, buffer, sizeof(c.company_name) - 1 ); c.company_name[ sizeof(c.company_name) - 1 ] = '\0';
				printf( "      Address1: " );  scanf( "%s", buffer );
				strncpy( c.address1, buffer, sizeof(c.address1) - 1 ); c.address1[ sizeof(c.address1) - 1 ] = '\0';
				printf( "      Address2: " );  scanf( "%s", buffer );
				strncpy( c.address2, buffer, sizeof(c.address2) - 1 ); c.address2[ sizeof(c.address2) - 1 ] = '\0';
				printf( "          City: " );  scanf( "%s", buffer );
				strncpy( c.city, buffer, sizeof(c.city) - 1 ); c.city[ sizeof(c.city) - 1 ] = '\0';
				printf( "         State: " );  scanf( "%s", buffer );
				strncpy( c.state, buffer, sizeof(c.state) - 1 ); c.state[ sizeof(c.state) - 1 ] = '\0';
				printf( "   Postal Code: " );  scanf( "%s", buffer );
				strncpy( c.postal_code, buffer, sizeof(c.postal_code) - 1 ); c.postal_code[ sizeof(c.postal_code) - 1 ] = '\0';
				printf( "        Phone1: " );  scanf( "%s", buffer );
				strncpy( c.phone1, buffer, sizeof(c.phone1) - 1 ); c.phone1[ sizeof(c.phone1) - 1 ] = '\0';
				printf( "        Phone2: " );  scanf( "%s", buffer );
				strncpy( c.phone2, buffer, sizeof(c.phone2) - 1 ); c.phone2[ sizeof(c.phone2) - 1 ] = '\0';
				printf( "       Website: " );  scanf( "%s", buffer );
				strncpy( c.website, buffer, sizeof(c.website) - 1 ); c.website[ sizeof(c.website) - 1 ] = '\0';
				printf( " Date Of Birth: " );  scanf( "%s", buffer );
				struct tm time_parts; strptime( buffer, "%D", &time_parts);
				c.date_of_birth = mktime( &time_parts );

				result = flatdb_record_add( db, CONTACTS, (flat_record *) &c );
				assert( result );
				#else
				{
					struct tm time_parts;
					memset( &c, 0, sizeof(contact) );
					tstrcpy( c.first_name, _T("Bob") );
					tstrcpy( c.last_name, _T("Dylan") );
					tstrcpy( c.middle_name, _T("") );
					tstrcpy( c.company_name, _T("Acme Inc") );
					tstrcpy( c.address1, _T("940 Appleton Road") );
					tstrcpy( c.address2, _T("Suite 202") );
					tstrcpy( c.city, _T("Anaheim") );
					tstrcpy( c.state, _T("California") );
					tstrcpy( c.postal_code, _T("23021") );
					tstrcpy( c.phone1, _T("(202) 555-4345") );
					tstrcpy( c.phone2, _T("1 (888) 555-1232") );
					tstrcpy( c.website, _T("http://www.acme.com/") );
					strptime( "01/28/1977", "%D", &time_parts);
					c.date_of_birth = mktime( &time_parts );

					result = flatdb_record_add( db, CONTACTS, (flat_record *) &c );
					printf( "Record %d added!\n", flat_object_id(&c) );
					assert( result );
				}

				{
					struct tm time_parts;
					expense e;
					memset( &e, 0, sizeof(expense) );
					e.type = 0;
					e.cost = 4.22;
					tstrcpy( e.description, _T("Gasoline for car.") );
					strptime( "05/8/2011", "%D", &time_parts);
					e.date = mktime( &time_parts );

					result = flatdb_record_add( db, EXPENSES, (flat_record *) &e );
					printf( "Record %d added!\n", flat_object_id(&e) );
					assert( result );
				}

				{
					struct tm time_parts;
					tstrcpy( c.first_name, _T("Joe") );
					tstrcpy( c.last_name, _T("Marrero") );
					tstrcpy( c.middle_name, _T("Alexander") );
					tstrcpy( c.company_name, _T("Shrewd LLC") );
					tstrcpy( c.address1, _T("1940 Madison Street #2") );
					tstrcpy( c.city, _T("Hollywood") );
					tstrcpy( c.state, _T("Florida") );
					tstrcpy( c.postal_code, _T("33020") );
					tstrcpy( c.phone1, _T("(954) 803-9157") );
					tstrcpy( c.website, _T("http://www.manvscode.com/") );
					strptime( "06/15/1984", "%D", &time_parts);
					c.date_of_birth = mktime( &time_parts );

					result = flatdb_record_add( db, CONTACTS, (flat_record *) &c );
					printf( "Record %d added!\n", flat_object_id(&c) );
					assert( result );
				}

				{
					struct tm time_parts;
					expense e;
					memset( &e, 0, sizeof(expense) );
					e.type = 2;
					e.cost = 425.22;
					tstrcpy( e.description, _T("Web hosting at Rackspace.") );
					strptime( "08/3/2011", "%D", &time_parts);
					e.date = mktime( &time_parts );

					result = flatdb_record_add( db, EXPENSES, (flat_record *) &e );
					printf( "Record %d added!\n", flat_object_id(&e) );
					assert( result );
				}

				{
					struct tm time_parts;
					memset( &c, 0, sizeof(contact) );
					tstrcpy( c.first_name, _T("Mary") );
					tstrcpy( c.last_name, _T("Poppins") );
					tstrcpy( c.middle_name, _T("") );
					tstrcpy( c.company_name, _T("Apple Waxxing") );
					tstrcpy( c.city, _T("Miami") );
					tstrcpy( c.state, _T("Florida") );
					tstrcpy( c.postal_code, _T("33402") );
					tstrcpy( c.website, _T("http://www.apple-wax.com/") );
					strptime( "09/18/1969", "%D", &time_parts);
					c.date_of_birth = mktime( &time_parts );

					result = flatdb_record_add( db, CONTACTS, (flat_record *) &c );
					assert( result == FALSE );
				}

				#endif
				break;
			}
			case 'c':
			case 'C':
			{
				#if defined(UNICODE)
				printf( "Created %ls\n", CONTACTS_DAT );
				#else
				printf( "Created %s\n", CONTACTS_DAT );
				#endif
				db = flatdb_create( CONTACTS_DAT, 2, 2 );

				/* create contact table */
				{
					flat_id_t table_id;
					flat_table *p_table;

					result = flatdb_table_create( db, &table_id );
					assert( result );
					assert( table_id == CONTACTS );

					p_table = flatdb_table_get( db, table_id );
					p_table->record_size = sizeof(contact);
					result = flatdb_table_save( db, table_id );
				}

				/* create expense table */
				{
					flat_id_t table_id;
					flat_table *p_table;
					result = flatdb_table_create( db, &table_id );
					assert( result );
					assert( table_id == EXPENSES );

					p_table = flatdb_table_get( db, table_id );
					p_table->record_size = sizeof(expense);
					result = flatdb_table_save( db, table_id );
				}

				assert( result );
				break;
			}
			case 'd':
			case 'D':
			{
				printf( "Deleting expense table.\n" );
				flatdb_table_delete( db, EXPENSES );
				/* flatdb_record_delete( db, CONTACTS, 0 ); */
				break;
			}
			case 'l':
			case 'L':
			{
				flat_table *p_contact_table = flatdb_table_get( db, CONTACTS );
				flat_table *p_expense_table;
				printf( "Contacts (size = %d):\n", p_contact_table->count  );
				{
					flat_record* p_record = flatdb_record_first( db, CONTACTS );

					while( p_record )
					{
						contact *p_contact = (contact *) p_record;
						#if defined(UNICODE)
						printf( "  - %ls %ls\n", p_contact->first_name, p_contact->last_name );
						#else
						printf( "  - %s %s\n", p_contact->first_name, p_contact->last_name );
						#endif

						p_record = flatdb_record_next( db, CONTACTS, p_record );
					}
				}

				p_expense_table = flatdb_table_get( db, EXPENSES );
				printf( "Expenses (size = %d):\n", p_expense_table->count );
				{
					flat_record* p_record = flatdb_record_first( db, EXPENSES );

					while( p_record )
					{
						expense *p_expense = (expense *) p_record;
						#if defined(UNICODE)
						printf( "  - %ls \n", p_expense->description );
						#else
						printf( "  - %s \n", p_expense->description );
						#endif

						p_record = flatdb_record_next( db, EXPENSES, p_record );
					}
				}
				break;
			}
			case 'r':
			case 'R':
			{
				#if defined(UNICODE)
				char  ascii[ 128 ];
				printf( "Removing %ls\n", CONTACTS_DAT );
				unicode_to_ascii( ascii, CONTACTS_DAT, sizeof(ascii) );
				remove( ascii );
				#else
				printf( "Removing %s\n", CONTACTS_DAT );
				remove( CONTACTS_DAT );
				#endif
				break;
			}
			case 'x':
			case 'X':
				done = TRUE;
			default:
				break;
		}
	} while( !done );


	flatdb_close( &db );

	assert( db == NULL );
	return 0;
}
