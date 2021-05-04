#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

char verbose = 0;

#define DIR_ID 4
#define FILE_ID 8

bool isTxt(const char *filename)
{
    size_t len = strlen(filename);
    if (len < 5)
    {
        return false;
    }
    if (strcmp(filename + len - 4, ".txt") != 0)
    {
        return false;
    }
    return true;
}

bool isOSFolder(const char *filename)
{
    if (strcmp(filename, ".") == 0 ||
        strcmp(filename, "..") == 0)
    {
        return true;
    }
    return false;
}

bool hasWord(const char *filename, const char *word)
{
    return true;
}

int lurkDirs(const char *beginPath, const char *absPath, int depth, const char *word)
{
    if (depth < 0)
    {
        return 0;
    }
    size_t nBegPath = strlen(beginPath);
    size_t nAbsPath = strlen(absPath);
    size_t nPath = nBegPath + nAbsPath + 1;
    char *path = malloc(sizeof(char) * (nBegPath + nAbsPath + 256));
    strcpy(path, beginPath);
    path[nBegPath] = '/';
    strcpy(path + nBegPath + 1, absPath);

    DIR *directory = opendir(path);

    if (!directory)
    {
        fprintf(stderr, "Cannot open %s folder.\n", absPath);
        return -1;
    }

    struct dirent *nextFile = NULL;
    do
    {
        nextFile = readdir(directory);
        if (nextFile)
        {
            if (isOSFolder(nextFile->d_name))
            {
                continue;
            }
            if (nextFile->d_type == DIR_ID)
            {
                char *newAbsPath = malloc(nAbsPath + strlen(absPath) + 1);
                strcpy(newAbsPath, absPath);
                newAbsPath[nAbsPath] = '/';
                strcpy(newAbsPath + nAbsPath + 1, nextFile->d_name);
                if (fork() == 0)
                {
                    lurkDirs(beginPath, newAbsPath, depth - 1, word);
                    free(newAbsPath);
                    free(path);
                    exit(0);
                }
                free(newAbsPath);
            }
            else if (nextFile->d_type == FILE_ID)
            {
                //smt else
                if (isTxt(nextFile->d_name))
                {
                    strcpy(path + nPath, nextFile->d_name);
                    if (hasWord(path, word))
                    {
                        printf("File \"%s\" from absolute path \"%s\" has word.\nFound by process %u.\n\n",
                               nextFile->d_name, absPath, getpid());
                    }
                }
            }
        }
    } while (nextFile);
    free(path);
    closedir(directory);
    return 0;
}

int main(int argc, char **argv)
{
    argc--;
    argv++;
    if (argc != 3)
    {
        fprintf(stderr, "Expected three arguments.\n");
        return -1;
    }

    const char *absPath = argv[0];
    const char *word = argv[1];
    int depth = atoi(argv[2]);
    if (depth == 0)
    {
        fprintf(stderr, "Depth should be positive integer, given \"%s\".\n", argv[2]);
        return -1;
    }
    return lurkDirs(absPath, "", depth, word);
}
