#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "lib_vector.h"

typedef struct
{
    char *firstFilename;
    char *secondFilename;
} FilenamePair;

typedef LibVector LibFiles;
typedef LibVector LibFilenamePairs;
typedef LibVector LibLines;
typedef LibVector LibLinesBlocks;

int test_function();

size_t libReadLineBlock(LibLinesBlocks *blocks, FILE *mergedLinesFile);
void libRemoveLineBlockAt(LibLinesBlocks *blocks, size_t idx);
void _libFreeLines(LibLines *lines) void removeLineInAt(LibLinesBlocks *blocks, size_t blockIdx, size_t lineIdx);
void printLinesBlocks(LibLinesBlocks *blocks);
void freeRowBlocks(LibLinesBlocks *blocks);

void addFilenamePair(LibFilenamePairs *pairs, const char *firstFilename, const char *secondFilename);
void mergeFilePairs(LibFiles *tmpFiles, LibFilenamePairs *filenamePairs);
void freeFilePairs(LibFilenamePairs *filenamePairs);

void freeFiles(LibFiles *tmpFiles);
