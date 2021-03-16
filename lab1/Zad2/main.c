#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char tableHasBeenCreated = 0;

void createTable(int *argc, char ***argv)
{
    if (!*argc || ***argv == '-')
    {
        fprintf(stderr, "--create_table needs one argument.\n");
        return;
    }
    int nMaxPairs = atoi(**argv);
    if (nMaxPairs == 0)
    {
        fprintf(stderr, "--create_table needs one positive integer argument, given: \"%s\".\n", **argv);
        ++*argv;
        --*argc;
        return;
    }
    ++*argv;
    --*argc;
    if (tableHasBeenCreated)
    {
        fprintf(stderr, "Table has been already initialized.\n");
        return;
    }
    tableHasBeenCreated = 1;
}

void mergeFiles(int *argc, char ***argv)
{
    int nToMerge = 0;
    char inputIsValid = 1;
    while (*argc > 0 && ***argv != '-')
    {
        char *filenamePair = **argv;
        char *colonPos = strchr(filenamePair, ':');
        if (colonPos == NULL || strchr(colonPos + 1, ':') != NULL)
        {
            fprintf(stderr, "merge_files input format is: \"filename_1:filename_2\", given: \"%s\".\n", filenamePair);
            inputIsValid = 0;
            goto next_argument;
        }

        // merge
        printf("Adding %s pair to list.\n", filenamePair);

        ++nToMerge;
    next_argument:
        ++*argv;
        --*argc;
    }
    if (nToMerge == 0)
    {
        fprintf(stderr, "--merge_table needs at least one valid pair of files.\n");
    }
    if (inputIsValid)
    {
        printf("Merging...\n");
    }
}

void removeBlock(int *argc, char ***argv)
{
    if (!*argc || ***argv == '-')
    {
        fprintf(stderr, "--remove_block needs one argument.\n");
        return;
    }
    int blockIdx = atoi(**argv);
    if (sscanf(**argv, "%d", &blockIdx) != 1)
    {
        fprintf(stderr, "--remove_block needs one non-negative index argument, given: \"%s\".\n", **argv);
        ++*argv;
        --*argc;
        return;
    }
    ++*argv;
    --*argc;
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been initialized.\n");
        return;
    }

    // remove block
    printf("Removed block [%d].\n", blockIdx);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("No arguments in input.\n");
        return -1;
    }
    argc--;
    argv++;
    while (argc)
    {
        const char *comm = *argv;
        argc--;
        argv++;
        if (strcmp(comm, "--create_table") == 0)
        {
            createTable(&argc, &argv);
        }
        else if (strcmp(comm, "--merge_files") == 0)
        {
            mergeFiles(&argc, &argv);
        }
        else if (strcmp(comm, "--remove_block") == 0)
        {
            removeBlock(&argc, &argv);
        }
        else if (strcmp(comm, "--remove_row") == 0)
        {
        }
        else if (strcmp(comm, "--start_measure_time") == 0)
        {
        }
        else if (strcmp(comm, "--stop_measure_time") == 0)
        {
        }
        else if (strcmp(comm, "") == 0)
        {
        }
        else
        {
            printf("Not known *comm: \"%s\".\n", comm);
        }
    }
    return 0;
}