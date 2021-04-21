#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int SIGNAL = SIGUSR1;
int END_SIGNAL = SIGUSR2;

#define TYPE_KILL 0
#define TYPE_SIGQUEUE 1
#define TYPE_SIGRT 2

pid_t PID = 0;
int N = 0;
char *MODE;
int TYPE;

volatile int COUNT = 0;
volatile int CATCHER_COUNT = 0;
volatile sig_atomic_t SHOULD_STOP = false;

void handler(int sig_num, siginfo_t *info, void *ctx);
sigset_t block_signals(struct sigaction *act);
void add_handlers(struct sigaction *act);

void handle_arg(int argc, char **argv);
void send_signals();

void receive_signals(sigset_t *mask);



int main(int argc, char **argv)
{
    argc--;
    argv++;

    handle_arg(argc, argv);

    SIGNAL = TYPE == TYPE_SIGQUEUE ? SIGRTMIN : SIGUSR1;
    END_SIGNAL = TYPE == TYPE_SIGQUEUE ? SIGRTMIN + 1 : SIGUSR2;

    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    sigset_t mask = block_signals(&act);
    add_handlers(&act);

    send_signals();

    receive_signals(&mask);

    printf("=== Sender | %s ==\nGot %d signals\nExpected %d signals\n", MODE, COUNT, N);
    if (TYPE == TYPE_SIGQUEUE)
    {
        printf("Catcher received %d signals\n", CATCHER_COUNT);
    }

    return 0;
}

void handler(int sig_num, siginfo_t *info, void *ctx)
{
    (void)ctx;
    if (sig_num == SIGUSR1 || sig_num == SIGRTMIN)
    {
        ++COUNT;
    }
    else if (sig_num == SIGUSR2 || sig_num == (SIGRTMIN + 1))
    {
        SHOULD_STOP = true;
        if (info->si_code == SI_QUEUE)
        {
            CATCHER_COUNT = info->si_value.sival_int;
        }
    }
};

sigset_t block_signals(struct sigaction *act)
{
    sigemptyset(&act->sa_mask);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigdelset(&mask, SIGNAL);
    sigdelset(&mask, END_SIGNAL);
    act->sa_mask = mask;
    sigprocmask(SIG_SETMASK, &mask, NULL);
    return mask;
}

void add_handlers(struct sigaction *act)
{
    sigaction(SIGNAL, act, NULL);
    sigaction(END_SIGNAL, act, NULL);
}

void handle_arg(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Expected three arguments.\n");
        exit(-1);
    }

    PID = atoi(argv[0]);
    if (PID == 0)
    {
        fprintf(stderr, "Expected positive integer as PID, given \"%s\".\n", argv[0]);
        exit(-1);
    }

    N = atoi(argv[1]);
    if (PID == 0)
    {
        fprintf(stderr, "Expected positive integer as number of signals to send, given \"%s\".\n", argv[1]);
        exit(-1);
    }

    MODE = argv[2];
    if (strcmp(MODE, "SIGQUEUE") == 0)
    {
        TYPE = TYPE_SIGQUEUE;
    }
    else if (strcmp(MODE, "SIGRT") == 0)
    {
        TYPE = TYPE_SIGRT;
    }
    else if (strcmp(MODE, "KILL") == 0)
    {
        TYPE = TYPE_KILL;
    }
    else
    {
        fprintf(stderr, "Not known mode \"%s\".\n", MODE);
        exit(-1);
    }
}

void send_signals()
{
    if (TYPE == TYPE_SIGQUEUE)
    {
        for (size_t i = 0; i < N; ++i)
        {
            sigqueue(PID, SIGNAL, (union sigval){0});
        }
        sigqueue(PID, END_SIGNAL, (union sigval){0});
    }
    else
    {
        for (size_t i = 0; i < N; ++i)
        {
            kill(PID, SIGNAL);
        }
        kill(PID, END_SIGNAL);
    }
}

void receive_signals(sigset_t *mask)
{
    while (!SHOULD_STOP)
    {
        sigsuspend(mask);
    }
}