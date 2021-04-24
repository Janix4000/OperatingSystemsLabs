#include "lib/ips.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

msgbuf msg;
int server_queue;
int next_id = 1;

typedef struct
{
    char id;
    L_QUEUE queue;
} client;

client avaible_clients[128];
size_t n_avaible = 0;

client not_avaible_clients[128];
size_t n_not_avaible = 0;

char add_client(L_QUEUE queue);
L_QUEUE remove_client(char id);
L_QUEUE connect_client(char id);
L_QUEUE disconnect_client(char id);
void print_lists();

void init_msg(msgbuf *msg)
{
    union
    {
        int q;
        char tag[sizeof(int)];
    } data;
    strncpy(data.tag, msg->mtext, sizeof(int));
    char id = add_client(data.q);

    printf("Client init received, given id: %d\n", id);

    msg->mtype = L_INIT;
    msg->mtext[0] = id;
    msg->mtext[1] = '\0';

    send_msg_to(msg, data.q);
    print_lists();
}

void stop_client_msg(msgbuf *msg)
{
    char id = msg->mtext[0];
    remove_client(id);
    print_lists();
}

void interpret_msg(msgbuf *msg)
{
    switch (msg->mtype)
    {
    case L_INIT:
        init_msg(msg);
        break;
    case L_STOP:
        stop_client_msg(msg);
    default:
        break;
    }
}

void destructor(void);
void sigc(int);

int main(int argc, char const *argv[])
{
    server_queue = create_queue("./id");
    apply_destructor(destructor, sigc);

    while (wait_for_msg_from(&msg, server_queue) != -1)
    {
        printf("Received type: %ld\n", msg.mtype);
        interpret_msg(&msg);
    }

    perror("msgrcv");

    return 0;
}
client *find(client *clients, int n, char id)
{
    for (size_t i = 0; i < n; i++)
    {
        if (clients[i].id == id)
        {
            return clients + i;
        }
    }
    return NULL;
}

L_QUEUE disconnect_client(char id)
{
    client *it = find(avaible_clients, n_avaible, id);
    if (it)
    {
        L_QUEUE queue = it->queue;
        not_avaible_clients[n_not_avaible++] = *it;
        *it = avaible_clients[--n_avaible];
        return queue;
    }
    else
    {
        fprintf(stderr, "No such client\n");
        return -1;
    }
}

L_QUEUE connect_client(char id)
{
    client *it = find(not_avaible_clients, n_not_avaible, id);
    if (it)
    {
        L_QUEUE queue = it->queue;
        avaible_clients[n_avaible++] = *it;
        *it = not_avaible_clients[--n_not_avaible];
        return queue;
    }
    else
    {
        fprintf(stderr, "No such client\n");
        return -1;
    }
}

L_QUEUE remove_client(char id)
{
    client *it = find(avaible_clients, n_avaible, id);
    L_QUEUE queue = -1;
    if (it)
    {
        queue = it->queue;
        *it = avaible_clients[--n_avaible];
    }
    else
    {
        it = find(not_avaible_clients, n_not_avaible, id);
        if (it)
        {
            queue = it->queue;
            *it = not_avaible_clients[--n_not_avaible];
        }
    }
    if (queue == -1)
    {
        fprintf(stderr, "No such client\n");
    }
    return queue;
}

void stop_all_clients()
{
    msg.mtype = L_STOP;
    msg.mtext[0] = '\0';
    while (n_avaible)
    {
        L_QUEUE q = remove_client(avaible_clients[0].id);
        send_msg_to(&msg, q);
    }
    while (n_not_avaible)
    {
        L_QUEUE q = remove_client(not_avaible_clients[0].id);
        send_msg_to(&msg, q);
    }
}

char add_client(L_QUEUE queue)
{
    char id = next_id++;
    client c;
    c.id = id;
    c.queue = queue;
    avaible_clients[n_avaible++] = c;
    return id;
}

void print_lists()
{
    printf("[");
    if (n_avaible)
    {
        for (size_t i = 0; i < n_avaible - 1; i++)
        {
            printf("%d ", avaible_clients[i].id);
        }
        printf("%d", avaible_clients[n_avaible - 1].id);
    }
    printf("]\n[");
    if (n_not_avaible)
    {
        for (size_t i = 0; i < n_not_avaible - 1; i++)
        {
            printf("%d ", not_avaible_clients[i].id);
        }
        printf("%d", not_avaible_clients[n_not_avaible - 1].id);
    }
    printf("]\n");
}

void destructor(void)
{
    stop_all_clients();
    close_queue(server_queue);
    printf("Server down\n");
}

void sigc(int sig_no)
{
    printf("Ctrl + C\n");
    exit(0);
}