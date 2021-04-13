#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define TYPE_KILL 0
#define TYPE_SIGQUEUE 1
#define TYPE_SIGRT 2

volatile sig_atomic_t SHOULD_STOP = false;
volatile size_t COUNT = 0;
volatile sig_atomic_t SENDER_PID = 0;
volatile sig_atomic_t SIGNAL = SIGUSR1;
volatile sig_atomic_t END_SIGNAL = SIGUSR2;
volatile sig_atomic_t SENDER_TYPE = TYPE_KILL;

void handler(int sig_num, siginfo_t *info, void *ucontext)
{
    (void)ucontext;
    SENDER_PID = info->si_pid;
    if (sig_num == SIGUSR1 || sig_num == SIGRTMIN)
    {
        ++COUNT;
        SIGNAL = sig_num;
    }
    else if (sig_num == SIGUSR2 || sig_num == (SIGRTMIN + 1))
    {
        SHOULD_STOP = true;
        END_SIGNAL = sig_num;
        if (info->si_code == SI_USER)
        {
            SENDER_TYPE = sig_num == SIGRTMIN + 1 ? TYPE_SIGRT : TYPE_KILL;
        }
        else if (info->si_code == SI_QUEUE)
        {
            SENDER_TYPE = TYPE_SIGQUEUE;
        }
    }
}

sigset_t block_signals(struct sigaction *act)
{
    sigemptyset(&act->sa_mask);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMIN);
    sigdelset(&mask, SIGRTMIN + 1);
    act->sa_mask = mask;
    sigprocmask(SIG_SETMASK, &mask, NULL);
    return mask;
}

void add_handlers(struct sigaction *act)
{
    sigaction(SIGUSR1, act, NULL);
    sigaction(SIGUSR2, act, NULL);
    sigaction(SIGRTMIN, act, NULL);
    sigaction(SIGRTMIN + 1, act, NULL);
}

int main(int argc, char **argv)
{
    int PID = getpid();
    printf("%d\n", PID);

    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    sigset_t mask = block_signals(&act);
    add_handlers(&act);

    while (!SHOULD_STOP)
    {
        sigsuspend(&mask);
    }

    if (SIGNAL == SIGUSR1)
    {
    }

    if (SENDER_TYPE == TYPE_SIGQUEUE)
    {
        for (size_t i = 0; i < COUNT; ++i)
        {
            sigqueue(SENDER_PID, SIGNAL, (union sigval){0});
        }
        sigqueue(SENDER_PID, END_SIGNAL, (union sigval){COUNT});
    }
    else
    {
        for (size_t i = 0; i < COUNT; ++i)
        {
            kill(SENDER_PID, SIGNAL);
        }
        kill(SENDER_PID, END_SIGNAL);
    }

    char MODE[20];
    switch (SENDER_TYPE)
    {
    case TYPE_SIGRT:
        strcpy(MODE, "SIGRT");
        break;
    case TYPE_SIGQUEUE:
        strcpy(MODE, "SIGQUEUE");
        break;
    case TYPE_KILL:
        strcpy(MODE, "KILL");
        break;

    default:
        break;
    }

    printf("=== Catcher | %s ===\nGot %ld signals\n", MODE, COUNT);

    return 0;
}

void wait_for_children()
{
    pid_t wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0)
        ;
}
