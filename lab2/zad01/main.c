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

int main(int argc, char **argv)
{
    const char *fFilename = "./a";
    const char *sFilename = "./b";

    LibUniFile fFile;
    LibUniFile sFile;
    if (!libOpen(fFilename, &fFile, LIB_C) || !libOpen(sFilename, &sFile, LIB_C))
    {
        if (fFile.type != LIB_ERR)
            libClose(&fFile);
        if (sFile.type != LIB_ERR)
            libClose(&sFile);
        return -1;
    }

    const size_t buffSize = 127;
    char buffs[2][buffSize + 1];
    LibUniFile *files[2] = {&fFile, &sFile};
    buffs[0][buffSize] = '\0';
    buffs[1][buffSize] = '\0';
    int iPrint = 0;
    bool oneDone = false;
    int counts[2];
    char *begin[2] = {NULL, NULL};

    while (true)
    {
        if (begin[iPrint] == NULL)
        {
            counts[iPrint] = libRead(buffs[iPrint], sizeof *buffs[iPrint], buffSize, files[iPrint]);
            if (counts[iPrint] == 0)
            {
                if (oneDone)
                {
                    break;
                }
                else
                {
                    oneDone = true;
                    iPrint = !iPrint;
                    printf("\n");
                }
            }
            else
            {
                begin[iPrint] = buffs[iPrint];
                buffs[iPrint][counts[iPrint]] = '\0';
            }
        }

        char *endLine = strchr(begin[iPrint], '\n');
        if (endLine == NULL)
        {
            printf("%s", begin[iPrint]);
            begin[iPrint] = NULL;
        }
        else
        {
            *endLine = '\0';
            printf("%s\n", begin[iPrint]);
            begin[iPrint] = endLine + 1;
            if (!oneDone)
            {
                iPrint = !iPrint;
            }
        }
    }
    printf("\n");
    return 0;
}