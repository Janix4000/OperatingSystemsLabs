#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

char *MODE;
const int TESTING_SIGNAL = SIGUSR1;

void test_signal(void (*ignore_handler)(), void (*handle_handler)(), void (*mask_handler)(), void (*pending_handler)())
{
    if (strcmp(MODE, "ignore") == 0)
    {
        ignore_handler();
    }
    else if (strcmp(MODE, "handle") == 0)
    {
        handle_handler();
    }
    else if (strcmp(MODE, "mask") == 0)
    {
        mask_handler();
    }
    else if (strcmp(MODE, "pending") == 0)
    {
        pending_handler();
    }
    else
    {
        fprintf(stderr, "Not known MODE \"%s\".", MODE);
        exit(-1);
    }
}

void test_pending()
{
    printf("\n==== Testing Pending Signals (fork) ====\n");
    sigset_t newmask, pendingMask;
    sigemptyset(&newmask);
    sigaddset(&newmask, TESTING_SIGNAL);
    sigprocmask(SIG_SETMASK, &newmask, NULL);

    raise(TESTING_SIGNAL);

    pid_t childPID;
    bool isPending;
    if ((childPID = fork()) < 0)
    {
        fprintf(stderr, "Error occured while creating child process\n");
        exit(1);
    }
    else if (childPID == 0)
    {
        sigpending(&pendingMask);
        isPending = sigismember(&pendingMask, TESTING_SIGNAL);
        printf("== Parent ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
        while (wait(NULL) > 0)
            ;
    }
    else
    {
        sigpending(&pendingMask);
        isPending = sigismember(&pendingMask, TESTING_SIGNAL);
        printf("== Child ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
    }
    exit(0);
}

void test_itself()
{
    test_signal(NULL, NULL, NULL, test_pending);
}

void wait_for_children()
{
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected only one argument.\n");
        exit(-1);
    }
    MODE = argv[1];

    test_itself();

    wait_for_children();
    return 0;
}