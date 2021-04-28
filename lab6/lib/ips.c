#include "ips.h"

#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <math.h>
void close_queue(L_QUEUE queue)
{
#ifdef L_SYS_V
    // nothing
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
#if defined(L_SYS_V)
#define L_QUEUE_DIR "./chat/"
#endif

void generate_name(pid_t pid, char *buff)
{
#if defined(L_SYS_V)
    const char prefix[] = L_QUEUE_DIR;
#elif defined(L_POSIX)
    const char prefix[] = "/chat_";
#endif
    strcpy(buff, prefix);
    buff += strlen(prefix);
    if (pid == 0)
    {
        strcpy(buff, "server");
    }
    else
    {
        int n = log10(pid) + 1;
        for (int i = n - 1; i >= 0; --i, pid /= 10)
        {
            buff[i] = (pid % 10) + '0';
        }
        buff[n] = '\0';
    }
}

void remove_queue(L_QUEUE queue, pid_t pid)
{
#ifdef L_SYS_V
    if (msgctl(queue, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
    }
    if (access(L_QUEUE_DIR, R_OK | W_OK) == 0)
    {
        char path[256];
        generate_name(pid, path);
        if (access(path, R_OK | W_OK) != 0)
        {
            remove(path);
        }
    }

#else
    char path[256];
    generate_name(pid, path);
    if (mq_unlink(path) != 0)
    {
        perror("mq_unlink");
    }
#endif // DEBUG
}

L_QUEUE open_queue(pid_t pid)
{
    char path[256];
    generate_name(pid, path);
#ifdef L_SYS_V

    key_t key = ftok(path, L_PROJ);
    L_QUEUE queue = msgget(key, 0);
    // const char error[] = "msgget";
#else
    L_QUEUE queue = mq_open(path, O_WRONLY);
    // const char error[] = "mq_open";
#endif
    // if (queue == -1)
    // {
    //     perror(error);
    // }

    return queue;
}

L_QUEUE create_queue(pid_t pid)
{
    char path[256];
    generate_name(pid, path);
#ifdef L_SYS_V
    if (access(L_QUEUE_DIR, R_OK | W_OK) != 0)
    {
        if (mkdir(L_QUEUE_DIR, 0777) != 0)
        {
            perror("mkdir");
            return -1;
        }
    }

    if (access(path, R_OK | W_OK) != 0)
    {
        int fd;

        if ((fd = creat(path, 0666)) == -1)
        {
            perror("creat");
            return -1;
        }

        close(fd);
    }

    key_t key = ftok(path, L_PROJ);
    L_QUEUE queue = msgget(key, IPC_CREAT | 0666);
    const char error[] = "msgget";
#else
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msgbuf);
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
    res = msgsnd(queue, msg, sizeof(*msg) - sizeof(msg->mtype), 0);
    const char error[] = "msgsnd";
#else
    res = mq_send(queue, (char *)msg, sizeof(*msg), msg->mtype);
    const char error[] = "mq_send";
#endif
    if (res == -1)
    {
        perror(error);
    }
    return res;
}

int get_msg_from(msgbuf *msg, L_QUEUE queue)
{
    int res;
#ifdef L_SYS_V
    res = msgrcv(queue, msg, sizeof(*msg) - sizeof(msg->mtype), -L_COUNT, IPC_NOWAIT);
#else
    unsigned type;
    res = mq_receive(queue, (char *)msg, sizeof(*msg), &type);
    msg->mtype = (long)type;
#endif

    return res;
}

// int wait_for_msg_from(msgbuf *msg, L_QUEUE queue)
// {
//     int res;
// #ifdef L_SYS_V
//     res = msgrcv(queue, msg, sizeof(msg->mtext), -L_COUNT, 0);
//     const char error[] = "msgrcv";
// #else
//     res = get_msg_from(msg, queue);
//     const char error[] = "mq_receive";
// #endif
//     if (res == -1)
//     {
//         perror(error);
//     }
//     return res;
// }
