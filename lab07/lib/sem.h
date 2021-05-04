#pragma once

#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/signal.h>
#include <sys/stat.h>
// #include <sys/msg.h>
// #include <mqueue.h>

#include <fcntl.h>
#include <errno.h>

semaphore_t create_sem(const char *name, int beg_val, int flag);
semaphore_t open_sem(const char *name);
int close_sem(semaphore_t semid);
int remove_sem(const char *name);

int incr_sem(semaphore_t semid);
int decr_sem(semaphore_t semid);