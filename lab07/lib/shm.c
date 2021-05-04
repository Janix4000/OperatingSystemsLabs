#include "types.h"
#include "shm.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

int create_shared(const char *name, size_t size)
{
    int shmid = -1;
    char path[256];
    generate_name(name, path, L_SHM_PREF);

#if defined(L_SYS_V)
    constrait_name_dir(L_SHM_PREF);
    constrait_name_file(path);
    key_t key = ftok(path, L_PROJ);
    shmid = shmget(key, size, IPC_CREAT | 0666);
    const char error[] = "schmget";

#elif defined(L_POSIX)
    const char error[] = "not implemented";
#endif

    if (shmid == -1)
    {
        perror(error);
        return -1;
    }

    return shmid;
}

void *open_shared(const char *name, int shmflags)
{
    void *ptr = (void *)-1;
    char path[256];
    generate_name(name, path, L_SHM_PREF);

#if defined(L_SYS_V)
    key_t key = ftok(path, L_PROJ);
    int shmid = shmget(key, 0, 0666);
    const char error[] = "schmget";

#elif defined(L_POSIX)
    const char error[] = "not implemented";
#endif

    if (shmid == -1)
    {
        perror(error);
        return (void *)-1;
    }

#if defined(L_SYS_V)

    ptr = shmat(shmid, NULL, shmflags);
    const char error2[] = "schmget";

#elif defined(L_POSIX)
    const char error[] = "not implemented";
#endif

    if (ptr == (void *)-1)
    {
        perror(error2);
    }

    return ptr;
}

int close_shared(const void *ptr)
{
    int res = -1;
#if defined(L_SYS_V)
    res = shmdt(ptr);
    const char error[] = "shmdt";

#elif defined(L_POSIX)
    const char error[] = "not implemented";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return -1;
}

int remove_shared(int shmid, const char *name)
{
    int res = -1;

#if defined(L_SYS_V)
    res = shmctl(shmid, IPC_RMID, NULL);
    const char error[] = "shmctl";

#elif defined(L_POSIX)
    char path[256];
    generate_name(name, path, L_SHM_PREF);
    const char error[] = "not implemented";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return res;
}