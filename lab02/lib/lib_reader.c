#include "lib_reader.h"

int libOpenReader(const char *fileName, LibReader *reader, int bufforSize, char type)
{
    if (libOpen(fileName, &reader->file, type, "r"))
    {
        reader->buffor = (char *)malloc(bufforSize + 1);
        reader->buffor[bufforSize] = '\0';
        reader->bufforSize = bufforSize;
        reader->ptrBuff = NULL;
        reader->hasEnded = false;
        return 1;
    }
    else
    {
        reader->buffor = NULL;
        reader->bufforSize = 0;
        reader->ptrBuff = NULL;
        reader->hasEnded = true;
        return 0;
    }
}

void _libReadFile(LibReader *reader)
{
    reader->realSize = libRead(reader->buffor, sizeof(char), reader->bufforSize, &reader->file);
    if (!reader->realSize)
    {
        reader->hasEnded = true;
        reader->ptrBuff = NULL;
    }
    else
    {
        reader->ptrBuff = reader->buffor;
    }
}

char libGetNextChar(LibReader *reader)
{
    if (!reader->ptrBuff)
    {
        _libReadFile(reader);
    }
    char c = *reader->ptrBuff;
    reader->ptrBuff++;
    if (reader->ptrBuff >= reader->buffor + reader->realSize)
    {
        _libReadFile(reader);
    }
    return c;
}

void libFreeReader(LibReader *reader)
{
    free(reader->buffor);
    reader->buffor = NULL;
    reader->ptrBuff = NULL;
    libClose(&reader->file);
    reader->bufforSize = 0;
}