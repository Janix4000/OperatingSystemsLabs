#include "ips.h"
typedef struct
{
    pid_t pid;
    char id;
    L_QUEUE queue;
    char conn_id;
} client;