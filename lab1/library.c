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

MergedPair *mergePair(char firstFilename[], char secondFilename[])
{
    return NULL;
}