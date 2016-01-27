#include "main.h"

void wtf( int line_no, char * file_name )
{
	fprintf( stderr, "[WTF] Ошибка на строке %d в файле %s.\n", line_no, file_name );
	exit( EXIT_WTF );
}

void todo( int line_no, char * file_name, char * message )
{
	fprintf( stderr, "[TODO] Файл %s, строка %d: %s\n", file_name, line_no, message );
	exit( EXIT_TODO );
}
