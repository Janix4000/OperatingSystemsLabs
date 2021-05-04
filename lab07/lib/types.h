#pragma once
#include <semaphore.h>

#define L_SYS_V
// #define L_POSIX

#if defined(L_SYS_V)
#define L_SEM_PREF "./sem/"
#define semaphore_t int
#define L_FAIL -1
union semun
{
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                                (Linux specific) */
};

#elif defined(L_POSIX)
#define L_SEM_PREF "/sem_"
#define semaphore_t sem_t *
#define L_FAIL SEM_FAILED
#endif

#define L_PROJ 'a'