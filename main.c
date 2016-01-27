#include "main.h"

#include "entry.h"

/*
План:
1. Хэширование (первые четыре символа, например).
2. Собрать все хэши в одном месте.
3. Собрать все хэши, слова и статьи в отдельных страницах (4096 - 65536 байт каждая).
4. Сортировать хэши и слова в страницах.
5. Сделать бинарное отсортированное дерево для хэшей.
*/

#define DEFAULT_DB_NAME		"default.db"

void check()
{
	if( 4 != sizeof( uint32 ) )
	{
		fprintf( stderr, "Размер uint32 не 32 бита, а %d. Надо поправить.\n", sizeof( uint32 ) );
		exit( EXIT_WRONG_SIZEOF );
	}

	if( 8 != sizeof( uint64 ) )
	{
		fprintf( stderr, "Размер uint64 не 64 бита, а %d. Надо поправить.\n", sizeof( uint64 ) );
		exit( EXIT_WRONG_SIZEOF );
	}
}

// Формат базы (little endian).



void write_header( struct header_t * header, FILE * f )
{
	int code = fseek( f, 0, SEEK_SET );
	if( code == -1 )
		WTF();

	code = fwrite( header, HEADER_SIZE, 1, f );
	if( code != 1 )
		WTF();
}

void write_new_header( struct header_t * header, FILE * f )
{
	header->version = 1;
	header->total_words = 0;
	header->actual_words = 0;

	int code;
	write_header( header, f );
	fflush( f );

	code = ftruncate( fileno( f ), HEADER_SIZE );
	if( 0 != code )
	{
		fprintf( stderr, "errno = %d\n", errno );
		WTF();
	}
}

char * db_filename = NULL;

void read_header( struct header_t * header, FILE * f )
{
	int code;
	code = fseek( f, 0, SEEK_SET );
	if( code == -1 )
		WTF();

	code = fread( header, HEADER_SIZE, 1, f );
	if( 1 != code )
		WTF();
}

struct header_t header;

void save_database( FILE * f )
{
	write_header( &header, f );
//	TODO( "save_database\n" );
}

void remove_newline( char * str )
{
	char * p = str;
	while( *p )
		p++;
	p[-1] = 0;
}

uint64 jump_to_first_word( FILE * f )
{
	long offset = HEADER_SIZE;
	int code = fseek( f, offset, SEEK_SET );
	if( 0 != code )
		WTF();
	return offset;
}

/*
	Находит смещение, где находится запись для этого слова.
	TODO: функции работающие с файлами 32-битные.
*/
uint64 find_word( FILE * f, char * pattern )
{
	int i;
	int code;
	uint64 offset = jump_to_first_word( f );
	int start;

	struct entry_t entry;
	for( i=0; i<header.total_words; i++ )
	{
		start = offset;
		// TODO: совсем не обязательно выделять память здесь
		offset = read_entry( f, &entry, start, READ_ENTRY_WORD | READ_ENTRY_DO_SEEK );
		int good = 0;
		if( existent_entry( &entry ) )
		{
			good = strcmp( entry.word, pattern ) == 0;
			free_entry( &entry );
		}

		if( good )
			return start;
	}
	return 0;
}

uint64 skip_to_end( FILE * f )
{
	int i;
	int code;
	int offset = jump_to_first_word( f );
	int start;

	struct entry_t entry;
	for( i=0; i<header.total_words; i++ )
	{
		start = offset;
		// TODO: совсем не обязательно выделять память здесь
		offset = read_entry( f, &entry, start, 0 );
		free_entry( &entry );
	}
	return offset;
}



void add_word( FILE * f, char * word, char * content )
{
	skip_to_end( f );
	struct entry_t entry;
	entry.word_len = strlen( word );
	entry.content_len = strlen( content );
	entry.word = word;
	entry.content = content;
	write_entry( f, &entry );
	header.total_words++;
	header.actual_words++;
	write_header( &header, f );
//	TODO( "Вставить слово." );
}

int main( int argc, char ** argv )
{
	check();

	// Передан файл базы как параметр
	if( argc == 2 )
	{
		db_filename = argv[1];
	}
	// Файл не передан, используется имя по умолчанию
	else if( argc == 1 )
	{
		db_filename = "default.db";
	}
	// Неправильное использование
	else
	{
		fprintf( stderr, "Usage: dictionary [words.db]\n" );
		exit( EXIT_WRONG_USAGE );
	}

	assert( NULL != db_filename );

	FILE * f = NULL;
	int code = access( db_filename, R_OK | W_OK );
	// Нет прав, не существует файл или что-нибудь ещё
	if( -1 == code )
	{
		// Нет файла, можно создавать
		if( errno == ENOENT )
		{
			f = fopen( db_filename, "w+" );
			fprintf( stderr, "INFO: Файл не существовал и теперь создан.\n" );
		}
		else if( errno == EACCES )
		{
			fprintf( stderr, "FATAL: Нет прав доступа к базе.\n" );
			exit( EXIT_WRONG_RIGHTS );
		}
		else
		{
			printf( "errno = %d\n", errno );
			exit( EXIT_WTF );
		}
	}
	// Есть права и файл существует
	else
	{
		f = fopen( db_filename, "r+" );
		fprintf( stderr, "INFO: Существующий файл открыт для работы.\n" );
	}
	assert( NULL != f );

	fseek( f, 0, SEEK_END );
	int length = ftell( f );
	fprintf( stderr, "DEBUG: Длина файла: %d\n", length );


	if( length < HEADER_SIZE )
	{
		// Длина файла заведомо меньше размера header. Пишем новый.
		write_new_header( &header, f );
	}
	else
	{
		// Читаем
		read_header( &header, f );

		// Проверяем
		if( header.version != 1 || header.actual_words > header.total_words )
		{
			fprintf( stderr, "FATAL: База данных (%s), вероятно, испорчена. Возможно, стоит её удалить.\n", db_filename );
			WTF();
		}
	}

	// Всё есть. Можно работать.
	printf( "Введите help для справки, quit для выхода.\n" );
	int working = 1;
	while( working )
	{
		printf( "> " );

		char str[6];
		fgets( str, 6, stdin );
		remove_newline( str );
		if( strcmp( str, "help" ) == 0 )
		{
			printf( "add\tДобавить слово.\n" );
			printf( "find\tНайти слово.\n" );
			printf( "del\tУдалить слово.\n" );
			printf( "frag\tДефрагментация.\n" );
			printf( "quit\tВыход и запись базы.\n" );
		}
		else if( strcmp( str, "quit" ) == 0 ||
				 strcmp( str, "exit" ) == 0 ||
				 strcmp( str, "bye" ) == 0 )
		{
			save_database( f );
			exit( EXIT_SUCCESS );
		}
		else if( strcmp( str, "add" ) == 0 )
		{
			command_add( f );
		}
		else if( strcmp( str, "find" ) == 0 )
		{
			command_find( f );
		}
		else if( strcmp( str, "del" ) == 0 ||
				 strcmp( str, "rm" ) == 0 )
		{
			command_remove( f );
		}
		else if( strcmp( str, "frag" ) == 0 )
		{
			command_defragment( f );
		}
		else
		{
			fprintf( stderr, "ERROR: Неизвестная команда '%s'\n", str );
		}
	}

	/*
	Операции:
	1. Добавить слово. Имя (128 байт), содержание (не ограничено) ->
	2. Найти слово. Имя -> содержание
	3. Удалить. Имя ->
	*/
	if( f != NULL )
		fclose( f );

	return EXIT_SUCCESS;
}
