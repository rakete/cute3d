#include <locale.h>
#include <assert.h>
#include <tstring.h>

#ifndef UNICODE
#error "The tstring tests assume unicode and C99 support"
#endif

int main( int argc, char *argv[] )
{
	char  ascii[ 128 ];
	tchar unicode[ 128 ];

	TERM_BEG(ascii);
	TERM_BEG(unicode);

	if( !setlocale(LC_CTYPE, "en_US.utf-8") )
	{
		fprintf(stderr, "Can't set the specified locale! "
				"Check LANG, LC_CTYPE, LC_ALL.\n");
		return 1;
	}


	tstrcat( unicode, _T("Hello") );
	tstrcat( unicode, _T(" ") );
	tstrcat( unicode, _T("World!") );
	tprintf( _T("string concatenation test: %S\n"), tstrcmp( unicode, _T("Hello World!") ) == 0 ? _T("passed") : _T("failed") );


	unicode_to_ascii( ascii, unicode, sizeof(ascii) );
	TERM_END( ascii );
	tprintf( _T("unicode to ascii test: %S\n"), strcmp( ascii, "Hello World!" ) == 0 ? _T("passed") : _T("failed") );


	const tchar *russian_text_utf8  = _T("В России тест принадлежит вам.");
	tprintf( _T(" UTF-8 Russian text = %S\n"), russian_text_utf8 );

	lc_tstring_t ltext;
	lc_tstring_t rtext;
	tstring_create( &ltext, _T("   \t\n \r This is a test of ltrim().") );
	tstring_create( &rtext, _T("This is a test of rtrim().   \n\t\t \r ") );

	size_t n = 0;

	n = tstring_ltrim( &ltext );
	assert( n == 8 );
	tprintf( _T("[%S] -- %ld chars removed\n"), ltext.s, n );

	n = tstring_rtrim( &rtext );
	assert( n == 9 );
	tprintf( _T("[%S] -- %ld chars removed\n"), rtext.s, n );

	tstring_destroy( &ltext );
	tstring_destroy( &rtext );


	return 0;
}
