#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <fcntl.h>

#include <errno.h>

#include "lib/ips.h"

int server_queue = -1;
int own_queue = -1;
int msg_queue = -1;
msgbuf msg;
char id = -1;

void ask_server_for_list();

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

void ask_server_for_connect(int other_id)
{
    msg.mtype = L_CONNECT;
    msg.mtext[0] = id;
    msg.mtext[1] = other_id;
    send_msg_to(&msg, server_queue);
}
void wait_for_connect();

void ask_server_for_disconnect()
{
    msg.mtype = L_DISCONNECT;
    msg.mtext[0] = id;
    send_msg_to(&msg, server_queue);
}
void ask_server_for_stop()
{
    msg.mtype = L_STOP;
    msg.mtext[0] = id;
    send_msg_to(&msg, server_queue);
}

void ask_server_for_list()
{
    msg.mtype = L_LIST;
    msg.mtext[0] = id;
    send_msg_to(&msg, server_queue);

    wait_for_msg_from(&msg, own_queue);
    char *list = msg.mtext;
    char chosen_id = -1;
    for (char *it = list; *it; ++it)
    {
        if (*it == id)
        {
            continue;
        }
        chosen_id = *it;
        printf("I will connect to this dude: %d\n", *it);
        break;
    }
    if (chosen_id == -1)
    {
        printf("No hot mommies in neighbourhood ;'\\\n");
    }
}

void server_down()
{
    printf("Server down...\n");
    server_queue = -1;
    exit(0);
}

void interpret_msg(msgbuf *msg)
{
    switch (msg->mtype)
    {
    case L_STOP:
        server_down();
    default:
        break;
    }
}

int main(int argc, char const *argv[])
{
    own_queue = create_queue("./a");
    apply_destructor(destructor, sigc);

    while ((server_queue = open_queue("./id")) == -1)
    {
        fprintf(stderr, "Waiting for server..\n");
        sleep(1);
    }

    ask_server_for_init();
    ask_server_for_list();

    while (wait_for_msg_from(&msg, own_queue) != -1)
    {
        printf("Received type: %ld\n", msg.mtype);
        interpret_msg(&msg);
    }

    char line[100];
    char *it = line;
    size_t n = 100;
    getline(&it, &n, stdin);

    return 0;
}

void destructor(void)
{
    if (server_queue != -1)
    {
        ask_server_for_stop();
    }
    close_queue(own_queue);
    printf("Client down\n");
}

void sigc(int sig_no)
{
    printf("Ctrl + C\n");
    exit(0);
}
