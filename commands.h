#ifndef COMMANDS_H
#define COMMANDS_H

#include "main.h"

extern void command_defragment(FILE * f);
extern void command_help();
extern void command_add(FILE * f);
extern int remove_words(FILE * f, char * word);
extern void command_find(FILE * f);
extern void command_remove(FILE * f);

#endif
