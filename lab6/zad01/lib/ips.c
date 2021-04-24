
#include "ips.h"

void close_queue(L_QUEUE queue)
{
#ifdef L_SYS_V
    if (msgctl(queue, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
    }
#endif // DEBUG
}

void apply_destructor(void (*destructor)(void), void (*sigc)(int))
{
    atexit(destructor);

    struct sigaction act;
    act.sa_handler = sigc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

L_QUEUE open_queue(const char *path)
{
    key_t key = ftok("./id", L_PROJ);
    L_QUEUE queue = msgget(key, IPC_CREAT | 0666);
    if (queue == -1)
    {
        perror("msget");
        exit(-1);
    }
    return queue;
}

int send_msg_to(msgbuf *msg, L_QUEUE queue)
{
    int res;
    if ((res = msgsnd(queue, msg, sizeof(msg->mtext), 0)) == -1)
    {
        perror("msgsend");
    }
    msg->mtext[0] = '\0';
    return res;
}

int wait_for_msg_from(msgbuf *msg, L_QUEUE queue)
{
    int res;
    if ((res = msgrcv(queue, msg, sizeof(msg->mtext), -L_COUNT, 0)) == -1)
    {
        perror("msgrcv");
    }
    return res;
}