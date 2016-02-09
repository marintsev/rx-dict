#include "utils.h"

void remove_newline(char * str) {
	char * p = str;
	while (*p)
		p++;
	p[-1] = 0;
}
