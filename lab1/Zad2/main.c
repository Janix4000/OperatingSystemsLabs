#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#ifndef DYNAMIC
#endif
#include "lib_merge.h"

#ifdef DYNAMIC
#include <dlfcn.h>
#endif

#define RRMERGE_FUNCTIONS(PROCESS_DECL)                                                            \
    PROCESS_DECL(size_t, libReadLineBlock, LibLinesBlocks *blocks, FILE *mergedLinesFile)          \
    PROCESS_DECL(void, libReadBlocksFromFiles, LibLinesBlocks *blocks, LibFiles *tmpFiles)         \
    PROCESS_DECL(void, libRemoveLineInAt, LibLinesBlocks *blocks, size_t blockIdx, size_t lineIdx) \
    PROCESS_DECL(void, libRemoveLineBlockAt, LibLinesBlocks *blocks, size_t idx)                   \
    PROCESS_DECL(void, libPrintLinesBlocks, LibLinesBlocks *blocks)                                \
    PROCESS_DECL(void, libFreeLinesBlocks, LibLinesBlocks *blocks)                                 \
                                                                                                   \
    PROCESS_DECL(void, libAddFilenamePair, LibFilenamePairs *pairs, const char *filenamePair)      \
    PROCESS_DECL(void, libMergeFilePairs, LibFiles *tmpFiles, LibFilenamePairs *filenamePairs)     \
    PROCESS_DECL(void, libFreeFilePairs, LibFilenamePairs *filenamePairs)                          \
                                                                                                   \
    PROCESS_DECL(void, libFreeFiles, LibFiles *tmpFiles)                                           \
                                                                                                   \
    PROCESS_DECL(LibVector *, vecInit, LibVector *vector)                                          \
    PROCESS_DECL(void, vecReserve, LibVector *vector, size_t capacity)                             \
    PROCESS_DECL(void, vecClear, LibVector *vector)                                                \
    PROCESS_DECL(void, vecFree, LibVector *vector)

#ifdef DYNAMIC
#define DEF_FPTR(ret, name, ...) ret (*name##Fptr)(__VA_ARGS__);
#else
#define DEF_FPTR(ret, name, ...) ret (*const name##Fptr)(__VA_ARGS__) = name;
#endif

RRMERGE_FUNCTIONS(DEF_FPTR)

#undef DEF_FPTR

void createTable(int *argc, char ***argv);
void freeTable(int *argc, char ***argv);
void mergeFiles(int *argc, char ***argv);
void removeBlock(int *argc, char ***argv);
void removeRow(int *argc, char ***argv);
void startTimeMeasurement(int *argc, char ***argv);
void stopTimeMeasurement(int *argc, char ***argv);
void printBloks(int *argc, char ***argv);

char tableHasBeenCreated = 0;
LibLinesBlocks blocks;
struct tms tmsBeginTime;
clock_t realBeginTime;
char isMeasureRunning = 0;

int main(int argc, char **argv)
{
#ifdef DYNAMIC
    void *dl_handle = dlopen("../Zad1/lib_merge/libmerge.so", RTLD_LAZY);
    if (!dl_handle)
    {
        fprintf(stderr, "Couldnt open dl_handle.\n");
        return -1;
    }
#define LINK_FPTR(ret, name, ...) name##Fptr = dlsym(dl_handle, #name);

    RRMERGE_FUNCTIONS(LINK_FPTR)

#undef LINK_FPTR
#endif

    printf("Nargs: %d.\n", argc);
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
        else if (strcmp(comm, "--free_table") == 0)
        {
            freeTable(&argc, &argv);
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
            stopTimeMeasurement(&argc, &argv);
        }
        else if (strcmp(comm, "--print") == 0)
        {
            printBloks(&argc, &argv);
        }
        else
        {
            printf("Not known command: \"%s\".\n", comm);
        }
    }
    libFreeLinesBlocksFptr(&blocks);

#ifdef DYNAMIC
    dlclose(dl_handle);
#endif

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

    printf("Creating table...");
    vecInitFptr(&blocks);
    vecReserveFptr(&blocks, nMaxPairs);
    tableHasBeenCreated = 1;
    printf(" Table created[%d].\n", nMaxPairs);
}
void freeTable(int *argc, char ***argv)
{
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been initialized yet.\n");
        return;
    }
    tableHasBeenCreated = 0;
    vecFreeFptr(&blocks);
    printf("Table freed.\n");
}

void mergeFiles(int *argc, char ***argv)
{
    int nToMerge = 0;
    char inputIsValid = 1;

    LibFilenamePairs filenamePairs;
    vecInitFptr(&filenamePairs);

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
        // printf("Adding %s pair to list...", filenamePair);
        libAddFilenamePairFptr(&filenamePairs, filenamePair);
        // printf("Added \"%s\".\t", filenamePair);

        ++nToMerge;

    next_argument:
        ++*argv;

        --*argc;
    }
    if (nToMerge == 0)
    {
        fprintf(stderr, "--merge_table needs at least one valid pair of files.\n");
    }
    printf("\n");
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been created!\n");
        return;
    }
    if (inputIsValid)
    {
        printf("Merging...\n");
        LibFiles tmpFiles;
        vecInitFptr(&tmpFiles);
        libMergeFilePairsFptr(&tmpFiles, &filenamePairs);

        libReadBlocksFromFilesFptr(&blocks, &tmpFiles);
        libFreeFilesFptr(&tmpFiles);

        printf("Merged %d file pairs.\n", nToMerge);
    }

    libFreeFilePairsFptr(&filenamePairs);
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
    libRemoveLineBlockAtFptr(&blocks, blockIdx);
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
    libRemoveLineInAtFptr(&blocks, blockIdx, rowIdx);
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
    }
}

void printBloks(int *argc, char ***argv)
{
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been initialized.\n");
        return;
    }
    libPrintLinesBlocksFptr(&blocks);
}