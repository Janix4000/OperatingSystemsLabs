#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "lib_uni_file.h"

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

void zad(int argc, char **argv, char type)
{
    if (argc < 3)
    {
        printf("Expected 2 arguments.\n");
        return;
    }
    const char c = argv[1][0];
    const char *fFilename = argv[2];

    LibUniFile file;
    if (!libOpen(fFilename, &file, type, "r"))
    {
        if (file.type != LIB_ERR)
            libClose(&file);
        return;
    }

    const size_t buffSize = 32;
    char buff[buffSize + 1];
    char word[256 + 1];
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
            wordEnd += count;
            buffBeg = NULL;
        }
        else
        {
            int len = endline - buffBeg;
            strncpy(wordEnd, buffBeg, len);
            wordEnd[len] = '\0';
            buffBeg += len + 1;
            shouldPrint = strchr(word, c) != NULL;
            if (!shouldPrint)
            {
                wordEnd = word;
            }
        }
    }
}

int main(int argc, char **argv)
{
    FILE *res = fopen("res_c", "w");
    char type = LIB_C;
    startTimeMeasurement(&argc, &argv);
    zad(argc, argv, type);
    stopTimeMeasurement(&argc, &argv, res);
    fclose(res);
}