
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
    openlog("test", LOG_CONS, LOG_LOCAL1);

    syslog(LOG_INFO, "pid %d", pid);
    return pid;
}

void prepare_send_pipe(int pipe[2]) {
    close(pipe[0]); 
    dup2(pipe[1],1);
    close(pipe[1]);
}

void prepare_receive_pipe(int pipe[2]) {
    close(pipe[1]);    /* close write end of pipe              */
    dup2(pipe[0],0);   /* make 0 same as read-from end of pipe */
    close(pipe[0]);    /* close excess fildes                  */
}

/*

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

/* utan parametrar ska digenv fungera som printev | sort | less 
pager ska vara less om inte envvar PAGER är satt till något annat

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

            // parent  
            prepare_receive_pipe(grep_pipe);
            prepare_send_pipe(sort_pipe);
            //printf("%s", join_arguments(argc, argv));
            //exit(1);
            //execlp("/bin/grep","grep", join_arguments(argc, argv), NULL);
            if (argc > 1) {
                execvp("grep", argv);
                perror("Grep failed");
                _exit(0);
            }
        }
        
        prepare_receive_pipe(sort_pipe);
        prepare_send_pipe(pager_pipe);
        execlp("/usr/bin/sort","sort",NULL);
        perror("Sorting failed");
        _exit(1);

    } else {
        prepare_receive_pipe(pager_pipe);
        execlp(get_pager(),get_pager(),NULL);
        perror("Invalid pager"); 
        _exit(1);
    }

    //printf("exiting %d", pid);
    return EXIT_SUCCESS;
}