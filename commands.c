#include "main.h"

#include "entry.h"

void read_word( char * word, int size )
{
    printf( "Введите слово> " );
    fgets( word, size, stdin );
    remove_newline( word );
}

void command_add( FILE * f )
{
    char word[130];
	read_word( word, 130 );

	uint64 index = find_word( f, word );
	if( 0 == index )
	{
		// Слова нет, можно добавлять
		char content[4096+8];
		printf( "Введите толкование > " );
		// TODO: ввод множества строк
		fgets( content, 4096, stdin );
		remove_newline( content );
		add_word( f, word, content );
	}
	else
	{
		TODO( "Прочитать толкование, запомнить слово, вставить новую, удалить старую." );
	}
}

void command_find( FILE * f )
{
	char word[130];
	read_word( word, 130 );

	uint64 index = find_word( f, word );
	if( 0 == index )
	{
		// слова нет, сообщить
		fprintf( stderr, "INFO: нет такого слова\n" );
	}
	else
	{
		// слово есть, выводим
		fseek( f, index, SEEK_SET );
		struct entry_t entry;
		read_entry( f, &entry, index, READ_ENTRY_CONTENT );
		printf( "Значение: %s\n", entry.content );
	}
}

void command_remove( FILE * f )
{
	TODO( "Считать слово. Пометить все вхождения как удалённые." );
}

void command_defragment( FILE * f )
{
	TODO( "Дефрагментация." );
}