#include "entry.h"

uint64 read_entry( FILE * f, struct entry_t * entry, uint64 offset, int flags )
{
	int code;
	if( ( flags & READ_ENTRY_DO_SEEK ) != 0 )
	{
		code = fseek( f, offset, SEEK_SET );
		if( 0 != code )
			WTF();
	}

	uint32 temp;
	code = fread( &temp, 4, 1, f );
	if( 1 != code )
		WTF();
	offset += 4;
		
	entry->word_len = temp & 0xFF;
	entry->content_len = temp >> 8;

	// если запись не удалена
	if( 0 != entry->word_len )
	{
		// выделяем память под слово и читаем его, устанавливаем \0.
		if( ( flags & READ_ENTRY_WORD ) != 0 )
		{
			entry->word = malloc( entry->word_len+1 );
			code = fread( entry->word, 1, entry->word_len, f );
			if( entry->word_len != code )
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
			printf( "content_len: %d", entry->content_len );
			entry->content = malloc( entry->content_len+1 );
			code = fread( entry->content, entry->content_len, 1, f);
			if( 1 != code )
				WTF();
			offset += entry->content_len;
			entry->content[entry->content_len] = 0;
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
	else
	{
		entry->word = NULL;
		entry->content = NULL;
		offset += entry->content_len;
		code = fseek( f, offset, SEEK_SET );
		if( 0 != code )
			WTF();
	}
	return offset;
}

void write_entry( FILE * f, struct entry_t * entry )
{
	assert( entry->word_len <= 128 );
	assert( entry->content_len < 16777216 );
	uint32 temp = (entry->word_len & 0xFF) | ((entry->content_len) << 8);
	int code = fwrite( &temp, 4, 1, f );
	if( 1 != code )
		WTF();

	if( entry->word_len != 0 )
	{	
		code = fwrite( entry->word, entry->word_len, 1, f );
		if( 1 != code )
			WTF();
	
		code = fwrite( entry->content, entry->content_len, 1, f );
		if( 1 != code )
			WTF();
	}
}

void free_entry( struct entry_t * entry )
{
	if( entry->word != NULL )
		free( entry->word );
	if( entry->content != NULL )
		free( entry->content );
}

void mark_deleted_entry( struct entry_t * entry )
{
	free_entry( entry );
	int total_size = entry->word_len + entry->content_len;
	entry->word_len = 0;
	entry->content_len = total_size;
}

int existent_entry( struct entry_t * entry )
{
	return entry->word_len != 0;
}
