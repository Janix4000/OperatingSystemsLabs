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
char id = -1;
char msg_id = -1;

void ask_server_for_list();

void destructor(void);
void sigc(int sig_no);

void ask_server_for_init();
void ask_server_for_connect(int other_id);
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
        id = msg->mtext[0];
        printf("My new id: %d\n", id);
    }
    break;
    case L_STOP:
        server_down();
        break;
    case L_LIST:
    {
        char *list = msg->mtext;
        printf("List:\n");
        for (char *it = list; *it; ++it)
        {
            if (*it == id)
            {
                continue;
            }
            printf("%d\n", *it);
        }
    }
    // list_msg(msg);
    break;
    case L_CONNECT:
    {
        strncpy((char *)&msg_queue, msg->mtext, sizeof(int));
        msg_id = msg->mtext[sizeof(int)];
        printf("Polaczono z %d\n", msg_id);
    }
    break;
    case L_DISCONNECT:
    {
        msg_queue = -1;
    }
    break;
    case L_MSG:
    {
        printf("[%d]: \"%s\"\n", msg_id, msg->mtext);
    }
    break;
    default:
        break;
    }
}

void init(int argc, char const *argv[])
{
    if (argc != 1 + 1)
    {
        perror("No args");
        exit(-1);
    }

    FILE *handle = popen("tail -f /als/als_test.txt", "r");
    int fd = fileno(handle);
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);

    own_queue = create_queue(argv[1]);
    apply_destructor(destructor, sigc);

    while ((server_queue = open_queue("./id")) == -1)
    {
        fprintf(stderr, "Waiting for server..\n");
        sleep(1);
    }

    ask_server_for_init();
}

void main_loop();

int main(int argc, char const *argv[])
{
    init(argc, argv);

    main_loop();

    return 0;
}

// LIST
// CONNECT ID
// DISCONNECT
// EXIT

void execute_command(char *cin)
{
    char comm[60];
    int id = -1;
    bool succes = sscanf(cin, "%s %d", comm, &id);
    for (char *p = comm; *p; ++p)
        *p = tolower(*p);
    if (strcmp(comm, "list") == 0)
    {
        ask_server_for_list();
    }
    else if (strcmp(comm, "connect") == 0)
    {
        if (id == -1)
        {
            printf("Bad id given\n");
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
        printf("Uknown command: %s\n", comm);
    }
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
                if (msg_id != -1)
                {
                    char *endl = strchr(cin, '\n');
                    if (endl)
                    {
                        *endl = '\0';
                    }
                    strcpy(msg.mtext, cin);
                    msg.mtype = L_MSG;
                    send_msg_to(&msg, msg_queue);
                }
                else
                {
                    execute_command(cin);
                }
            }
        }
    }
}

void ask_server_for_init()
{
    msg.mtype = L_INIT;

    strncpy(msg.mtext, (char *)&own_queue, sizeof(int));
    msg.mtype = L_INIT;

    send_msg_to(&msg, server_queue);

    wait_for_msg_from(&msg, own_queue);
    interpret_msg(&msg);
}

void ask_server_for_connect(int other_id)
{
    msg.mtype = L_CONNECT;
    msg.mtext[0] = id;
    msg.mtext[1] = other_id;
    send_msg_to(&msg, server_queue);
}
void wait_for_connect()
{
    printf("Czekanie na nowego rozmowce...\n");
    wait_for_msg_from(&msg, own_queue);
    interpret_msg(&msg);
}

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
    close_queue(own_queue);
    printf("Client down\n");
}

void sigc(int sig_no)
{
    printf("Ctrl + C\n");
    exit(0);
}
