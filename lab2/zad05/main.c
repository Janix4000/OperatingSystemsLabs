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

void zad(int argc, char **args, char type)
{
    if (argc <= 2)
    {
        printf("Expected 2 arguments.\n");
        return;
    }
    const char *inputFilename = args[1];
    const char *outputFilename = args[2];

    LibReader input;
    LibUniFile output;
    if (!libOpenReader(inputFilename, &input, 10000, type))
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

int main(int argc, char **args)
{

    FILE *res = fopen("res_c", "w");
    char type = LIB_C;
    startTimeMeasurement(&argc, &args);
    zad(argc, args, type);
    stopTimeMeasurement(&argc, &args, res);
    fclose(res);
}