#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void forkProcesses(int n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (fork() == 0)
        {
            printf("Hello from %ld forked proc!\n", i);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        int n = atoi(argv[1]);
        if (n == 0)
        {
            fprintf(stderr, "Expected positive argument as  number of children processes.\n");
            return -1;
        }
        forkProcesses(n);
    }
    else
    {
        fprintf(stderr, "Expected one parameter, given %d.\n", argc - 1);
        return -1;
    }
    return 0;
}