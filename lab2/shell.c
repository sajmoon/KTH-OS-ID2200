#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

void prompt() {
  fputs("\n>> ", stdout);
  fflush(stdout);
}

int main(int argc, char **argv, char **envp)
{
  char* input = NULL;
  prompt();

  size_t size = 70;
  getline(&input, &size, stdin);
  
  input[strlen(input) - 1] = 0;

  if (strcmp(input, "exit") == 0) {
    return EXIT_SUCCESS;
  } else {
    prompt();
  }
  return EXIT_SUCCESS;
}

