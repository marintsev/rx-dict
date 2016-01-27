#include "log.h"

void wtf( int line_no )
{
	fprintf( stderr, "[WTF] Ошибка на строке %d.\n", line_no );
	exit( EXIT_WTF );
}

void todo( int line_no, char * message )
{
	fprintf( stderr, "[TODO] Строка %d: %s\n", line_no, message );
	exit( EXIT_TODO );
}
