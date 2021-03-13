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

int getNumberOfLines(FILE *file)
{
    int lines = 0;
    char ch;
    rewind(file);
    while (!feof(file))
    {
        ch = fgetc(file);
        if (ch == '\n')
        {
            lines++;
        }
    }
    if (ch != '\n')
    {
        lines++;
    }
    rewind(file);
    return lines;
}

MergedPair *mergePair(char firstFilename[], char secondFilename[])
{
}
