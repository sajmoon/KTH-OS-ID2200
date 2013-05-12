#include <stdio.h>
#include "malloc.h"

void main(int argc, char* argv) {

  void * a = malloc(10);

  free(a);

}