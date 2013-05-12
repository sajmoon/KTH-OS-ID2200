#include <stdio.h>
#include "malloc.h"

union test{
	int a;
	float b;
};

void printAdd(void*a) {
  print("Adress",(long)a);
}

void simpleMallocTest1() {
  print("simpleMallocTest1", 0);
  void *a = malloc(1024);
  show("simpleMallocTest1:");
  free(a);
  print("simpleMallocTest1 passed.");
}

void simpleReallocTest() {
  print("simpleReallocTest1", 0);
	void *a  = malloc(1024);
  //printAdd(a);
	
  void *b  = malloc(1024);
  //printAdd(b);
	
  realloc(a, 2048);
  //printAdd(a);

  show("test after malloc and realloc");
  
  free(a);
  
  free(b);
  show("test after free");
}

simpleMallocTest2() {
  print("simpleMallocTest2", 0);
  void *a1 = malloc(10);
  void *a2 = malloc(10);
  void *a3 = malloc(10);
  void *a4 = malloc(10);
  void *a5 = malloc(10);
  void *a6 = malloc(10);
  void *a7 = malloc(10);
  void *a8 = malloc(10);
  void *a9 = malloc(10);
  
  free(a2);
  free(a4);
  free(a6);
  free(a8);

  show("sparsly malloc");

  free(a3);

  show("see if it has merged 1,2 and 3 into a bigger block");

  free(a1);

  
  free(a5);
  free(a7);

  show("removed all but one datablock");
  free(a9);
  show("remove all");
}


int main(int argc, char const *argv[]) {
  
  void *test = malloc(12*sizeof(int));

  free(test);


  simpleMallocTest1();
  simpleMallocTest2();
  simpleReallocTest();


	return 0;
}
