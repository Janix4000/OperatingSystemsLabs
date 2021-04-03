#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

#ifndef DYNAMIC
#endif
#include "../../lab1/Zad1/lib_merge/lib_merge.h"
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

#define DEF_FPTR(ret, name, ...) ret (*const name##Fptr)(__VA_ARGS__) = name;

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

char verbose = 0;

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "No arguments in input.\n");
        return -1;
    }
    argc--;
    argv++;

    if (strcmp(*argv, "--verbose") == 0)
    {
        verbose = 1;

        argc--;
        argv++;
    }

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
        else if (strcmp(comm, "--start_measure_time") == 0)
        {
        }
        else if (strcmp(comm, "--stop_measure_time") == 0)
        {
        }
        else
        {
            fprintf(stderr, "Not known command: \"%s\".\n", comm);
        }
    }
    libFreeLinesBlocksFptr(&blocks);
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

    if (verbose)
        printf("Creating table...");
    vecInitFptr(&blocks);
    vecReserveFptr(&blocks, nMaxPairs);
    tableHasBeenCreated = 1;
    if (verbose)
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
    if (verbose)
        printf("Table freed.\n");
}
static void _libMergePairOfFilesToTmp(FILE *firstFile, FILE *secondFile, FILE *tmp)
{
    char *fLine = NULL, *sLine = NULL;
    size_t nFirst = 0, nSecond = 0;
    size_t nF = 0, nS = 0;
    while (
        (nF = getline(&fLine, &nFirst, firstFile)) != -1 &&
        (nS = getline(&sLine, &nSecond, secondFile)) != -1)
    {
        fwrite(fLine, sizeof *fLine, nF, tmp);
        fwrite(sLine, sizeof *sLine, nS, tmp);
    }
    free(fLine);
    free(sLine);
    rewind(tmp);
}

void mergeFilePairs(LibFiles *tmpFiles, LibFilenamePairs *filenamePairs)
{
    for (size_t iPair = 0; iPair < filenamePairs->size; iPair++)
    {
        if (fork() == 0)
        {
            FilenamePair *pair = filenamePairs->container[iPair];
            FILE *tmp = tmpfile();
            FILE *firstFile = fopen(pair->firstFilename, "r");
            FILE *secondFile = fopen(pair->secondFilename, "r");
            if (tmp && firstFile && secondFile)
            {
                _libMergePairOfFilesToTmp(firstFile, secondFile, tmp);
                vecPushBack(tmpFiles, tmp);
            }
            else
            {
                fclose(tmp);
                printf("Couldnt open files.\n");
            }
            if (firstFile)
            {
                fclose(firstFile);
            }
            if (secondFile)
            {
                fclose(secondFile);
            }
            exit(0);
        }
    }
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
        // if (verbose) printf("Adding %s pair to list...", filenamePair);
        libAddFilenamePairFptr(&filenamePairs, filenamePair);
        // if (verbose) printf("Added \"%s\".\t", filenamePair);

        ++nToMerge;

    next_argument:
        ++*argv;
        --*argc;
    }
    if (nToMerge == 0)
    {
        fprintf(stderr, "--merge_table needs at least one valid pair of files.\n");
    }
    if (verbose)
        printf("\n");
    if (!tableHasBeenCreated)
    {
        fprintf(stderr, "Table has not been created!\n");
        return;
    }
    if (inputIsValid)
    {
        if (verbose)
            printf("Merging...\n");
        LibFiles tmpFiles;
        vecInitFptr(&tmpFiles);
        libMergeFilePairsFptr(&tmpFiles, &filenamePairs);

        libReadBlocksFromFilesFptr(&blocks, &tmpFiles);
        libFreeFilesFptr(&tmpFiles);

        if (verbose)
            printf("Merged %d file pairs.\n", nToMerge);
    }

    libFreeFilePairsFptr(&filenamePairs);
}

void startTimeMeasurement(int *argc, char ***argv)
{
    if (isMeasureRunning)
    {
        fprintf(stderr, "Time measurement has already started!\n");
    }
    else
    {
        if (verbose)
            printf("Time measurement started.\n");
        isMeasureRunning = 1;
        realBeginTime = times(&tmsBeginTime);
    }
}
void stopTimeMeasurement(int *argc, char ***argv)
{
    if (!isMeasureRunning)
    {
        fprintf(stderr, "Time measurement has not started!\n");
    }
    else
    {
        struct tms tmsEndTime;
        clock_t realEndTime;
        realEndTime = times(&tmsEndTime);

        clock_t dtReal = realEndTime - realBeginTime;
        clock_t dtUser = tmsEndTime.tms_utime - tmsBeginTime.tms_utime;
        clock_t dtSys = tmsEndTime.tms_stime - tmsBeginTime.tms_stime;
        if (verbose)
            printf("Time measurement stopped.\n");
        if (verbose)
            printf("Real : User : System\n");
        printf("%ld %ld %ld\n", dtReal, dtUser, dtSys);
        isMeasureRunning = 0;
    }
}