main: main.c
	make -C ./Debug/ all
#	gcc -o $@ $<

clean: default.db
	make -C ./Debug/ clean

mrproper:
	-@rm default.db

run: main
	./$<