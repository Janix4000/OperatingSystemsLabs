#pragma once
#include "types.h"

typedef struct msgbuf
{
    long mtype;
    char sender;

    union
    {
        char list[L_MAX_CLIENTS];

        struct
        {
            char id;
            pid_t pid;
        } conn;

        char msg[L_MAX_MSG_SIZE];
    };
} msgbuf;

void close_queue(L_QUEUE queue);
void remove_queue(L_QUEUE queue, pid_t pid);

void apply_destructor(void (*destructor)(void), void (*sigc)(int));

L_QUEUE open_queue(pid_t pid);
L_QUEUE create_queue(pid_t pid);

int send_msg_to(msgbuf *msg, L_QUEUE queue);

// int wait_for_msg_from(msgbuf *msg, L_QUEUE queue);
int get_msg_from(msgbuf *msg, L_QUEUE queue);