#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char tableHasBeenCreated = 0;

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("No arguments in input.\n");
        return -1;
    }
    argc--;
    argv++;
    printf("kek\n");
    while (argc)
    {
        if (strcmp(*argv, "--create_table") == 0)
        {
        }
        else if (strcmp(*argv, "--merge_files") == 0)
        {
        }
        else if (strcmp(*argv, "--remove_block") == 0)
        {
        }
        else if (strcmp(*argv, "--remove_row") == 0)
        {
        }
        else if (strcmp(*argv, "--start_measure_time") == 0)
        {
        }
        else if (strcmp(*argv, "--stop_measure_time") == 0)
        {
        }
        else if (strcmp(*argv, "") == 0)
        {
        }
        else
        {
            printf("Not known *argv: \"%s\".\n", *argv);
        }
        getopt()
        argc--;
        argv++;
    }
    return 0;
}