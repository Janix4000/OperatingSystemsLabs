#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "lib_uni_file.h"

int main(int argc, char **args)
{
    const char *fFilename = "./dane.txt";

    LibUniFile file;
    if (!libOpen(fFilename, &file, LIB_C))
    {
        if (file.type != LIB_ERR)
            libClose(&file);
        return -1;
    }

    const size_t buffSize = 4;
    char buff[buffSize + 1];
    buff[buffSize] = '\0';
    char *itBuff = NULL;
    char num[10];
    char *itNum = num;
    bool loop = true;
    int realSize;
    while (loop)
    {
        if (!itBuff)
        {
            realSize = libRead(buff, sizeof *buff, buffSize, &file);
            if (!realSize)
            {
                itBuff = NULL;
                loop = false;
            }
            else
            {
                itBuff = buff;
            }
        }
        while (itBuff && itBuff < buff + realSize && *itBuff != ' ')
        {
            *itNum = *itBuff;
            ++itNum;
            ++itBuff;
        }
        if (itBuff && itBuff < buff + buffSize)
        {
            *itNum = '\0';
            itNum = num;
            int n = atoi(num);
            printf("%d\n", n);
        }
        if (itBuff)
        {
            while (itBuff && *itBuff == ' ')
            {
                itBuff++;
            }
            if (itBuff >= buff + realSize)
            {
                itBuff = NULL;
            }
        }
    }
}