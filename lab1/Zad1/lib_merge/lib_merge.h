#pragma once

#include <stdlib.h>
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
void libRemoveLineInAt(LibLinesBlocks *blocks, size_t blockIdx, size_t lineIdx);
void libPrintLinesBlocks(LibLinesBlocks *blocks);
void libFreeLinesBlocks(LibLinesBlocks *blocks);

void libAddFilenamePair(LibFilenamePairs *pairs, const char *filenamePair);
void libMergeFilePairs(LibFiles *tmpFiles, LibFilenamePairs *filenamePairs);
void libFreeFilePairs(LibFilenamePairs *filenamePairs);

void libFreeFiles(LibFiles *tmpFiles);