#ifndef DB_H
#define DB_H

#include "main.h"

extern void write_header(struct header_t * header, FILE * f);
extern uint64 jump_to_first_word(FILE * f);
extern uint64 find_word(FILE * f, char * pattern);
extern void add_word(FILE * f, char * word, char * content);

#endif
