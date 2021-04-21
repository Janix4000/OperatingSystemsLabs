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
volatile sig_atomic_t SHOULD_COUNT = true;
volatile sig_atomic_t RECEIVED_PONG = false;

char MODE[20];

void handler(int sig_num, siginfo_t *info, void *ucontext);
sigset_t block_signals(struct sigaction *act);
void add_handlers(struct sigaction *act);

void guess_mode();
void receive_signals(sigset_t *mask);
void send_signals();

int main(int argc, char **argv)
{
    int PID = getpid();
    printf("%d\n", PID);

    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    sigset_t mask = block_signals(&act);
    add_handlers(&act);

    receive_signals(&mask);

    send_signals(&mask);

    guess_mode();

    printf("=== Catcher | %s ===\nGot %ld signals\n", MODE, COUNT);

    return 0;
}

void handler(int sig_num, siginfo_t *info, void *ucontext)
{
    (void)ucontext;
    SENDER_PID = info->si_pid;
    if (sig_num == SIGUSR1 || sig_num == SIGRTMIN)
    {
        if (SHOULD_COUNT)
        {
            ++COUNT;
        }
        RECEIVED_PONG = true;
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

void add_handlers(struct sigaction *act)
{
    sigaction(SIGUSR1, act, NULL);
    sigaction(SIGUSR2, act, NULL);
    sigaction(SIGRTMIN, act, NULL);
    sigaction(SIGRTMIN + 1, act, NULL);
}

sigset_t block_signals(struct sigaction *act)
{
    sigemptyset(&act->sa_mask);
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);

    sigprocmask(SIG_SETMASK, &mask, NULL);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMIN);
    sigdelset(&mask, SIGRTMIN + 1);
    act->sa_mask = mask;
    return mask;
}

void guess_mode()
{
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
}

void receive_signals(sigset_t *mask)
{
    SHOULD_COUNT = true;
    RECEIVED_PONG = false;
    while (!SHOULD_STOP)
    {
        if (!RECEIVED_PONG)
        {
            sigsuspend(mask);
        }
        RECEIVED_PONG = false;
        if (SENDER_TYPE == TYPE_SIGQUEUE)
        {
            sigqueue(SENDER_PID, SIGNAL, (union sigval){0});
        }
        else
        {
            kill(SENDER_PID, SIGNAL);
        }
    }
}

void send_signals(sigset_t *mask)
{
    SHOULD_COUNT = false;
    RECEIVED_PONG = false;
    if (SENDER_TYPE == TYPE_SIGQUEUE)
    {
        for (size_t i = 0; i < COUNT; ++i)
        {
            sigqueue(SENDER_PID, SIGNAL, (union sigval){0});
            if (!RECEIVED_PONG)
            {
                sigsuspend(mask);
            }
            RECEIVED_PONG = false;
        }

        sigqueue(SENDER_PID, END_SIGNAL, (union sigval){COUNT});
    }
    else
    {
        for (size_t i = 0; i < COUNT; ++i)
        {
            kill(SENDER_PID, SIGNAL);
            if (!RECEIVED_PONG)
            {
                sigsuspend(mask);
            }
            RECEIVED_PONG = false;
        }
        kill(SENDER_PID, END_SIGNAL);
    }
}
