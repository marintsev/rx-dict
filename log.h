#ifndef LOG_H
#define LOG_H

#include <stdlib.h>

#define WTF() wtf( __LINE__, __FILE__ )
#define TODO( message ) todo( __LINE__, __FILE__, message )

extern void wtf( int line_no, char * file_name );
extern void todo( int line_no, char * file_name, char * message );

#endif

