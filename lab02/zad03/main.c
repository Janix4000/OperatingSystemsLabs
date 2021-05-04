#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

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

void zad(int argc, char **args, char type)
{
    const char *fFilename = "./dane.txt";

    LibUniFile file;
    if (!libOpen(fFilename, &file, type, "r"))
    {
        if (file.type != LIB_ERR)
            libClose(&file);
        return;
    }

    int nEven = 0;
    LibUniFile aFile, bFile, cFile;
    libOpen("a.txt", &aFile, type, "w");
    libOpen("b.txt", &bFile, type, "w");
    libOpen("c.txt", &cFile, type, "w");

    const size_t buffSize = 32;
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
    libClose(&aFile);
    libClose(&bFile);
    libClose(&cFile);
    libClose(&file);
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