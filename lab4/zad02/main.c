#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

const int TESTING_SIGNAL = SIGUSR1;

void wait_for_children()
{
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;
}

int main(int argc, char **argv)
{
    return 0;
}