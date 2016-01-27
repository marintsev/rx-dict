#ifndef ENTRY_H
#define ENTRY_H

#include "main.h"

/*	
	Слова
	0 word_len			byte		word length
  1-3 content_len		byte[3]		article length
	? word				char[?]		null-terminated word
	? content			char[?]		null-terminated content
	
	Удалённое слово
	0 remove_flag		byte		значение 0
  1-3 remove_length		byte[3]		бывшие ранее word_len+content_len
rem_l ...							мусор
*/
struct entry_t
{
	int	word_len;
	int content_len;
	char * word;
	char * content;
};

#define READ_ENTRY_CONTENT	(1<<0)
#define READ_ENTRY_WORD		(1<<1)
#define READ_ENTRY_DO_SEEK  (1<<2)
#define READ_ENTRY_ALL		(READ_ENTRY_CONTENT | READ_ENTRY_WORD | READ_ENTRY_DO_SEEK)

extern uint64 read_entry( FILE * f, struct entry_t * entry, uint64 offset, int flags );
extern void   write_entry( FILE * f, struct entry_t * entry );
extern int    existent_entry( struct entry_t * entry );
extern void   free_entry( struct entry_t * entry );

#endif

