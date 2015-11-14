.PHONY: all valgrind
all: shell.o

shell.o: shell.c
	gcc -Wall -ansi -pedantic -O3 shell.c -o shell.o

valgrind: shell.o
	valgrind --leak-check=full ./shell.o

automake:
	@ls *.c | entr -c sh -c "rm -f shell.o && make --no-print-directory && echo done"
