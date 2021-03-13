#include "library.h"

int test_function()
{
    return 1;
}

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

MergedPair *makeMergedPair(int allLines)
{
    MergedPair *pair = malloc(sizeof *pair);
    pair->nrows = allLines;
    pair->lines = calloc(allLines, sizeof *pair->lines);
    return pair;
}

MergedPair *mergePair(const char firstFilename[], const char secondFilename[])
{
    FILE *firstFile = fopen(firstFilename, 'r');
    FILE *secFile = fopen(secondFilename, 'r');
    if (!(firstFile && secFile))
    {
        return NULL;
    }
    int firstLines = getNumberOfLines(firstFile);
    int secLines = getNumberOfLines(secFile);
    MergedPair *merged = makeMergedPair(firstLines + secLines);
    int comLines = firstLines < secLines ? firstLines : secLines;
    for (size_t i = 0; i < comLines; i++)
    {
        getline(merged->lines[2 * i], 64, firstFile);
        getline(merged->lines[2 * i + 1], 64, secFile);
    }
    int diff = abs(firstLines - secLines);
    if (diff > 0)
    {
        FILE *biggerFile = firstLines > secLines ? firstFile : secFile;
        for (size_t i = 0; i < diff; i++)
        {
            getline(merged->lines[2 * comLines + i], 64, biggerFile);
        }
    }
    return merged;
}