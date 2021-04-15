#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

#define MAX_ARGC 8
#define READ 0
#define WRITE 1

typedef struct PipeNode
{
    char *args[MAX_ARGC + 2];
    int argc;

    struct PipeNode *next;
} PipeNode;

int init_pipe_node(PipeNode *node, int argc, char **argv)
{
    if (argc - 1 > MAX_ARGC)
    {
        return -1;
    }
    memcpy(node->args, argv, sizeof(char *) * argc);
    node->args[argc] = NULL;
    node->argc = argc;
    node->next = NULL;
    return 0;
}

int link_nodes(PipeNode *prev, PipeNode *next)
{
    prev->next = next;
    return 0;
}

void swap(int in[2], int out[2])
{
    int tmp[2];
    tmp[0] = in[0];
    tmp[1] = in[1];
    in[0] = out[0];
    in[1] = out[1];
    out[0] = tmp[0];
    out[1] = tmp[1];
}

void exec_node(PipeNode *node, int in[2], int out[2], bool has_input)
{
    if (has_input)
    {
        dup2(in[READ], STDIN_FILENO);
        close(in[READ]);
        close(in[WRITE]);
    }
    if (node->next)
    {
        dup2(out[WRITE], STDOUT_FILENO);
        close(out[READ]);
        close(out[WRITE]);
    }

    execvp(node->args[0], node->args);
    perror("Cannot exec");
    exit(-1);
}

void exec_chain(PipeNode *head)
{
    PipeNode *node = head;

    int in[2] = {};
    int out[2] = {};

    int progs = 0;
    do
    {
        progs++;
        if (progs > 2)
        {
            close(out[READ]);
            close(out[WRITE]);
        }
        if (pipe(out) == -1)
        {
            perror("Cannot pipe out.");
            exit(-1);
        }
        if (vfork() == 0)
        {
            exec_node(node, in, out, node != head);
            exit(0);
        }
        swap(in, out);
        node = node->next;
    } while (node);
}

typedef struct Block
{
    PipeNode *head;
    PipeNode *last;
} Block;

void init_block(Block *block)
{
    block->head = NULL;
    block->last = NULL;
}

void block_push_pip_node(Block *block, PipeNode *node)
{
    if (!block->head)
    {
        block->head = node;
        block->last = node;
    }
    else
    {
        link_nodes(block->last, node);
        block->last = node;
    }
}

void link_blocks(Block **blocks, int n)
{
    for (size_t i = 0; i < n - 1; i++)
    {
        Block *prev = blocks[i];
        Block *next = blocks[i + 1];

        link_nodes(prev->last, next->head);
    }
}

void unlink_blocks(Block **blocks, int n)
{
}

int main(int argc, char **argv)
{
    // cat /etc/passwd | wc -l | grep '31'
    PipeNode n1, n2, n3, n4;
    char *a1[] = {"cat", "/etc/passwd"};
    init_pipe_node(&n1, 2, a1);
    char *a2[] = {"wc", "-l"};
    init_pipe_node(&n2, 2, a2);
    char *a3[] = {"grep", "aaaa"};
    init_pipe_node(&n3, 2, a3);
    char *a4[] = {"grep", "3"};
    init_pipe_node(&n4, 2, a4);

    link_nodes(&n1, &n2);
    // link_nodes(&n2, &n3);
    // link_nodes(&n3, &n4);

    exec_chain(&n1);
    return 0;
}