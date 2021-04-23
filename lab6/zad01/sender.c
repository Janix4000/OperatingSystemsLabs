#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

typedef struct msgbuf
{
    long mtype;
    char mtext[80];
} msgbuf;

int main(int argc, char const *argv[])
{
    key_t key = ftok("./id", 'a');
    printf("Key: %X\n", key);

    int queue = msgget(key, IPC_CREAT | 0666);
    if (queue == -1)
    {
        perror("msget");
        exit(-1);
    }
    perror("msget");
    printf("Queue: %X\n", queue);

    msgbuf msg;
    int bytes;

    msg.mtype = 10;

    char *line = msg.mtext;
    size_t line_size = sizeof(msg.mtext);

    msg.mtext[0] = '\0';

    while ((bytes = getline(&line, &line_size, stdin)) != -1)
    {
        char *endline = strchr(line, '\n');
        if (endline)
        {
            *endline = '\0';
        }
        if ((bytes = msgsnd(queue, &msg, sizeof(msg.mtext), 0)) == -1)
        {
            perror("msgsend");
        }
        msg.mtext[0] = '\0';
    }

    printf("Bytes: %d\n", bytes);

    return 0;
}
