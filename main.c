#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define uint32 unsigned int

#define DEFAULT_DB_NAME		"default.db"

#define EXIT_WRONG_SIZEOF (1)
#define EXIT_WRONG_USAGE  (2)
#define EXIT_WTF		  (3)

void check()
{
	if( 4 != sizeof( uint32 ) )
	{
		fprintf( stderr, "Размер unsigned int не 32 бита, а %d. Надо поправить.\n", sizeof( uint32 ) );
		exit( EXIT_WRONG_SIZEOF );
	}
}

#define WTF() wtf( __LINE__ )

void wtf( int line_no )
{
	fprintf( stderr, "Ошибка на строке %d.\n", line_no );
	exit( EXIT_WTF );
}

// Формат базы (little endian).

/*
	Заголовок:
	0 version		int32 версия
	4 words			int32 количество слов
	8 first_page	int32 указатель на страницу. адрес = first_page * 4096
*/
#define HEADER_SIZE	(4*3)

struct header_t
{
	uint32 version;
	uint32 words;
	uint32 first_page;
};

/*
	Страница:
	0 words			int32 количество слов в странице
	4 next_page		int32 указатель на следующую страницу, или 0, если последняя.
	
	Слово
	0 byte			word length
  1-3 byte[3]		article length
  4-5 byte[2]		смещение статьи в странице
  6-9 byte[4]		страница, в которой начинается статья
	? char[?]		null-terminated word
*/
struct page_t
{
	uint32 words;
	uint32 next_page;
};

void write_new_header( struct header_t * header, FILE * f )
{
	int code;
	code = fseek( f, 0, SEEK_SET );
	if( code == -1 )
		WTF();
		
	header->version = 1;
	header->words = 0;
	header->first_page = 0;
	code = fwrite( header, HEADER_SIZE, 1, f );
	if( code != 1 )
		WTF();
		
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

void save_database()
{
	fprintf( stderr, "TODO: save_database\n" );
}

void remove_newline( char * str )
{
	char * p = str;
	while( *p )
		p++;
	p[-1] = 0;
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
	
	struct header_t header;
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
		if( header.version != 1 )
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
			printf( "quit\tВыход и запись базы.\n" );
		}
		else if( strcmp( str, "quit" ) == 0 ||
				 strcmp( str, "exit" ) == 0 ||
				 strcmp( str, "bye" ) == 0 )
		{
			save_database();
			exit( EXIT_SUCCESS );
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