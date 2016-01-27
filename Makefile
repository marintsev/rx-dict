main: main.c
	gcc -o $@ $<

clean: default.db
	-@rm default.db

run: main
	./$<