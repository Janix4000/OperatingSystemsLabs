#include "sem.h"

#include "types.h"

#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>

// void apply_destructor(void (*destructor)(void), void (*sigc)(int))
// {
//     atexit(destructor);

//     struct sigaction act;
//     act.sa_handler = sigc;
//     sigemptyset(&act.sa_mask);
//     act.sa_flags = 0;
//     sigaction(SIGINT, &act, NULL);
// }

semaphore_t create_sem(const char *name, int beg_val, int flag)
{
    semaphore_t semid = L_FAIL;
    char path[256];
    generate_name(name, path, L_SEM_PREF);
#if defined(L_SYS_V)
    constrait_name_dir(L_SEM_PREF);
    constrait_name_file(path);

    key_t key = ftok(path, L_PROJ);
    semid = semget(key, 1, IPC_CREAT | 0666);
    const char error[] = "semget";
#elif defined(L_POSIX)
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msgbuf);
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;
    L_QUEUE queue = mq_open(path, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);
    const char error[] = "mq_open";
#endif // L_SYS_V
    if (semid == L_FAIL)
    {
        perror(error);
        exit(-1);
    }

#if defined(L_SYS_V)
    union semun arg;
    arg.val = beg_val;
    semctl(semid, 0, SETVAL, arg);
#elif defined(L_POSIX)

#endif // L_SYS_V

    return semid;
}

semaphore_t open_sem(const char *name)
{
    semaphore_t semid = L_FAIL;
    char path[256];
    generate_name(name, path, L_SEM_PREF);
#if defined(L_SYS_V)
    constrait_name_dir(L_SEM_PREF);
    constrait_name_file(path);

    key_t key = ftok(path, L_PROJ);
    semid = semget(key, 0, 0666);
    const char error[] = "semget";
#elif defined(L_POSIX)
    const char error[] = "mq_open";
#endif // L_SYS_V
    if (semid == L_FAIL)
    {
        perror(error);
        exit(-1);
    }
    return semid;
}

int close_sem(semaphore_t semid)
{
#if defined(L_SYS_V)
    return 0;
#elif defined(L_POSIX)
    const char error[] = "mq_open";
    return -1;
#endif
}

int remove_sem(const char *name)
{
    semaphore_t semid = L_FAIL;
    char path[256];
    generate_name(name, path, L_SEM_PREF);
#if defined(L_SYS_V)
    key_t key = ftok(path, L_PROJ);
    semid = semget(key, 0, 0666);
    union semun arg;
    semctl(semid, 1, IPC_RMID, arg);
    const char error[] = "remove_sem semctl";
#elif defined(L_POSIX)
    const char error[] = "mq_open";
#endif // L_SYS_V
    if (semid == L_FAIL)
    {
        perror(error);
        return -1;
    }
    return 0;
}

int incr_sem(semaphore_t semid)
{
    int res = -1;
#ifdef L_SYS_V
    struct sembuf op = {.sem_num = 0, .sem_op = 1, .sem_flg = 0};
    res = semop(semid, &op, 1);
    const char error[] = "semop";
#else
    const char error[] = "mq_send";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return res;
}

int decr_sem(semaphore_t semid)
{
    int res = -1;
#ifdef L_SYS_V
    struct sembuf op = {.sem_num = 0, .sem_op = -1, .sem_flg = 0};
    res = semop(semid, &op, 1);
    const char error[] = "semop";
#else
    const char error[] = "mq_send";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return res;
}