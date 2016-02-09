#include "main.h"

#include "entry.h"
#include "db.h"
#include "commands.h"
#include "utils.h"

#define DEFAULT_DB_NAME		"default.db"

void check() {
	if (4 != sizeof( uint32 )) {
		fprintf( stderr, "Размер uint32 не 32 бита, а %lu. Надо поправить.\n",
				sizeof( uint32 ));
		exit( EXIT_WRONG_SIZEOF);
	}

	if (8 != sizeof( uint64 )) {
		fprintf( stderr, "Размер uint64 не 64 бита, а %lu. Надо поправить.\n",
				sizeof( uint64 ));
		exit( EXIT_WRONG_SIZEOF);
	}
}

// Формат базы (little endian).

void write_new_header(struct header_t * header, FILE * f) {
	header->version = 1;
	header->total_words = 0;
	header->actual_words = 0;

	int code;
	write_header(header, f);
	fflush(f);

	code = ftruncate(fileno(f), HEADER_SIZE);
	if (0 != code) {
		fprintf( stderr, "errno = %d\n", errno);
		WTF();
	}
}

char * db_filename = NULL;

void read_header(struct header_t * header, FILE * f) {
	int code;
	code = fseek(f, 0, SEEK_SET);
	if (code == -1)
		WTF();

	code = fread(header, HEADER_SIZE, 1, f);
	if (1 != code)
		WTF();
}

struct header_t header;

void save_database(FILE * f) {
	write_header(&header, f);
//	TODO( "save_database\n" );
}

int main(int argc, char ** argv) {
	check();

	// Передан файл базы как параметр
	if (argc == 2) {
		db_filename = argv[1];
	}
	// Файл не передан, используется имя по умолчанию
	else if (argc == 1) {
		db_filename = "default.db";
	}
	// Неправильное использование
	else {
		fprintf( stderr, "Usage: dictionary [words.db]\n");
		exit( EXIT_WRONG_USAGE);
	}

	assert(NULL != db_filename);

	FILE * f = NULL;
	int code = access(db_filename, R_OK | W_OK);
	// Нет прав, не существует файл или что-нибудь ещё
	if (-1 == code) {
		// Нет файла, можно создавать
		if ( errno == ENOENT) {
			f = fopen(db_filename, "w+");
			fprintf( stderr, "INFO: Файл не существовал и теперь создан.\n");
		} else if ( errno == EACCES) {
			fprintf( stderr, "FATAL: Нет прав доступа к базе.\n");
			exit( EXIT_WRONG_RIGHTS);
		} else {
			printf("errno = %d\n", errno);
			exit( EXIT_WTF);
		}
	}
	// Есть права и файл существует
	else {
		f = fopen(db_filename, "r+");
		fprintf( stderr, "INFO: Существующий файл открыт для работы.\n");
	}
	assert(NULL != f);

	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	fprintf( stderr, "DEBUG: Длина файла: %d\n", length);

	if (length < HEADER_SIZE) {
		// Длина файла заведомо меньше размера header. Пишем новый.
		write_new_header(&header, f);
	} else {
		// Читаем
		read_header(&header, f);

		// Проверяем
		if (header.version != 1 || header.actual_words > header.total_words) {
			fprintf( stderr,
					"FATAL: База данных (%s), вероятно, испорчена. Возможно, стоит её удалить.\n",
					db_filename);
			WTF();
		}
	}

	// Всё есть. Можно работать.
	printf("Введите help для справки, quit для выхода.\n");
	int working = 1;
	while (working) {
		printf("> ");

		char str[6];
		fgets(str, 6, stdin);
		remove_newline(str);
		if (strcmp(str, "help") == 0) {
			command_help();
		} else if (strcmp(str, "quit") == 0 || strcmp(str, "exit") == 0
				|| strcmp(str, "bye") == 0) {
			save_database(f);
			exit( EXIT_SUCCESS);
		} else if (strcmp(str, "add") == 0) {
			command_add(f);
		} else if (strcmp(str, "find") == 0) {
			command_find(f);
		} else if (strcmp(str, "del") == 0 || strcmp(str, "rm") == 0) {
			command_remove(f);
		} else if (strcmp(str, "frag") == 0) {
			command_defragment(f);
		} else {
			fprintf( stderr, "ERROR: Неизвестная команда '%s'\n", str);
		}
	}

	/*
	 Операции:
	 1. Добавить слово. Имя (128 байт), содержание (не ограничено) ->
	 2. Найти слово. Имя -> содержание
	 3. Удалить. Имя ->
	 */
	if (f != NULL)
		fclose(f);

	return EXIT_SUCCESS;
}
