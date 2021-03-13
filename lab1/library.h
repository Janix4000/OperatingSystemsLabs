#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct MergedPair
{
    int nrows;
    char **lines;
} MergedPair;

typedef struct MergedFiles
{
    int nPairs;
    MergedPair *mergedPairs;
} MergedFiles;

int test_function();

int getNumberOfLines(FILE *file);

MergedPair *mergePair(char firstFilename[], char secondFilename[]);
