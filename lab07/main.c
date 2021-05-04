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

#include <fcntl.h>
#include <errno.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux specific) */
};

int main(int argc, char const *argv[])
{
    key_t key = ftok("./id", 'a');
    int semid = semget(key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 0;
    semctl(semid, 0, SETVAL, arg);
    
    
    return 0;
}
