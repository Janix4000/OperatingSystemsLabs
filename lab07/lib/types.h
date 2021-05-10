#pragma once
#include <semaphore.h>

#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>

// #define L_SYS_V
#define L_POSIX

#if defined(L_SYS_V)
#define L_SEM_PREF "./sem/"
#define L_SHM_PREF "./shm/"
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
#define L_SHM_PREF "/shm_"
#define semaphore_t sem_t *
#define L_FAIL SEM_FAILED
#endif

#define L_PROJ 'a'

static inline void generate_name(const char *name, char *buff, const char *prefix)
{
    strcpy(buff, prefix);
    buff += strlen(prefix);
    strcpy(buff, name);
}

static inline int constrait_name_dir(const char *dir)
{
    if (access(dir, R_OK | W_OK) != 0)
    {
        if (mkdir(dir, 0777) != 0)
        {
            perror("mkdir");
            return -1;
        }
    }
    return 0;
}

static inline int constrait_name_file(const char *filename)
{
    if (access(filename, R_OK | W_OK) != 0)
    {
        int fd;

        if ((fd = creat(filename, 0666)) == -1)
        {
            perror("creat");
            return -1;
        }

        close(fd);
    }
    return 0;
}
