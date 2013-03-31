
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

pid_t fork_errors() {
    pid_t pid = fork();

    if (pid < 0){
        printf("forking error");
        exit(1);
    }
    // Prints a log message with the pid so we can track if a process is not killed
    openlog("digenv", LOG_CONS, LOG_LOCAL1);
    syslog(LOG_INFO, "pid %d", pid);
    return pid;
}

/*
helper method to lock the read part of the pipe
*/
void prepare_send_pipe(int pipe[2]) {
    close(pipe[0]); 
    dup2(pipe[1],1);
    close(pipe[1]);
}

/* 
helper method to close the write part of the pipe
*/
void prepare_receive_pipe(int pipe[2]) {
    close(pipe[1]);    /* close write end of pipe              */
    dup2(pipe[0],0);   /* make 0 same as read-from end of pipe */
    close(pipe[0]);    /* close excess fildes                  */
}

/*
    returns the current pager as defined by env variables
    or less
*/
char *get_pager() {
    char * value;

    value = getenv ("PAGER");
    if (! value) {
        return "less";
    }
    else {
        return value;
    }
}

/* 
    Works as printenv | sort | less
    if we have defined pager in env variabels then use that pager.
    if we define parameters digenv works as:
    ./digenv [params] -> printenv | grep params | sort | less
 */
int main(int argc, char **argv, char **envp)
{
    int     pager_pipe[2];
    pipe(pager_pipe);
    
    pid_t sort_pid = fork_errors();

    if (sort_pid == 0 ) {

        int sort_pipe[2];
        pipe(sort_pipe);

        pid_t grep_pid = fork_errors();

        if (grep_pid == 0 ) {
            int grep_pipe[2];
            pipe(grep_pipe);

            pid_t env_pid = fork_errors();

            if (env_pid == 0) {
                // child pager
                prepare_send_pipe(grep_pipe);
                execlp("printenv","printenv",NULL);
                perror("errror");       /* still around?  exec failed           */
                _exit(1);
            }

            prepare_receive_pipe(grep_pipe);
            prepare_send_pipe(sort_pipe);
            
            if (argc > 1) {
                // runs the binary "grep" with the arguments in argv
                execvp("grep", argv);

                // if it fails this message is printed.
                perror("Grep failed");
                _exit(0);
            }
        }
        
        prepare_receive_pipe(sort_pipe);
        prepare_send_pipe(pager_pipe);
        // runs the sort binary with to parameters but with piped input
        execlp("/usr/bin/sort","sort",NULL);

        // failure message
        perror("Sorting failed");
        _exit(1);

    } else {
        prepare_receive_pipe(pager_pipe);
        // runs the specified pager with input from pipe
        execlp(get_pager(),get_pager(),NULL);

        // failure message
        perror("Invalid pager"); 
        _exit(1);
    }

    //printf("exiting %d", pid);
    return EXIT_SUCCESS;
}