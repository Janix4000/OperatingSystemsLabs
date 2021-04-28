#include <sys/stat.h>
#include <fcntl.h>
#include "ips.h"
#include <sys/stat.h>

void close_queue(L_QUEUE queue)
{
#ifdef L_SYS_V
    if (msgctl(queue, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
    }
#else
    mq_close(queue);
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
#ifdef L_SYS_V
    key_t key = ftok(path, L_PROJ);
    L_QUEUE queue = msgget(key, 0);
#else
    L_QUEUE queue = mq_open(path, O_WRONLY);
#endif

    return queue;
}

L_QUEUE create_queue(const char *path)
{
#ifdef L_SYS_V
    key_t key = ftok(path, L_PROJ);
    L_QUEUE queue = msgget(key, IPC_CREAT | 0666);
    const char error[] = "msgget";
#else
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = L_MAXSIZE;
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;
    L_QUEUE queue = mq_open(path, O_RDONLY | O_CREAT | O_NONBLOCK, 0666, &attr);
    const char error[] = "mq_open";
#endif // L_SYS_V
    if (queue == -1)
    {
        perror(error);
        exit(-1);
    }
    return queue;
}

int send_msg_to(msgbuf *msg, L_QUEUE queue)
{
    int res;
#ifdef L_SYS_V
    res = msgsnd(queue, msg, sizeof(msg->mtext), 0);
    const char error[] = "msgsnd";
#else
    res = mq_send(queue, msg->mtext, sizeof(msg->mtext), msg->mtype);
    const char error[] = "mq_send";
#endif
    if (res == -1)
    {
        perror(error);
    }
    msg->mtext[0] = '\0';
    return res;
}

int get_msg_from(msgbuf *msg, L_QUEUE queue)
{
    int res;
#ifdef L_SYS_V
    res = msgrcv(queue, msg, sizeof(msg->mtext), -L_COUNT, IPC_NOWAIT);
#else
    unsigned type;
    res = mq_receive(queue, msg->mtext, sizeof(msg->mtext), &type);
    msg->mtype = (long)type;
#endif

    return res;
}

int wait_for_msg_from(msgbuf *msg, L_QUEUE queue)
{
    int res;
#ifdef L_SYS_V
    res = msgrcv(queue, msg, sizeof(msg->mtext), -L_COUNT, 0);
    const char error[] = "msgrcv";
#else
    res = get_msg_from(msg, queue);
    const char error[] = "mq_receive";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return res;
}
