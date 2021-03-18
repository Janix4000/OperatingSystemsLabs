#include "lib_uni_file.h"

int libOpen(const char *fileName, LibUniFile *uniFile, char type)
{
    if (type == LIB_SYS)
    {
        int id = open(fileName, O_RDWR);
        if (id < 0)
        {
            goto couldnt_open;
        }
        uniFile->file.sysid = id;
    }
    else
    {
        FILE *file = fopen(fileName, "r+");
        if (!file)
        {
            goto couldnt_open;
        }
        uniFile->file.cfile = file;
    }
    uniFile->type = type;
    return 1;
couldnt_open:
    uniFile->type = LIB_ERR;
    fprintf(stderr, "Couldnt open file \"%s\".\n", fileName);
    return 0;
}

void libClose(LibUniFile *uniFile)
{
    if (uniFile->type == LIB_C)
    {
        fclose(uniFile->file.cfile);
    }
    else if (uniFile->type == LIB_SYS)
    {
        close(uniFile->file.sysid);
    }
}

size_t libRead(void *buffor, size_t size, size_t count, LibUniFile *file)
{
    if (file->type == LIB_C)
    {
        return fread(buffor, size, count, file->file.cfile);
    }
    else if (file->type == LIB_SYS)
    {
        return read(file->file.sysid, buffor, size * count);
    }
    return 0;
}

size_t libWrite(void *buffor, size_t size, size_t count, LibUniFile *file)
{
    if (file->type == LIB_C)
    {
        return fwrite(buffor, size, count, file->file.cfile);
    }
    else if (file->type == LIB_SYS)
    {
        return write(file->file.sysid, buffor, size * count);
    }
    return 0;
}