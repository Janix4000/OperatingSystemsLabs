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
    const char *inputFilename = "./dane.txt";
    const char *outputFilename = "./kek.txt";

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
            int len = idxText + 1;
            if (c == '\n')
            {
                len = idxText;
            }
            // text[idxText + 1] = '\n';
            // libWrite(text, sizeof (char), len, &output);
            text[len] = '\0';
            printf("%s\n", text);
            idxText = 0;
        }
        else
        {
            idxText++;
        }
    }
    // text[idxText] = '\n';
    // libWrite(text, sizeof (char), idxText, &output);
    text[idxText] = '\0';
    printf("%s", text);
    libFreeReader(&input);
    libClose(&output);
}