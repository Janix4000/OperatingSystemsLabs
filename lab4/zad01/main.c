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

void wait_for_children()
{
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;
}

void test_signal_fork(void (*ignore_handler)(), void (*handle_handler)(), void (*mask_handler)(), void (*pending_handler)())
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

void test_ignore_fork()
{
    printf("\n==== Testing Ignore Signals (fork) ====\n");
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(TESTING_SIGNAL, &act, NULL);
    pid_t childPID;
    if ((childPID = fork()) < 0)
    {
        fprintf(stderr, "Error occured while creating child process\n");
        exit(1);
    }
    else if (childPID == 0)
    {
        printf("== Parent run ==\n");
        raise(TESTING_SIGNAL);
        sleep(1);
        printf("== Parent ==\nSignal succesfuly ignored\n");
        wait_for_children();
    }
    else
    {
        raise(TESTING_SIGNAL);
        printf("== Child run ==\n");
        sleep(1);
        printf("== Child ==\n Signal succesfuly ignored\n");
    }
    wait_for_children();
}

void handler(int signum)
{
    printf("Handled signal.\n");
}

void test_handle_fork()
{
    printf("\n==== Testing Handle Signals (fork) ====\n");
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(TESTING_SIGNAL, &act, NULL);

    printf("== Parent run ==\n");
    raise(TESTING_SIGNAL);
    sleep(1);

    pid_t childPID;
    if ((childPID = fork()) < 0)
    {
        fprintf(stderr, "Error occured while creating child process\n");
        exit(1);
    }
    else if (childPID > 0)
    {
        printf("== Child run ==\n");
        raise(TESTING_SIGNAL);
        sleep(1);
        exit(0);
    }
    wait_for_children();
}

void test_mask_fork()
{
    printf("\n==== Testing Mask Signals (fork) ====\n");
    sigset_t newmask, pendingMask;
    sigemptyset(&newmask);
    sigaddset(&newmask, TESTING_SIGNAL);
    sigprocmask(SIG_SETMASK, &newmask, NULL);

    bool isPending;
    raise(TESTING_SIGNAL);
    sleep(1);
    sigpending(&pendingMask);
    isPending = sigismember(&pendingMask, TESTING_SIGNAL);
    printf("== Parent ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");

    pid_t childPID;
    if ((childPID = fork()) < 0)
    {
        fprintf(stderr, "Error occured while creating child process\n");
        exit(1);
    }
    else if (childPID > 0)
    {
        raise(TESTING_SIGNAL);
        sleep(1);
        sigpending(&pendingMask);
        isPending = sigismember(&pendingMask, TESTING_SIGNAL);
        printf("== Child ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
        exit(0);
    }
    wait_for_children();
}

void test_pending_fork()
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
    wait_for_children();
}

void test_fork()
{
    test_signal_fork(test_ignore_fork, test_handle_fork, test_mask_fork, test_pending_fork);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected only one argument.\n");
        exit(-1);
    }
    MODE = argv[1];

    test_fork();

    return 0;
}