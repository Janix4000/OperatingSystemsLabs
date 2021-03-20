#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "lib_uni_file.h"

bool isSq(int n)
{
    for (int a = 1; a * a <= n; a++)
    {
        if (a * a == n)
        {
            return true;
        }
    }

    return false;
}

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

    int nEven = 0;
    LibUniFile aFile, bFile, cFile;
    libOpen("a.txt", &aFile, LIB_C);
    libOpen("b.txt", &bFile, LIB_C);
    libOpen("c.txt", &cFile, LIB_C);

    const size_t buffSize = 256;
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
            if (n % 2 == 0)
            {
                nEven++;
            }
            if ((n / 10) % 10 == 0 || (n / 10) % 10 == 7)
            {
                libWrite(num, sizeof *num, strlen(num), &bFile);
                char endline = '\n';
                libWrite(&endline, 1, 1, &bFile);
            }
            if (isSq(n))
            {
                libWrite(num, sizeof *num, strlen(num), &cFile);
                char endline = '\n';
                libWrite(&endline, 1, 1, &cFile);
            }
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
    char str[64];
    sprintf(str, "Liczb parzystych jest %d", nEven);
    libWrite(str, sizeof *str, strlen(str), &aFile);
}