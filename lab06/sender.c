#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <fcntl.h>
#include <fcntl.h>

#include <errno.h>

#include "lib/ips.h"

int server_queue = -1;
int own_queue = -1;
int msg_queue = -1;
msgbuf msg;
char own_id = -1;
char msg_id = -1;

void ask_server_for_list();

void destructor(void);
void sigc(int sig_no);

void ask_server_for_init();
void ask_server_for_connect(char other_id);
void wait_for_connect();
void ask_server_for_disconnect();
void ask_server_for_stop();
void ask_server_for_list();
void server_down();

void interpret_msg(msgbuf *msg)
{
    switch (msg->mtype)
    {
    case L_INIT:
    {
        own_id = msg->conn.id;
        printf("My new id: %d\n", own_id);
    }
    break;
    case L_STOP:
        server_down();
        break;
    case L_LIST:
    {
        char *list = msg->list;
        int n = 0;
        for (char *it = list; *it; ++it)
        {
            if (*it == own_id)
            {
                continue;
            }
            n++;
        }
        if (n == 0)
        {
            printf("No avaible clients.\n");
        }
        else
        {
            printf("List:\n");
            for (char *it = list; *it; ++it)
            {
                if (*it == own_id)
                {
                    continue;
                }
                printf("%d\n", *it);
            }
        }
    }
    // list_msg(msg);
    break;
    case L_CONNECT:
    {
        pid_t msg_pid = msg->conn.pid;
        msg_id = msg->conn.id;
        msg_queue = open_queue(msg_pid);
        printf("Polaczono z %d\n", msg_id);
    }
    break;
    case L_DISCONNECT:
    {
        printf("Rozlaczono sie z [%d].\n", msg_id);
        msg_queue = -1;
        msg_id = -1;
    }
    break;
    case L_MSG:
    {
        printf("[%d]: \"%s\"\n", msg_id, msg->msg);
    }
    break;
    case L_FAIL:
    {
        printf("SERVER_FAIL: \"%s\"\n", msg->msg);
    }
    break;
    default:
        break;
    }
}

void init()
{
    own_queue = create_queue(getpid());
    apply_destructor(destructor, sigc);
    printf("My queue %d\n", own_queue);

    while ((server_queue = open_queue(0)) == -1)
    {
        fprintf(stderr, "Waiting for server..\n");
        sleep(1);
    }

    ask_server_for_init();
}

void main_loop();

int main(int argc, char const *argv[])
{
    init();

    main_loop();

    return 0;
}

// LIST
// CONNECT ID
// DISCONNECT
// EXIT

bool execute_command(char *cin)
{
    char comm[60];
    int id = -1;
    sscanf(cin, "%s %d", comm, &id);
    for (char *p = comm; *p; ++p)
        *p = tolower(*p);
    if (strcmp(comm, "list") == 0)
    {
        ask_server_for_list();
    }
    else if (strcmp(comm, "connect") == 0)
    {
        if (id < 0 || id > 254)
        {
            printf("Bad id given\n");
        }
        else if (msg_id != -1)
        {
            printf("Disconnect first.\n");
        }
        else
        {
            ask_server_for_connect(id);
        }
    }
    else if (strcmp(comm, "disconnect") == 0)
    {
        if (msg_id == -1)
        {
            printf("Already not connected\n");
        }
        else
        {
            ask_server_for_disconnect();
        }
    }
    else if (strcmp(comm, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        if (msg_queue == -1)
        {
            printf("Uknown command: %s\n", comm);
        }
        return 0;
    }
    return 1;
}

void main_loop()
{
    char cin[64];
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set read_set;
    FD_ZERO(&read_set);
    while (true)
    {
        if (get_msg_from(&msg, own_queue) >= 0)
        {
            interpret_msg(&msg);
        }

        FD_SET(STDIN_FILENO, &read_set); // dodajemy STDIN_FILENO
        select(STDIN_FILENO + 1, &read_set, NULL, NULL, &timeout);
        if (FD_ISSET(STDIN_FILENO, &read_set))
        {
            if (fgets(cin, sizeof(cin), stdin) != NULL)
            {
                bool executed = execute_command(cin);
                if (!executed && msg_id != -1)
                {
                    char *endl = strchr(cin, '\n');
                    if (endl)
                    {
                        *endl = '\0';
                    }
                    strcpy(msg.msg, cin);
                    msg.mtype = L_MSG;
                    msg.sender = own_id;
                    send_msg_to(&msg, msg_queue);
                }
            }
        }
    }
}

void ask_server_for_init()
{
    msg.sender = own_id;
    msg.mtype = L_INIT;
    msg.conn.pid = getpid();

    send_msg_to(&msg, server_queue);
}

void ask_server_for_connect(char other_id)
{
    msg.mtype = L_CONNECT;
    msg.sender = own_id;
    msg.conn.id = other_id;
    send_msg_to(&msg, server_queue);
}

void ask_server_for_disconnect()
{
    msg.mtype = L_DISCONNECT;
    msg.sender = own_id;
    send_msg_to(&msg, server_queue);
}
void ask_server_for_stop()
{
    msg.mtype = L_STOP;
    msg.sender = own_id;
    send_msg_to(&msg, server_queue);
}

void ask_server_for_list()
{
    msg.mtype = L_LIST;
    msg.sender = own_id;
    send_msg_to(&msg, server_queue);
}

void server_down()
{
    printf("Server down...\n");
    server_queue = -1;
    exit(0);
}

void destructor(void)
{
    if (server_queue != -1)
    {
        ask_server_for_stop();
    }
    remove_queue(own_queue, getpid());
    printf("Client down\n");
}

void sigc(int sig_no)
{
    printf("trl + C\n");
    exit(0);
}
