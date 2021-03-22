#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "lib_uni_file.h"
#include "lib_reader.h"

void get_kmp(int *tab, const char *txt)
{
    int n = strlen(txt);
    tab[0] = -1;
    int k = -1;
    for (size_t i = 0; i < n; i++)
    {
        while (k != -1 && txt[k] != txt[i])
        {
            k = tab[k];
        }
        tab[i + 1] = ++k;
    }
}

int main(int argc, char **args)
{
    if (argc <= 4)
    {
        printf("Expected 4 arguments.\n");
        return -1;
    }
    const char *inputFilename = args[1];
    const char *outputFilename = args[2];
    const char *n1 = args[3];
    const char *n2 = args[4];

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

    int tab[65];
    get_kmp(tab, n1);

    const size_t textSize = 64;
    char text[textSize + 1];
    text[textSize] = '\0';
    int idxText = 0;
    int q = -1;
    size_t pattLen = strlen(n1);
    size_t replaceLen = strlen(n2);
    while (!input.hasEnded)
    {
        ++q;
        char c = libGetNextChar(&input);
        while (q > -1 && c != n1[q])
        {
            q = tab[q];
        }
        if (q + 1 == pattLen)
        {
            int t = idxText - q;
            for (int j = 0; j < replaceLen; ++j, t = (t + 1) % textSize)
            {
                text[t] = n2[j];
                if (t == textSize - 1)
                {
                    libWrite(text, sizeof(char), textSize, &output);
                }
            }
            idxText = t;
            q = -1;
        }
        else
        {
            text[idxText] = c;
            if (idxText == textSize - 1)
            {
                libWrite(text, sizeof(char), textSize, &output);
            }
            idxText = (idxText + 1) % textSize;
        }
    }
    text[idxText] = '\0';
    libWrite(text, sizeof(char), idxText, &output);
    libFreeReader(&input);
    libClose(&output);
}