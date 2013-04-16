all: lab1

lab1: digenv.c
	gcc -Wall digenv.c -o digenv

pdf: rapport.tex
	xelatex rapport.tex

clean: 
	rm digenv
	rm rapport.aux
	rm rapport.pdf
	rm rapport.log
	rm rapport.out
