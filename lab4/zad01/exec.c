#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

char *MODE;
int TESTING_SIGNAL;

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
    raise(TESTING_SIGNAL);
    printf("== Child run ==\n");
    sleep(1);
    printf("== Child ==\n Signal succesfuly ignored\n");
}

void handler(int signum)
{
    printf("Handled signal.\n");
}

void test_handle_fork()
{
    printf("== Child run ==\n");
    raise(TESTING_SIGNAL);
    sleep(1);
}

void test_pending_fork()
{
    sigset_t pendingMask;
    bool isPending;

    sleep(1);
    sigpending(&pendingMask);
    isPending = sigismember(&pendingMask, TESTING_SIGNAL);
    printf("== Child ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
}
void test_mask_fork()
{
    raise(TESTING_SIGNAL);
    test_pending_fork();
}

void test()
{
    test_signal_fork(test_ignore_fork, test_handle_fork, test_mask_fork, test_pending_fork);
}

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        fprintf(stderr, "Expected two arguments.\n");
        exit(-1);
    }

    TESTING_SIGNAL = atoi(argv[1]);
    if (TESTING_SIGNAL == 0)
    {
        fprintf(stderr, "Not suported signall \"%s\" given.\n");
        exit(-1);
    }
    MODE = argv[2];

    test();
    return 0;
}