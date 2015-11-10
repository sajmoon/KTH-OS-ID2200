#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define EXECUTE_STATUS int
#define SKIP_EXECUTE 0
#define NORMAL_EXECUTE 1


/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  do {
    printf("%s >> ", cwd);
    fgets(input, input_length, stdin);
  } while(strcmp(input, "\n") == 0);
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
  free(args);
}

EXECUTE_STATUS builtin(char* command, char** args)
{
  if(strcmp(command, "exit") == 0)
  {
    printf(">> Bye..");
    freeargs(args);
    exit(0);
  }

  if (strcmp(command, "cd") == 0)
  {
    chdir(args[1]);
    return SKIP_EXECUTE;
  }
  return NORMAL_EXECUTE;
}

int main(int argc, char **argv, char **envp)
{
  char **args;
  char input[71];
  const size_t input_length = 71;

  while (1) {
    prompt(input, input_length);
    args = getargs(input);

    if(builtin(args[0], args) == NORMAL_EXECUTE) {
      execute(args[0], args);
    }

    freeargs(args);
  }

  return 0;
}
