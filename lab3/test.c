#include <stdio.h>
#include "malloc.h"

union test{
	int a;
	float b;
};

void printAdd(void*a) {
  print("Adress",(long)a);
}

void simpleMallocTest() {
  void *a = malloc(1024);
  
  printAdd((void *)a);  
  free(a);
}

void simpleReallocTest() {
	void *a  = malloc(1024);
	void *b  = malloc(1024);
	realloc(a, 2048);
}

int main(int argc, char const *argv[]) {
  
  simpleMallocTest();
  /*simpleReallocTest();/**/

	return 0;
}
