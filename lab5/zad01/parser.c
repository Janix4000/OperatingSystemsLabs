#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "chain.h"

#define N_BLOCKS 32
#define N_COMMS 200

Block blocks[N_BLOCKS];
char defs[N_BLOCKS][64];

PipeNode nodes[N_BLOCKS];
char *comms[N_COMMS];
int n_nodes = 0;
int n_blocks = 0;
int n_comms = 0;

bool is_def(const char *line)
{
    return strchr(line, '=') != NULL;
}

char *clear_line(char *line)
{
    while (*line && *line != '\n' && (*line == ' ' || *line == '\t' || *line == '='))
    {
        line++;
    }
    return line;
}

char *find_lit_end(char *line)
{
    char *it = line;
    while (*it && *it != ' ' && *it != '=' && *it != '|' && *it != '\t' && *it != '\n')
    {
        it++;
    }
    return it;
}

void make_def(char *line)
{
    line = clear_line(line);
    char *it = find_lit_end(line);
    strncpy(defs[n_blocks], line, it - line);
    line = it;
    init_block(&blocks[n_blocks]);
    while (*line && *line != '\n')
    {
        line = clear_line(line);
        int argc = 0;
        while (*line != '\n' && *line != '\0' && *line != '|')
        {
            it = find_lit_end(line);
            comms[n_comms] = strndup(line, it - line);
            n_comms++;
            line = clear_line(it);
            argc++;
        }
        comms[n_comms++] = NULL;
        init_pipe_node(&nodes[n_nodes], argc, comms - argc - 1);
        block_push_pip_node(&blocks[n_blocks], &nodes[n_nodes]);
        n_nodes++;
    }
    n_blocks++;
}

Block *get_block(const char *def)
{
    for (size_t i = 0; i < n_blocks; i++)
    {
        if (strcmp(def, defs[i]) == 0)
        {
            return blocks[i];
        }
    }
    return NULL;
}

void exec_line(char *line)
{
    Block *chain[10];
    int n_chain = 0;
    while (*line && *line != '\n')
    {
        line = clear_line(line);
        int argc = 0;
        while (*line != '\n' && *line != '\0' && *line != '|')
        {
            char *it = find_lit_end(line);
            *it = '\0';

            line = clear_line(it);
        }
        comms[n_comms++] = NULL;
        init_pipe_node(&nodes[n_nodes], argc, comms - argc - 1);
        block_push_pip_node(&blocks[n_blocks], &nodes[n_nodes]);
        n_nodes++;
    }
}

void free_comms()
{
    for (size_t i = 0; i < n_comms; i++)
    {
        free(comms[i]);
    }
}

int interpret_line(char *line)
{
    if (is_def(line))
    {
        make_def(line);
    }
    else
    {
        exec_line(line);
    }
}