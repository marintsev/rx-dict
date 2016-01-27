#include "entry.h"

uint64 read_entry( FILE * f, struct entry_t * entry, uint64 offset, int flags )
{
	uint32 temp;
	int code = fread( &temp, 4, 1, f );
	if( 1 != code )
		WTF();
	offset += 4;
		
	entry->word_len = temp & 0xFF;
	entry->content_len = temp >> 8;
	
	// выделяем память под слово и читаем его, устанавливаем \0.
	if( ( flags & READ_ENTRY_WORD ) != 0 )
	{
		entry->word = malloc( entry->word_len+1 );
		code = fread( entry->word, entry->word_len, 1, f );
		if( 1 != code )
			WTF();
		offset += entry->word_len;
		entry->word[entry->word_len] = 0;
	}
	else
	{
		entry->word = NULL;
		offset += entry->word_len;
		code = fseek( f, offset, SEEK_SET );
		if( 0 != code )
			WTF();
	}
	
	// если надо, то читаем статью
	if( ( flags & READ_ENTRY_CONTENT ) != 0 )
	{
		entry->content = malloc( entry->content_len+1 );
		code = fread( entry->content, entry->content_len, 1, f);
		if( 1 != code )
			WTF();
		offset += entry->content_len;
	}
	// или пропускаем её
	else
	{
		entry->content = NULL;
		offset += entry->content_len;
		code = fseek( f, offset, SEEK_SET );
		if( 0 != code )
			WTF();
	}
}

void free_entry( struct entry_t * entry )
{
	free( entry->word );
	free( entry->content );
}
