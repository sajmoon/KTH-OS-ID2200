#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
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
    // Prints a log message with the pid so we can track if a process is not killed
    openlog("Simons och mattis shell", LOG_CONS, LOG_LOCAL1);
    syslog(LOG_INFO, "pid %d", pid);
    return pid == 0;
}

/* Block Ctrl-C */
void handle_signal(int signo)
{
	printf("\n Ctrl-C");
	fflush(stdout);
}

void print_usage() {
  printf("usage:");
}

void runCommand(char *input) {
    pid_t pid = fork();

    if (pid < 0) {
      // error
      //
    } else if (pid == 0) {
      //Child
      
      if (execlp(input, input, NULL) < 0) {
        printf("ERROR in execlp \n");
        exit(1);
      }

    } else {
      // parent
      int status;
      while (wait(&status) != pid);       /* wait for completion  HITTAD DEN RAD ONLINE */
    }
}

int main(int argc, char **argv, char **envp)
{

  // signal(SIGINT, SIG_IGN);
  // signal(SIGINT, handle_signal);
  
  char* input = NULL;

  while(1) {
    prompt();

    size_t size = 70;
    getline(&input, &size, stdin);
    printf("\n");
    print_usage();
    
    input[strlen(input) - 1] = 0;

    if (strcmp(input, "exit") == 0) {
      return EXIT_SUCCESS;
    } else {
      int pipe = 0;
      
      runCommand(input);

 
    }
  }
  return EXIT_SUCCESS;
}
