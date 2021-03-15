#include "lib_merge.h"

int test_function()
{
    return 1;
}

size_t libReadLineBlock(LibLinesBlocks *blocks, FILE *mergedLinesFile)
{
    char *line = NULL;
    size_t nLine = 0;
    LibLines *lines = malloc(sizeof *lines);
    vecInit(lines);
    while (getline(&line, &nLine, mergedLinesFile) != -1)
    {
        vecPushBack(lines, line);
        line = NULL;
        nLine = 0;
    }
    vecPushBack(blocks, lines);
    return 1;
}

void _libFreeLines(LibLines *lines)
{
    vecFree(lines);
    free(lines);
}

void libRemoveLineBlockAt(LibLinesBlocks *blocks, size_t idx)
{
    LibLines *lines = vecEraseAt(blocks, idx);
    _libFreeLines(lines);
}

void libRemoveLineInAt(LibLinesBlocks *blocks, size_t blockIdx, size_t lineIdx)
{
    LibLines *lines = blocks->container[blockIdx];
    free(vecEraseAt(lines, lineIdx));
    if (lines->size == 0)
    {
        libRemoveLineBlockAt(blocks, blockIdx);
    }
}

void printLinesBlocks(LibLinesBlocks *blocks)
{
    for (size_t iBlock = 0; iBlock < blocks->size; iBlock++)
    {
        LibLines *lines = blocks->container[iBlock];
        for (size_t iLine = 0; iLine < lines->size; iLine++)
        {
            printf("%s\n", lines->container[iLine]);
        }
        printf("\n");
    }
}