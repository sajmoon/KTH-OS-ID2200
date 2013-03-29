
#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void ourSignalHandler (int signo) {
    printf("received signal: %d\n", signo);
}

int main()
{
    if(signal(SIGINT, ourSignalHandler) == SIG_ERR)
        printf("error :/\n");

    while (1) {
        sleep(10);
    }
    return 0;
}



