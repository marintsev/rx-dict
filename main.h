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

#define EXIT_WRONG_SIZEOF (1)
#define EXIT_WRONG_USAGE  (2)
#define EXIT_WTF		  (3)
#define EXIT_TODO		  (4)
#define EXIT_WRONG_RIGHTS (5)

#define HEADER_SIZE	(4 * 3)

#include "log.h"

// TODO: перетащить в header.c

struct header_t
{
	uint32 version;      // сейчас 1
	uint32 total_words;	 // количество даже удалённых слов
	uint32 actual_words; // количество неудалённых слов
};

extern struct header_t header;

#endif

