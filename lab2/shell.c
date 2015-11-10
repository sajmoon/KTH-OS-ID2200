#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  printf("$ ");
  fgets(input, input_length, stdin);

  strtok(input, "\n"); /* does not strip on string with only \n in it. */
}

int main(int argc, char **argv, char **envp)
{
  char input[70];
  const size_t input_length = 70;

  prompt(input, input_length);

  execvp(input, argv);
  printf("Error: execwp failed\n");
  return 0;
}
