#pragma once
#include "types.h"

#define L_SYS_V
// #define L_POSIX

#ifdef L_SYS_V
#define L_QUEUE int
#else
#define L_QUEUE int
#endif

typedef struct msgbuf
{
    long mtype;
    char mtext[L_MAXSIZE];
} msgbuf;

void close_queue(L_QUEUE queue);

void apply_destructor(void (*destructor)(void), void (*sigc)(int));

L_QUEUE open_queue(const char *path);
L_QUEUE create_queue(const char *path);

int send_msg_to(msgbuf *msg, L_QUEUE queue);

int wait_for_msg_from(msgbuf *msg, L_QUEUE queue);