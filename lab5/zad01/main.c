#include "chain.h"

int main(int argc, char **argv)
{
    // cat /etc/passwd | wc -l | grep '31'
    PipeNode n1, n2, n3, n4, n5;
    char *a1[] = {"cat", "/etc/passwd"};
    init_pipe_node(&n1, 2, a1);
    char *a2[] = {"wc", "-l"};
    init_pipe_node(&n2, 2, a2);
    char *a3[] = {"grep", "31"};
    init_pipe_node(&n3, 2, a3);
    char *a4[] = {"ls"};
    init_pipe_node(&n4, 1, a4);
    char *a5[] = {"grep", "\'^a\'"};
    init_pipe_node(&n5, 2, a5);

    link_nodes(&n1, &n2);
    link_nodes(&n2, &n3);
    // link_nodes(&n3, &n4);

    exec_chain(&n1);
    return 0;
}