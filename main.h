#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define uint32 unsigned int
#define uint64 unsigned long

#define WTF() wtf( __LINE__ )
#define TODO( message ) todo( __LINE__, message )

#define EXIT_WRONG_SIZEOF (1)
#define EXIT_WRONG_USAGE  (2)
#define EXIT_WTF		  (3)
#define EXIT_TODO		  (4)
#define EXIT_WRONG_RIGHTS (5)

#endif

