#include <stdio.h>

/* prompt -> prints to term. waits for input. */
void prompt(char* input, size_t input_length)
{
  printf("$ ");
  getline(&input, &input_length, stdin);
}

int main(int argc, char **argv, char **envp)
{
  char input[70];
  size_t input_length = 70;

  prompt(input, input_length);

  printf("%s", input);
  return 0;
}
