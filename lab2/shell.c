#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define EXECUTE_STATUS bool
#define SKIP_EXECUTE false
#define NORMAL_EXECUTE true

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"


void print_prompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s%s %s>> %s", KRED, cwd, KYEL, KNRM);
    fflush(stdout);
}

void print_usage(const char* command, const bool is_background) {
    if (is_background)
      printf("\n");

    printf("Command '%s' finished in <USAGE STATISTICS HÄR SIMON FÖR HELVETE>\n", command);

    if (is_background)
      print_prompt();
}

/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  do {
    print_prompt();
    fgets(input, input_length, stdin);
  } while(strcmp(input, "\n") == 0);


  strtok(input, "\n"); /* does not strip on string with only \n in it. */
}

bool ends_with_ampersand(const char* input) {
  return strcmp(&input[strlen(input)-1], "&") == 0;
}

char** getargs(char* input) {
  int i;
  char * tokenc;
  char *token;
  char** args;

  args = malloc(6*sizeof(char*));
  
  token = strtok(input, " ");

  for(i =0; i<7; i++)
  {
    if(token != NULL && strcmp(token, "&") != 0){
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

  for(i=0; i<6; i++) {
    free(args[i]);
  }
  free(args);
}

void execute_and_exit(char* command, char** argv)
{
  execvp(command, argv);
  printf("Unknown or invalid command: %s\n", command);
  _exit(1);
}

void execute_then_free(char* command, char **argv, const bool is_background)
{
  int pid, wait_return;
  pid = fork();
  if (pid == 0){
    execute_and_exit(command, argv);
  } else {
    waitpid(pid, &wait_return, WUNTRACED);

    print_usage(command, is_background);

    freeargs(argv);
    _exit(0);
  }
}

void execute(char* command, char **argv, const bool is_background)
{
  pid_t pid;
  int wait_return;
  
  pid = fork();

  if (pid == 0)
  {
    execute_then_free(command, argv, is_background);
  } else {
    if(!is_background) {
      waitpid(pid, &wait_return, WUNTRACED);
    }
  }
  printf("all done\n");
}

EXECUTE_STATUS builtin(char* command, char** args)
{
  if(strcmp(command, "exit") == 0)
  {
    printf("%s>> %sBye..", KYEL, KNRM);
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
  bool is_background_process;
  const size_t input_length = 71;

  while (1) {
    prompt(input, input_length);
    is_background_process= ends_with_ampersand(input);
    args = getargs(input);

    if(builtin(args[0], args) == NORMAL_EXECUTE) {
      execute(args[0], args, is_background_process);
    }
  }

  return 0;
}
