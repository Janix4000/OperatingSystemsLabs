#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "lib_uni_file.h"
#include "lib_reader.h"

int main(int argc, char **args)
{
    if (argc <= 2)
    {
        printf("Expected 2 arguments.\n");
        return -1;
    }
    const char *inputFilename = args[1];
    const char *outputFilename = args[2];

    LibReader input;
    LibUniFile output;
    if (!libOpenReader(inputFilename, &input, 8, LIB_C))
    {
        libFreeReader(&input);
        return -1;
    }
    if (!libOpen(outputFilename, &output, LIB_C))
    {
        if (output.type != LIB_ERR)
        {
            libClose(&output);
            libFreeReader(&input);
        }
        return -1;
    }

    const size_t textSize = 10;
    char text[textSize + 1];
    text[textSize] = '\n';
    int idxText = 0;
    while (!input.hasEnded)
    {
        char c = libGetNextChar(&input);
        text[idxText] = c;
        if (c == '\n' || idxText == textSize - 1)
        {
            int len = idxText + 2;
            if (c == '\n')
            {
                len = idxText + 1;
            }
            text[idxText + 1] = '\n';
            libWrite(text, sizeof(char), len, &output);
            idxText = 0;
        }
        else
        {
            idxText++;
        }
    }
    libWrite(text, sizeof(char), idxText, &output);
    libFreeReader(&input);
    libClose(&output);
}