
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LIB_EMPTY 0
#define LIB_SYS 1
#define LIB_C 2
#define LIB_ERR -1

typedef struct
{
    union
    {
        FILE *cfile;
        int sysid;
    } file;
    char type;
} LibUniFile;

int libOpen(const char *fileName, LibUniFile *uniFile, char type, const char *mode);

void libClose(LibUniFile *uniFile);

size_t libRead(void *buffor, size_t size, size_t count, LibUniFile *file);

size_t libWrite(void *buffor, size_t size, size_t count, LibUniFile *file);