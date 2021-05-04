#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/signal.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>

int create_shared(const char *name, size_t size);
void *open_shared(const char *name, int shmflags);
int close_shared(const void *ptr);
int remove_shared(int shmid, const char *name);