all: lab1

lab1: digenv.c
	gcc -Wall digenv.c -o digenv

clean: 
	rm digenv