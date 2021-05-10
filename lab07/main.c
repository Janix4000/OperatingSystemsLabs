#include "lib/sem.h"
#include "lib/shm.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    semaphore_t semid = create_sem("main", 1, 0);
    int shmid = create_shared("main", sizeof(int));
    int *cnt = open_shared("main", 0, sizeof(int));

    *cnt = 0;

    for (size_t i = 0; i < 10000; i++)
    {
        if (fork() == 0)
        {
            int *cnt_fork = open_shared("main", 0, sizeof(int));
            semaphore_t semid_fork = open_sem("main");

            decr_sem(semid_fork);
            // printf("Hopity hopity this sem is my property, ~ %ld\n", i);
            *cnt_fork += 1;
            // printf("Ok, I'll give it back, ~ %ld\n", i);
            incr_sem(semid_fork);
            close_sem(semid_fork);
            close_shared(cnt_fork, sizeof(int));
            exit(0);
        }
    }
    while (wait(NULL) > 0)
        ;
    printf("Result %d\n", *cnt);
    close_sem(semid);
    close_shared(cnt, sizeof(int));

    remove_sem("main");
    remove_shared(shmid, "main");

    return 0;
}
