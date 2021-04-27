#include "lib/ips.h"
#include "lib/client_vector.h"

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

LibVector avaible;
LibVector connected;

char add_client_to_lists(L_QUEUE queue);
L_QUEUE remove_client_from_lists(char id);
L_QUEUE make_client_avaible(char id);
L_QUEUE make_client_connected(char id);
void print_lists();
L_QUEUE get_queue(char id);
bool is_avaible(char id);
void disconnect_pair(char id);
void disconnect_one_from_pair(char id);
void connect_clients(char id, char other_id);
void remove_pair(char id);
char get_second_id_from_pair(int id);

void init_msg(msgbuf *msg)
{
    L_QUEUE q;
    strncpy((char *)&q, msg->mtext, sizeof(int));

    char id = add_client_to_lists(q);

    printf("Client init received, given id: %d\n", id);

    msg->mtype = L_INIT;
    msg->mtext[0] = id;
    msg->mtext[1] = '\0';

    send_msg_to(msg, q);
}

void stop_client_msg(msgbuf *msg)
{
    char id = msg->mtext[0];
    if (!is_avaible(id))
    {
        char other_id = get_second_id_from_pair(id);
        disconnect_one_from_pair(other_id);
    }
    remove_client_from_lists(id);
}
void list_msg(msgbuf *msg)
{
    char id = msg->mtext[0];
    L_QUEUE q = get_queue(id);
    msg->mtype = L_LIST;
    for (size_t i = 0; i < avaible.size; i++)
    {
        msg->mtext[i] = avaible.container[i].id;
    }
    msg->mtext[avaible.size] = '\0';

    send_msg_to(msg, q);
}
void connect_msg(msgbuf *msg)
{
    char id = msg->mtext[0];
    char other_id = msg->mtext[1];
    L_QUEUE q = get_queue(id);
    if (id == other_id)
    {
        msg->mtype = L_FAIL;
        strcpy(msg->mtext, "Same ID");
        send_msg_to(msg, q);
        return;
    }
    if (!is_avaible(id) || !is_avaible(other_id))
    {
        msg->mtype = L_FAIL;
        strcpy(msg->mtext, "Not avaible ID");
        send_msg_to(msg, q);
        return;
    }
    L_QUEUE other_q = get_queue(other_id);

    connect_clients(id, other_id);

    msg->mtype = L_CONNECT;
    strncpy(msg->mtext, (char *)&other_q, sizeof(int));
    msg->mtext[sizeof(int)] = other_id;
    send_msg_to(msg, q);

    msg->mtype = L_CONNECT;
    strncpy(msg->mtext, (char *)&q, sizeof(int));
    msg->mtext[sizeof(int)] = id;
    send_msg_to(msg, other_q);
}

void disconnect_msg(msgbuf *msg)
{
    char id = msg->mtext[0];
    if (is_avaible(id))
    {
        fprintf(stderr, "Client is already avaible\n");
        return;
    }

    disconnect_pair(id);
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
        break;
    case L_LIST:
        list_msg(msg);
        break;
    case L_CONNECT:
        connect_msg(msg);
        break;
    case L_DISCONNECT:
        disconnect_msg(msg);
        break;
    default:
        break;
    }
}

void destructor(void);
void sigc(int);

int main(int argc, char const *argv[])
{
    vecInit(&avaible, 256);
    vecInit(&connected, 256);

    server_queue = create_queue("./id");
    printf("My queue: %d\n", server_queue);
    apply_destructor(destructor, sigc);

    while (wait_for_msg_from(&msg, server_queue) != -1)
    {
        interpret_msg(&msg);
        print_lists();
    }

    perror("msgrcv");

    return 0;
}
client *find_client_in(LibVector *clients, char id)
{
    for (size_t i = 0; i < clients->size; i++)
    {
        if (clients->container[i].id == id)
        {
            return clients->container + i;
        }
    }
    return NULL;
}

L_QUEUE get_queue(char id)
{
    client *it = NULL;
    it = find_client_in(&avaible, id);
    if (!it)
    {
        it = find_client_in(&connected, id);
    }
    return it->queue;
}

bool is_avaible(char id)
{
    return find_client_in(&avaible, id) != NULL;
}

void disconnect_one_from_pair(char id)
{
    int q = make_client_avaible(id);
    msg.mtype = L_DISCONNECT;
    send_msg_to(&msg, q);
}

char get_second_id_from_pair(int id)
{
    return find_client_in(&connected, id)->conn_id;
}

void disconnect_pair(char id)
{
    char other_id = get_second_id_from_pair(id);
    disconnect_one_from_pair(id);
    disconnect_one_from_pair(other_id);
}

void connect_clients(char id, char other_id)
{
    client *first = find_client_in(&avaible, id);
    first->conn_id = other_id;
    client *second = find_client_in(&avaible, other_id);
    second->conn_id = id;

    make_client_connected(id);
    make_client_connected(other_id);
}

L_QUEUE make_client_connected(char id)
{
    client *it = find_client_in(&avaible, id);
    if (it)
    {
        L_QUEUE queue = it->queue;
        vecPushBack(&connected, *it);
        *it = vecPopBack(&avaible);
        return queue;
    }
    else
    {
        fprintf(stderr, "No such client\n");
        return -1;
    }
}

L_QUEUE make_client_avaible(char id)
{
    client *it = find_client_in(&connected, id);
    if (it)
    {
        L_QUEUE queue = it->queue;
        it->conn_id = -1;
        vecPushBack(&avaible, *it);
        *it = vecPopBack(&connected);
        return queue;
    }
    else
    {
        fprintf(stderr, "No such client\n");
        return -1;
    }
}

L_QUEUE remove_client_from_lists(char id)
{
    client *it = find_client_in(&avaible, id);
    L_QUEUE queue = -1;
    if (it)
    {
        queue = it->queue;
        *it = vecPopBack(&avaible);
        // remove_pair_from_list(id);
    }
    else
    {
        it = find_client_in(&connected, id);
        if (it)
        {
            queue = it->queue;
            *it = vecPopBack(&connected);
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
    msg.mtext[0] = '\0';
    while (avaible.size)
    {
        msg.mtype = L_STOP;
        L_QUEUE q = remove_client_from_lists(avaible.container[0].id);
        send_msg_to(&msg, q);
    }
    while (connected.size)
    {
        msg.mtype = L_STOP;
        L_QUEUE q = remove_client_from_lists(connected.container[0].id);
        send_msg_to(&msg, q);
    }
}

char add_client_to_lists(L_QUEUE queue)
{
    char id = next_id++;
    client c;
    c.id = id;
    c.queue = queue;
    c.conn_id = -1;
    vecPushBack(&avaible, c);
    return id;
}

void print_lists()
{
    printf("Avaible :[");
    if (avaible.size)
    {
        for (size_t i = 0; i < avaible.size - 1; i++)
        {
            printf("%d ", avaible.container[i].id);
        }
        printf("%d", avaible.container[avaible.size - 1].id);
    }
    printf("]\nUnavaible[");
    if (connected.size)
    {
        for (size_t i = 0; i < connected.size - 1; i++)
        {
            printf("%d ", connected.container[i].id);
        }
        printf("%d", connected.container[connected.size - 1].id);
    }
    printf("]\n\n");
}

void destructor(void)
{
    stop_all_clients();
    vecFree(&avaible);
    vecFree(&connected);
    close_queue(server_queue);
    printf("Server down\n");
}

void sigc(int sig_no)
{
    printf("Ctrl + C\n");
    exit(0);
}