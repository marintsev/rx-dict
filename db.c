#include "db.h"

#include "entry.h"

void write_header(struct header_t * header, FILE * f) {
	int code = fseek(f, 0, SEEK_SET);
	if (code == -1)
		WTF();

	code = fwrite(header, HEADER_SIZE, 1, f);
	if (code != 1)
		WTF();
}

uint64 jump_to_first_word(FILE * f) {
	long offset = HEADER_SIZE;
	int code = fseek(f, offset, SEEK_SET);
	if (0 != code)
		WTF();
	return offset;
}

/*
 Находит смещение, где находится запись для этого слова.
 TODO: функции работающие с файлами 32-битные.
 */
uint64 find_word(FILE * f, char * pattern) {
	int i;
	uint64 offset = jump_to_first_word(f);
	int start;

	struct entry_t entry;
	for (i = 0; i < header.total_words; i++) {
		start = offset;
		// TODO: совсем не обязательно выделять память здесь
		offset = read_entry(f, &entry, start,
		READ_ENTRY_WORD | READ_ENTRY_DO_SEEK);
		int good = 0;
		if (existent_entry(&entry)) {
			good = strcmp(entry.word, pattern) == 0;
			free_entry(&entry);
		}

		if (good)
			return start;
	}
	return 0;
}

uint64 skip_to_end(FILE * f) {
	int i;
	int offset = jump_to_first_word(f);
	int start;

	struct entry_t entry;
	for (i = 0; i < header.total_words; i++) {
		start = offset;
		// TODO: совсем не обязательно выделять память здесь
		offset = read_entry(f, &entry, start, 0);
		free_entry(&entry);
	}
	return offset;
}

void add_word(FILE * f, char * word, char * content) {
	skip_to_end(f);
	struct entry_t entry;
	entry.word_len = strlen(word);
	entry.content_len = strlen(content);
	entry.word = word;
	entry.content = content;
	write_entry(f, &entry);
	header.total_words++;
	header.actual_words++;
	write_header(&header, f);
}
