#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "lib_uni_file.h"
#include "lib_reader.h"

#include <sys/times.h>
struct tms tmsBeginTime;
clock_t realBeginTime;

char verbose = 0;
char isMeasureRunning = 0;

void startTimeMeasurement(int *argc, char ***argv)
{
    if (isMeasureRunning)
    {
        fprintf(stderr, "Time measurement has already started!\n");
    }
    else
    {
        if (verbose)
            printf("Time measurement started.\n");
        isMeasureRunning = 1;
        realBeginTime = times(&tmsBeginTime);
    }
}
void stopTimeMeasurement(int *argc, char ***argv, FILE *res)
{
    if (!isMeasureRunning)
    {
        fprintf(stderr, "Time measurement has not started!\n");
    }
    else
    {
        struct tms tmsEndTime;
        clock_t realEndTime;
        realEndTime = times(&tmsEndTime);

        clock_t dtReal = realEndTime - realBeginTime;
        clock_t dtUser = tmsEndTime.tms_utime - tmsBeginTime.tms_utime;
        clock_t dtSys = tmsEndTime.tms_stime - tmsBeginTime.tms_stime;
        if (verbose)
            printf("Time measurement stopped.\n");
        if (verbose)
            fprintf(res, "Real : User : System\n");
        fprintf(res, "%ld %ld %ld\n", dtReal, dtUser, dtSys);
        isMeasureRunning = 0;
    }
}

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

void zad(int argc, char **args, char type)
{
    if (argc <= 4)
    {
        printf("Expected 4 arguments.\n");
        return;
    }
    const char *inputFilename = args[1];
    const char *outputFilename = args[2];
    const char *n1 = args[3];
    const char *n2 = args[4];

    LibReader input;
    LibUniFile output;
    if (!libOpenReader(inputFilename, &input, 1024, type))
    {
        libFreeReader(&input);
        return;
    }
    if (!libOpen(outputFilename, &output, type, "w"))
    {
        if (output.type != LIB_ERR)
        {
            libClose(&output);
            libFreeReader(&input);
        }
        return;
    }

    int *tab = calloc(sizeof *tab, strlen(n1) + 2);
    get_kmp(tab, n1);

    const size_t textSize = 16;
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
                int t = q + 1;
                libWrite(text, sizeof(char), textSize - t, &output);
                strcpy(text, text + textSize - t);
                idxText = t;
            }
            else
            {
                idxText = (idxText + 1) % textSize;
            }
        }
    }
    text[idxText] = '\0';
    libWrite(text, sizeof(char), idxText, &output);
    libFreeReader(&input);
    libClose(&output);
    free(tab);
}

int main(int argc, char **args)
{
    FILE *res = fopen("res_sys", "w");
    char type = LIB_SYS;
    startTimeMeasurement(&argc, &args);
    zad(argc, args, type);
    stopTimeMeasurement(&argc, &args, res);
    fclose(res);
}