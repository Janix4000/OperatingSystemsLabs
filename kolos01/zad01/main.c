#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int sig, siginfo_t *siginfo, void *context)
{
    printf("Value: %d\n", siginfo->si_value.sival_int);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigset_t newmask;
    sigemptyset(&newmask);
    sigfillset(&newmask);
    sigdelset(&newmask, SIGUSR1);
    sigdelset(&newmask, SIGINT);
    sigaction(SIGUSR1, &action, NULL);
    sigprocmask(SIG_SETMASK, &newmask, NULL);

    pid_t child = fork();
    if (child == 0)
    {
        sigsuspend(&newmask);

        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
    }
    else
    {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        union sigval value;
        value.sival_int = atoi(argv[1]);
        sigqueue(child, atoi(argv[2]), value);
    }

    return 0;
}