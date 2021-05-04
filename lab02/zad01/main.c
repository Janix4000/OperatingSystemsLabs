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
    char fFilename[64];
    char sFilename[64];
    if (argc < 2)
    {
        printf("First file name: ");
        scanf("%s", fFilename);
    }
    else
    {
        strcpy(fFilename, argv[1]);
    }
    if (argc < 3)
    {
        printf("Second file name: ");
        scanf("%s", sFilename);
    }
    else
    {
        strcpy(sFilename, argv[2]);
    }

    LibUniFile fFile;
    LibUniFile sFile;
    if (!libOpen(fFilename, &fFile, LIB_C, "r") || !libOpen(sFilename, &sFile, LIB_C, "r"))
    {
        if (fFile.type != LIB_ERR)
            libClose(&fFile);
        if (sFile.type != LIB_ERR)
            libClose(&sFile);
        return;
    }

    const size_t buffSize = 1024;
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
    libClose(&fFile);
    libClose(&sFile);
}

int main(int argc, char **argv)
{
    FILE *res = fopen("res_c", "w");
    char type = LIB_C;
    startTimeMeasurement(&argc, &argv);
    zad(argc, argv, type);
    stopTimeMeasurement(&argc, &argv, res);
    fclose(res);
    return 0;
}
