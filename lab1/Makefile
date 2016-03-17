all: lab1

lab1: digenv.c
	gcc -Wall -ansi -DDIGENV_MAIN digenv.c -o digenv.o

pdf: rapport.tex
	xelatex rapport.tex

clean: 
	rm digenv.o
	rm rapport.aux
	rm rapport.pdf
	rm rapport.log
	rm rapport.out
