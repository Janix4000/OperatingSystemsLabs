#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "lib_uni_file.h"

int main(int argc, char **args)
{
    const char *fFilename = "./a";
    const char c = 'b';

    LibUniFile file;
    if (!libOpen(fFilename, &file, LIB_C))
    {
        if (file.type != LIB_ERR)
            libClose(&file);
        return -1;
    }

    const size_t buffSize = 8;
    char buff[buffSize + 1];
    char word[buffSize + 1];
    word[buffSize] = '\0';
    buff[buffSize] = '\0';
    char *buffBeg = NULL;
    char *wordEnd = word;
    int count;
    bool shouldPrint = false;
    while (true)
    {
        if (shouldPrint)
        {
            printf("%s\n", word);
            shouldPrint = false;
            wordEnd = word;
        }
        if (buffBeg == NULL)
        {
            count = libRead(buff, sizeof *buff, buffSize, &file);
            if (!count)
            {
                if (word != wordEnd)
                {
                    shouldPrint = true;
                    continue;
                }
                break;
            }
            else
            {
                buff[count] = '\0';
                buffBeg = buff;
            }
        }

        char *endline = strchr(buffBeg, '\n');
        if (!endline)
        {
            strcpy(wordEnd, buffBeg);
            wordEnd += buff + count - buffBeg;
            buffBeg = NULL;
        }
        else
        {
            int len = endline - buffBeg;
            strncpy(wordEnd, buffBeg, len);
            wordEnd[len] = '\0';
            buffBeg += len + 1;
            shouldPrint = strchr(word, c) != NULL;
        }
    }
}