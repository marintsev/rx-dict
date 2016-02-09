#include "commands.h"

#include "main.h"
#include "entry.h"
#include "utils.h"
#include "db.h"

void read_word(char * word, int size) {
	printf("Введите слово> ");
	fgets(word, size, stdin);
	remove_newline(word);
	// TODO: не позволять пустое слово
	assert(strlen(word) != 0);
}

extern char * read_content();

char * read_content() {
	printf("<Enter> -- новая строка, <Enter><Enter> -- завершить ввод.\n");
	printf("Введите толкование> ");
	char * content = NULL;
	// TODO: ввод множества строк
	char line[4096];
	while (1) {
		fgets(line, 4096, stdin);
		//remove_newline(line);
		// добавить строку
		if (strlen(line) != 1)
		{
			if (content == NULL) {
				content = malloc(strlen(line) + 1);
				strcpy(content, line);
			} else {
				char * new_content = malloc(strlen(content) + strlen(line) + 1);
				strcpy(new_content, content);
				strcat(new_content, line);
				free(content);
				content = new_content;
			}
		} else {
			return content;
		}
	}
}

void command_add(FILE * f) {
	char word[130];
	read_word(word, 130);

	uint64 index = find_word(f, word);
	if (0 == index) {
		// Слова нет, можно добавлять
		char * content = read_content();
		add_word(f, word, content);
		free( content );
	} else {
		// найденное слово запомним
		struct entry_t entry;
		read_entry(f, &entry, index, READ_ENTRY_WORD | READ_ENTRY_DO_SEEK);
		remove_words(f, entry.word);
		char * content = read_content();
		add_word(f, entry.word, content);
		free( content );
		free_entry(&entry);

		//TODO( "Прочитать толкование, запомнить слово, вставить новую, удалить старую." );
	}
}

void command_find(FILE * f) {
	char word[130];
	read_word(word, 130);

	uint64 index = find_word(f, word);
	if (0 == index) {
		// слова нет, сообщить
		fprintf( stderr, "[INFO] нет такого слова\n");
	} else {
		// слово есть, выводим
		//fseek( f, index, SEEK_SET );
		struct entry_t entry;
		read_entry(f, &entry, index, READ_ENTRY_CONTENT | READ_ENTRY_DO_SEEK);
		printf("Значение: %s\n", entry.content);
	}
}

int remove_words(FILE * f, char * word) {
	int count = 0;
	// асимптотика квадратичная :)
	// TODO: сделать параметр from в find_word, чтобы сделать её линейной.
	while (1) {
		uint64 start = find_word(f, word);
		if (start == 0) {
			// Если нет слова, и мы его не удаляли

			return count;
		}
		struct entry_t entry;
		// TODO: дважды read_entry -- плохо
		read_entry(f, &entry, start, READ_ENTRY_ALL);
		mark_deleted_entry(&entry);
		count++;

		int code = fseek(f, start, SEEK_SET);
		if (0 != code)
			WTF();

		write_entry(f, &entry);
		header.actual_words--;
		write_header(&header, f);
	}
}

void command_remove(FILE * f) {
	char word[130];
	read_word(word, 130);

	int count = remove_words(f, word);
	if (count == 0)
		printf("[INFO] Нет такого слова.\n");
	else if (count == 1)
		printf("[INFO] Слово удалено.\n");
	else
		printf("[WARNING] Удалено %d вхождений.\n", count);

}

void command_defragment(FILE * f) {
	uint64 read_index, write_index, index;
	read_index = write_index = jump_to_first_word(f);
	int read_count = 0;
	int write_count = 0;
	struct entry_t entry;
	again: if (write_count < header.actual_words) {
		index = read_entry(f, &entry, read_index, READ_ENTRY_ALL);
		if (existent_entry(&entry)) {
			read_index = index;
			read_count++;
			int code = fseek(f, write_index, SEEK_SET);
			if (0 != code)
				WTF();
			write_entry(f, &entry);
			write_count++;
			write_index += entry_size(&entry);
			goto again;
		} else {
			read_index = index;
			goto again;
		}
	} else {
		header.total_words = header.actual_words;
		write_header(&header, f);
		ftruncate(fileno(f), write_index);
	}
}

void command_help() {
	printf("add\tДобавить слово.\n");
	printf("find\tНайти слово.\n");
	printf("del\tУдалить слово.\n");
	printf("frag\tДефрагментация.\n");
	printf("quit\tВыход и запись базы.\n");
}
