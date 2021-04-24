#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>

#include <errno.h>

#include "lib/ips.h"

int server_queue = -1;
int own_queue = -1;
int msg_queue = -1;
msgbuf msg;
char id = -1;

bool ask_server_for_list();

void destructor(void);
void sigc(int sig_no);

void ask_server_for_init()
{
    msg.mtype = L_INIT;
    union
    {
        int q;
        char tag[sizeof(int)];
    } data;
    data.q = own_queue;

    strncpy(msg.mtext, data.tag, sizeof(int));
    msg.mtype = L_INIT;

    send_msg_to(&msg, server_queue);
    wait_for_msg_from(&msg, own_queue);
    id = msg.mtext[0];
    printf("My new id: %d\n", id);
}

void ask_server_for_connect(int id);
void wait_for_connect();

void ask_server_for_disconnect();
void ask_server_for_stop();

int main(int argc, char const *argv[])
{
    server_queue = open_queue("./id");
    own_queue = open_queue("./a");
    apply_destructor(destructor, sigc);

    ask_server_for_init();

    // ask_server_for_list();
    // connect_to_first_open();

    // int bytes;

    // msg.mtype = 10;

    // char *line = msg.mtext;
    // size_t line_size = sizeof(msg.mtext);

    // msg.mtext[0] = '\0';

    // while ((bytes = getline(&line, &line_size, stdin)) != -1)
    // {
    //     char *endline = strchr(line, '\n');
    //     if (endline)
    //     {
    //         *endline = '\0';
    //     }
    //     if ((bytes = msgsnd(server_queue, &msg, sizeof(msg.mtext), 0)) == -1)
    //     {
    //         perror("msgsend");
    //     }
    //     msg.mtext[0] = '\0';
    // }

    // printf("Bytes: %d\n", bytes);

    return 0;
}

void destructor(void)
{
    close_queue(own_queue);
    printf("Client down\n");
}

void sigc(int sig_no)
{
    printf("Ctrl + C\n");
    destructor();
}
