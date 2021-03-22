#pragma once

#include <stdbool.h>

#include "lib_uni_file.h"

typedef struct
{
    LibUniFile file;
    char *buffor;
    size_t bufforSize;
    char *ptrBuff;
    bool hasEnded;
    size_t realSize;
} LibReader;

int libOpenReader(const char *fileName, LibReader *reader, int bufforSize, char type);

char libGetNextChar(LibReader *reader);

void libFreeReader(LibReader *reader);