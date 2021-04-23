#include <stdlib.h>
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

    while ((bytes = msgrcv(queue, &msg, sizeof(msg.mtext), 0, 0)) != -1)
    {
        printf("Received: \"%s\"\n", msg.mtext);
    }

    perror("msgrcv");
    printf("%d\n", bytes);

    if (msgctl(queue, IPC_RMID, NULL) != 0)
    {
        perror("msgctl");
    }

    return 0;
}
