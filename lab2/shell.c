#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define EXECUTE_STATUS bool
#define SKIP_EXECUTE false
#define NORMAL_EXECUTE true

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRNn  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"


void print_prompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    printf("%s%s %s>> %s", KRED, cwd, KYEL, KNRM);
    fflush(stdout);
}


void print_usage(const pid_t pid, const char* command, const bool is_background, struct timeval start_time) {
  struct timeval end_time;
  gettimeofday(&end_time, 0);

  if (is_background)
    printf("\n");

  printf("Pid: %d finished command '%s' in", pid, command);

  printf(" %ld.%06lds\n", end_time.tv_sec - start_time.tv_sec, end_time.tv_usec - start_time.tv_usec);

  if (is_background)
    print_prompt();
  fflush(stdout);
}

/* prompt -> prints to term. waits for input. */
void prompt(char* input, const int input_length)
{
  int status;
  
  do {
    print_prompt();
    fgets(input, input_length, stdin);

    if(waitpid(-1, &status, WNOHANG) == -1) {
      printf("ERROR: waitpid failed\n");
    }

    if (WIFEXITED(status)) {
      printf("exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("killed by signal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
      printf("stopped by signal %d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
      printf("continued\n");
    }
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

  for(i =0; i<6; i++)
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
  _exit(EXIT_FAILURE);
}

void execute_then_free(char* command, char **argv, const bool is_background)
{
  /* Executed only as child */
  int pid, status;
  struct timeval start_time;
  gettimeofday(&start_time, 0);
  
  pid = fork();
  
  if (pid == 0){
    execute_and_exit(command, argv);
  } else {

    if(waitpid(pid, &status, WUNTRACED | WCONTINUED) == -1) {
      printf("ERROR: NO CHILD TO WAIT FOR\n");
    }

    if (WIFSIGNALED(status)) {
      printf("ERROR: killed by signal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
      printf("ERROR: stopped by signal %d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
      printf("WTF: continued\n");
    }
    
    print_usage(pid, command, is_background, start_time);

    freeargs(argv);
    exit(EXIT_SUCCESS);
  }
}

void execute(char* command, char **argv, const bool is_background)
{
  pid_t pid;
  int status;
  
  pid = fork();

  if (pid == 0)
  {
    execute_then_free(command, argv, is_background);
  } else {
    if(!is_background && waitpid(pid, &status, WUNTRACED) == -1) {
      printf("ERROR: expected to wait for child process\n");
    }
    freeargs(argv);
  }
}

EXECUTE_STATUS builtin(char* command, char** args)
{
  if(strcmp(command, "exit") == 0)
  {
    printf("%s>> %sBye..", KYEL, KNRM);
    freeargs(args);
    exit(EXIT_SUCCESS);
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
  int status;
  char **args;
  char input[71];
  bool is_background_process;
  const size_t input_length = 71;

  printf("Shell pid: %d\n", getpid());
  while (1) {
    prompt(input, input_length);
    is_background_process = ends_with_ampersand(input);
    args = getargs(input);

    if(builtin(args[0], args) == NORMAL_EXECUTE) {
      execute(args[0], args, is_background_process);
    }

  }

  return 0;
}
