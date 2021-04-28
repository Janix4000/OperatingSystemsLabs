#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include <errno.h>

// #define L_SYS_V
#define L_POSIX

#ifdef L_SYS_V
#define L_QUEUE int
#define SERVER_QUEUE_PATH "/home/janix/"
#else
#define L_QUEUE mqd_t
#define SERVER_QUEUE_PATH "/home"
#endif

#define L_COUNT 8
#define L_FAIL 7
#define L_MSG 6
#define L_INIT 5
#define L_CONNECT 4
#define L_LIST 3
#define L_DISCONNECT 2
#define L_STOP 1

#define L_MAXSIZE 80

#define L_PROJ 'a'