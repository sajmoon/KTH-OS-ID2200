all: lab2-ansi

lab2: shell.c
	gcc -Wall -O4 shell.c -o shell.o

lab2-ansi: shell.c
	gcc -Wall -ansi -pedantic -O3 shell.c -o shell.o

automake:
	@ls *.c | entr -c sh -c "rm -f shell.o && make --no-print-directory && echo done"
