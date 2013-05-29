/*
 Operating systems - ID2200 KTH
 Lab 1: Pipelines

 ========== digenv.c ==========
 Produces a result similar to:
 
 $ printenv | grep [args] | sort | $PAGER

 with a default value of $PAGER=/usr/bin/less

 Authors:
 Simon Ström <simstr@kth.se>
 Mattis Kancans Envall <mattiske@kth.se>

 For a more detailed task description, please see:
  http://www.imit.kth.se/courses/2G1520/COURSELIB/labs/pipelab.pdf 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

/* Pipe constants */
#define READ        0
#define WRITE       1 
/* Pipe names */
#define DIGENV      0
#define GREP        1
#define SORT        2
#define PIPE_COUNT  3

/* global pipes? */
int pipes[PIPE_COUNT][2];

bool safe_fork() {
    pid_t pid = fork();

    if (pid < 0){
        printf("forking error");
        exit(EXIT_FAILURE);
    }
    /* Prints a log message with the pid,
        so we can track if a process is not killed */
    openlog("digenv", LOG_CONS, LOG_LOCAL1);
    syslog(LOG_INFO, "pid %d", pid);
    return pid == 0;
}

/*
Helper method to lock the read part of the pipe
*/
void prepare_send_pipe(int pipe) {
    close(pipes[pipe][READ]);
    dup2 (pipes[pipe][WRITE], 1);
    close(pipes[pipe][WRITE]);
}

/* 
Helper method to close the write part of the pipe
*/
void receive_from_pipe(int pipe) {
    close(pipes[pipe][WRITE]);    /* close write end of pipe              */
    dup2 (pipes[pipe][READ], 0);  /* make 0 same as read-from end of pipe */
    close(pipes[pipe][READ]);     /* close excess fildes                  */
}

/*  returns the current pager as defined by env variables
    or "less"  */
char *get_pager() {
    char * value;

    value = getenv ("PAGER");
    return (value) ? value : "less";
}

static int a = 0;

void wait_for_child_to_terminate() {
    return;
    int status;
    fprintf(stderr, "Waiting for child %d\n", a++);
    wait(&status);

    if ( !WIFEXITED(status) ) {
        fprintf(stderr, "Child failed to terminate\n");
        _exit(EXIT_FAILURE);
    }
}

/* 
    With no arguments specified: works as printenv | sort | less
    if we have defined pager in env variabels, that pager is used instead of less
    if we define parameters digenv works as:
    ./digenv [params] -> printenv | grep params | sort | less
 */
int main(int argc, char **argv, char **envp)
{
    /* true for childprocesses after each fork */
    bool isChild;

    pipe(pipes[DIGENV]);

    isChild = safe_fork();
    if ( isChild ) {
        /* child pager */
        prepare_send_pipe(DIGENV);
        execlp("printenv","printenv",NULL);
        _exit(EXIT_FAILURE);
    }

    receive_from_pipe(DIGENV);

    wait_for_child_to_terminate();

    pipe(pipes[GREP]);

    isChild = safe_fork();
    if ( isChild ) {
        prepare_send_pipe(GREP);
        if (argc > 1) {
        /* runs the binary "grep" with the arguments in argv */
            execvp("grep", argv);
        }else {
            /* cat simply rewrites input to output, just like 
            a grep without arguments ;) */
            execvp("cat", argv);
        }
        _exit(EXIT_FAILURE);
    }

    receive_from_pipe(GREP);

    wait_for_child_to_terminate();

    pipe(pipes[SORT]);

    isChild = safe_fork();
    if ( isChild ) {
        prepare_send_pipe(SORT);
    /* runs the sort binary with to parameters but with piped input */
        execlp("/usr/bin/sort","sort",NULL);

        /* If we get here, call to sort has failed. */
        perror("Sorting failed");
        _exit(EXIT_FAILURE);
    }

    receive_from_pipe(SORT);

    wait_for_child_to_terminate();

    /* runs the specified pager with input from pipe */
    execlp(get_pager(), get_pager(), NULL);

    /* if we get here, pager has failed. */
    perror("Invalid pager"); 
    _exit(EXIT_FAILURE);
    return EXIT_FAILURE;
}
