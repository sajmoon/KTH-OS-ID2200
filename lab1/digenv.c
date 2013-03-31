
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/* utan parametrar ska digenv fungera som printev | sort | less 
pager ska vara less om inte envvar PAGER är satt till något annat

 */
int main(int argc, char **argv, char **envp)
{

    int     fd[2];
    int     envToSort[2];
    
    char    readbuffer[2256];
    pipe(fd);

    pid_t   pID = fork();

    if (pID == 0 ) {
        // child
        int pID2 = fork();

        if (pID2 == 0) {
            close(fd[0]);
            dup2(fd[1],1);
            close(fd[1]);
            execlp("printenv","printenv",NULL);
            perror("demo");       /* still around?  exec failed           */
            _exit(1);             /* no flush  */  
        } else {
            close(fd[1]);    /* close write end of pipe              */
            dup2(fd[0],0);   /* make 0 same as read-from end of pipe */
            close(fd[0]);    /* close excess fildes                  */



        }

        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);
        execlp("printenv","printenv",NULL);
        perror("demo");       /* still around?  exec failed           */
        _exit(1);             /* no flush    */

        int     i;
        char    status[1];

    } else if (pID < 0) {
        //error
        printf("some error in forking");
        return 1;
    } else {
        // parent
        close(fd[1]);    /* close write end of pipe              */
        dup2(fd[0],0);   /* make 0 same as read-from end of pipe */
        close(fd[0]);    /* close excess fildes                  */
        execlp("/usr/bin/less","less",NULL);

    }

    printf("exiting %d", pID);
    return EXIT_SUCCESS;
}