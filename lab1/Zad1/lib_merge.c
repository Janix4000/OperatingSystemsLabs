#include "lib_merge.h"
#include <string.h>
#include <stdio.h>

int test_function()
{
    return 1; //
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
    free(line);
    vecPushBack(blocks, lines);
    return 1;
}

static void _libFreeLines(LibLines *lines)
{
    while (lines->size)
    {
        free(vecPopBack(lines));
    }
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

void libPrintLinesBlocks(LibLinesBlocks *blocks)
{
    for (size_t iBlock = 0; iBlock < blocks->size; iBlock++)
    {
        LibLines *lines = blocks->container[iBlock];
        for (size_t iLine = 0; iLine < lines->size; iLine++)
        {
            printf("%s\n", (char *)lines->container[iLine]);
        }
        printf("\n");
    }
}

void libFreeRowBlocks(LibLinesBlocks *blocks)
{
    while (blocks->size)
    {
        _libFreeLines(vecPopBack(blocks));
    }
    free(blocks);
}

void libAddFilenamePair(LibFilenamePairs *pairs, const char *filenamePair)
{
    FilenamePair *pair = malloc(sizeof *pair);
    char *colonPos = strchr(filenamePair, ':');
    pair->firstFilename = strndup(filenamePair, colonPos - filenamePair);
    pair->secondFilename = strdup(colonPos + 1);
    vecPushBack(pairs, pair);
}

static void _libMergePairOfFilesToTmp(FILE *firstFile, FILE *secondFile, FILE *tmp)
{
    char *fLine = NULL, *sLine = NULL;
    size_t nFirst, nSecond;
    while (
        getline(&fLine, &nFirst, firstFile) != 1 &&
        getline(&sLine, &nSecond, secondFile) != 1)
    {
        fwrite(fLine, sizeof *fLine, nFirst, tmp);
        fwrite(sLine, sizeof *sLine, nSecond, tmp);
    }
    rewind(tmp);
    free(fLine);
    free(sLine);
}

void libMergeFilePairs(LibFiles *tmpFiles, LibFilenamePairs *filenamePairs)
{
    for (size_t iPair = 0; iPair < filenamePairs->size; iPair++)
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
        }
        fclose(firstFile);
        fclose(secondFile);
    }
}

void libFreeFilePairs(LibFilenamePairs *filenamePairs)
{
    while (filenamePairs->size)
    {
        FilenamePair *pair = vecPopBack(filenamePairs);
        free(pair->firstFilename);
        free(pair->secondFilename);
        free(pair);
    }
    free(filenamePairs);
}

void libFreeFiles(LibFiles *tmpFiles)
{
    while (tmpFiles->size)
    {
        FILE *tmp = vecPopBack(tmpFiles);
        fclose(tmp);
    }
    free(tmpFiles);
}