#include <stdio.h>
#include "malloc.h"

union test{
	int a;
	float b;
};


int main(int argc, char const *argv[]) {

	void *a  = malloc(1024);
	void *b  = malloc(1024);
	realloc(a, 2048);

	return 0;
}
