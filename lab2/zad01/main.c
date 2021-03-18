#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "lib_uni_file.h"

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