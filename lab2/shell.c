#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <ctype.h>

/* Pipe constants */
#define READ        0
#define WRITE       1 
#define PIPE_COUNT  3

typedef void (*sighandler_t)(int);

int pipes[PIPE_COUNT][2];

void prompt() {
  fputs("\n>> ", stdout);
  fflush(stdout);
}

bool safe_fork() {
    pid_t pid = fork();

    if (pid < 0){
        printf("forking error");
        exit(1);
    }
    /* Prints a log message with the pid so we can track if a process is not killed */
    openlog("Simons och mattis shell", LOG_CONS, LOG_LOCAL1);
    syslog(LOG_INFO, "pid %d", pid);
    return pid == 0;
}

/* Block Ctrl-C */
void handle_signal(int signo)
{
  prompt();
}

void print_usage() {
  printf("usage:\n");
}

char** tokenize(const char* input) {
    char* str = strdup(input);
    int count = 0;
    int capacity = 10;
    char** result = malloc(capacity*sizeof(char));

    char* tok=strtok(str," "); 

    while(1)
    {
        if (count >= capacity)
            result = realloc(result, (capacity*=2)*sizeof(*result));

        result[count++] = tok? strdup(tok) : tok;

        if (!tok) break;

        tok=strtok(NULL," ");
    } 

    free(str);
    return result;
}

bool is_background(char ** args) {
  int i;
  
  for (i = 0; i < 9; i++) {
    if (args[i] == NULL)
      return false;
    if (strcmp(args[i], "&")==0) {
      args[i] = NULL;
      return true;
    }
  }
  return false;
}

void runCommand(char *input) {
  pid_t pid;
  char ** args;
  bool bg;

  pid   = fork();
  args  = tokenize(input);
  bg    = is_background(args);

  if (pid < 0) {
    /* error */
  } else if (pid == 0) {
    /* Child */
    if (execvp(args[0], args) < 0) {
      printf("ERROR in execlp \n");
      exit(1);
    }
    
    if (bg) {
      /* should the child do something special when its in bg mode? */
      free(args);
    }

  } else {
    /* parent */
    if (!bg) {
      int status;
      while (wait(&status) != pid);       /* wait for completion  HITTAD DEN RAD ONLINE */
      free(args);
    }
  }
}

int main(int argc, char **argv, char **envp)
{
  size_t size = 70;
  char* input = NULL;

  signal(SIGINT, SIG_IGN);
  signal(SIGINT, handle_signal);
  
  while(1) {
    prompt();

    getline(&input, &size, stdin);
    
    if (strcmp(input, "\n")==0) {
      printf("Try entering a command. Exit is a good one. Give it a try!");
      continue;
    }
    printf("\n");
    
    input[strlen(input) - 1] = 0;

    if (strcmp(input, "exit") == 0) {
      printf("Good bye! ttyl\n");
      return EXIT_SUCCESS;

    } else if (strcmp(input, "cd") == 0) {
      char ** args = NULL;
      args = tokenize(input);
      chdir(args[1]);
      free(args);
    } else {
      runCommand(input);
      print_usage();
 
    }
  }
  return EXIT_SUCCESS;
}

