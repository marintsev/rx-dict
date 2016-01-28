#include "main.h"

void wtf( int line_no, char * file_name )
{
    fflush(stdout);
	fprintf( stderr, "[WTF] Ошибка в файл %s на строке %d.\n", file_name, line_no );
	fflush(stderr);
	exit( EXIT_WTF );
}

void todo( int line_no, char * file_name, char * message )
{
    fflush(stdout);
	fprintf( stderr, "[TODO] Файл %s, строка %d: %s\n", file_name, line_no, message );
	fflush(stderr);
	exit( EXIT_TODO );
}
