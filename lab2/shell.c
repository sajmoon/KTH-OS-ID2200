#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  printf("%s >> ", cwd);
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


char** getargs(char* input) {
  int i;
  char * tokenc;
  char *token;
  char** args;

  args = malloc(6*sizeof(char*));
  
  token = strtok(input, " ");

  for(i =0; i<6; i++)
  {
    if(token != NULL){
      tokenc = malloc(71*sizeof(char));
      strcpy(tokenc, token);
      args[i] = tokenc;
      token = strtok(NULL, " ");
    } else {
      args[i] = NULL;
    }
  }

  return args;
}

void freeargs(char** args) {
  int i;
  printf("free willy, dead or alive!\n");

  for(i=0; i<6; i++) {
    free(args[i]);
  }
}

int builtin(char* command, char** args)
{
  if(strcmp(command, "exit") == 0)
  {
    printf(">> Bye..");
    freeargs(args);
    return -1;
  }

  if (strcmp(command, "cd") == 0)
  {
    chdir(args[1]);
    return 1;
  }
  return 0;
}

int main(int argc, char **argv, char **envp)
{
  char **args;
  char input[71];
  int builtin_return;
  const size_t input_length = 71;

  while (1) {
    prompt(input, input_length);
    args = getargs(input);

    builtin_return = builtin(args[0], args);
    if (builtin_return == -1)
    {
      break;
    } else if (builtin_return == 0) {
      execute(args[0], args);
    }

    freeargs(args);
  }

  free(args);
  return 0;
}
