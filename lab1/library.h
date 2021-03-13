#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct MergedPair
{
    int nrows;
    char (*lines)[64];
} MergedPair;

typedef struct MergedFiles
{
    int nPairs;
    MergedPair *mergedPairs;
} MergedFiles;

int test_function();

int getNumberOfLines(FILE *file);

MergedPair *makeMergedPair(int allLines);

MergedPair *mergePair(const char firstFilename[], const char secondFilename[]);
