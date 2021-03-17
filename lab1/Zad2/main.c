#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#include "lib_merge.h"

void createTable(int *argc, char ***argv);
void mergeFiles(int *argc, char ***argv);
void removeBlock(int *argc, char ***argv);
void removeRow(int *argc, char ***argv);
void startTimeMeasurement(int *argc, char ***argv);
void stopTimeMeasurement(int *argc, char ***argv);

char tableHasBeenCreated = 0;
LibLinesBlocks blocks;
struct tms tmsBeginTime;
clock_t realBeginTime;
char isMeasureRunning = 0;

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
            removeRow(&argc, &argv);
        }
        else if (strcmp(comm, "--start_measure_time") == 0)
        {
            startTimeMeasurement(&argc, &argv);
        }
        else if (strcmp(comm, "--stop_measure_time") == 0)
        {
            stopTimeMeasurement(argc, argv);
        }
        else
        {
            printf("Not known command: \"%s\".\n", comm);
        }
    }
    libFreeLinesBlocks(&blocks);
    return 0;
}

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

    printf("Creating table...\n");
    vecInit(&blocks);
    vecReserve(&blocks, nMaxPairs);
    tableHasBeenCreated = 1;
    printf("Table created.\n");
}
void mergeFiles(int *argc, char ***argv)
{
    int nToMerge = 0;
    char inputIsValid = 1;

    LibFilenamePairs filenamePairs;
    vecInit(&filenamePairs);

    while (*argc > 0 && ***argv != '-')
    {
        char *filenamePair = **argv;
        char *colonPos = strchr(filenamePair, ':');
        if (colonPos == NULL || strchr(colonPos + 1, ':') != NULL)
        {
            fprintf(stderr, "--merge_files input format is: \"filename_1:filename_2\", given: \"%s\".\n", filenamePair);
            inputIsValid = 0;
            goto next_argument;
        }

        // merge
        printf("Adding %s pair to list...\n", filenamePair);
        &filenamePairs;
        libAddFilenamePair(&filenamePairs, filenamePair);
        printf("Added %s pair to list.\n", filenamePair);

        ++nToMerge;
    next_argument:
        ++*argv;
        --*argc;
    }
    if (nToMerge == 0)
    {
        fprintf(stderr, "--merge_table needs at least one valid pair of files.\n");
    }
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been created!\n");
        return;
    }
    if (inputIsValid)
    {
        printf("Merging...\n");
        LibFiles tmpFiles;
        vecInit(&tmpFiles);

        libMergeFilePairs(&tmpFiles, &filenamePairs);

        libReadBlocksFromFiles(&blocks, &tmpFiles);

        libFreeFiles(&tmpFiles);
        printf("Merged.\n");
    }
    libFreeFilePairs(&filenamePairs);
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
    if (blockIdx >= blocks.size)
    {
        fprintf(stderr, "Block idx out of range.\n");
        return;
    }

    // remove block
    printf("Removed block [%d].\n", blockIdx);
    libRemoveLineBlockAt(&blocks, blockIdx);
}
void removeRow(int *argc, char ***argv)
{
    if (*argc < 2 || ***argv == '-')
    {
        fprintf(stderr, "--remove_row needs two arguments.\n");
        return;
    }
    int blockIdx;
    if (sscanf(**argv, "%d", &blockIdx) != 1)
    {
        fprintf(stderr, "--remove_row needs non-negative block index argument, given: \"%s\".\n", **argv);
        ++*argv;
        --*argc;
        return;
    }
    ++*argv;
    --*argc;

    int rowIdx;
    if (sscanf(**argv, "%d", &rowIdx) != 1)
    {
        fprintf(stderr, "--remove_row needs non-negative row index argument, given: \"%s\".\n", **argv);
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
    if (blockIdx >= blocks.size)
    {
        fprintf(stderr, "Block idx out of range.\n");
        return;
    }
    if (rowIdx >= ((LibLines *)blocks.container[blockIdx])->size)
    {
        fprintf(stderr, "Row idx out of range.\n");
        return;
    }
    printf("Removed row [%d][%d].\n", blockIdx, rowIdx);
    libRemoveLineInAt(&blocks, blockIdx, rowIdx);
}
void startTimeMeasurement(int *argc, char ***argv)
{
    if (isMeasureRunning)
    {
        printf("Time measurement has already started!\n");
    }
    else
    {
        printf("Time measurement started.\n");
        isMeasureRunning = 1;
        realBeginTime = times(&tmsBeginTime);
    }
}
void stopTimeMeasurement(int *argc, char ***argv)
{
    if (!isMeasureRunning)
    {
        printf("Time measurement has not started!\n");
    }
    else
    {
        struct tms tmsEndTime;
        clock_t realEndTime;
        realEndTime = times(&tmsEndTime);

        clock_t dtReal = realEndTime - realBeginTime;
        clock_t dtUser = tmsEndTime.tms_utime - tmsBeginTime.tms_utime;
        clock_t dtSys = tmsEndTime.tms_stime - tmsBeginTime.tms_stime;
        printf("Time measurement stopped.\n");
        printf("Real : User : System\n");
        printf("%ld %ld %ld\n", dtReal, dtUser, dtSys);
        isMeasureRunning = 0;
        printf("Time measurement stopped.\n");
    }
}
