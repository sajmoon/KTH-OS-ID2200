
#include <stdio.h>
#include <stdlib.h>


/* utan parametrar ska digenv fungera som printev | sort | less 
pager ska vara less om inte envvar PAGER är satt till något annat

 */
int main(int argc, char **argv, char **envp)
{

    int i;
    for(i=0; envp[i]!= NULL; i++)
        printf("%2d:%s\n",i, envp[i]);

    return EXIT_SUCCESS;
}