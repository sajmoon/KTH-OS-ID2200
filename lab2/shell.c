#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  printf("$ ");
  fgets(input, input_length, stdin);

  strtok(input, "\n"); /* does not strip on string with only \n in it. */
}

void child_execute(int pid, char* command, char **argv)
{
    execvp(command, argv);
    printf("Error: execwp failed\n");
}

void execute(char* command, char **argv)
{
  int pid;
  int wait_return;

  pid = fork();

  if (pid == 0)
  {
    child_execute (pid, command, argv);
  } else {
    wait(&wait_return);
    printf("Child finished executing\n");
  }
}

int main(int argc, char **argv, char **envp)
{
  char input[70];
  const size_t input_length = 70;

  prompt(input, input_length);

  execute(input, argv);

  return 0;
}
