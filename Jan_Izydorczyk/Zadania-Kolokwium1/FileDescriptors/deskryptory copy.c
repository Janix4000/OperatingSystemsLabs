

#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* 
Wstaw we wskazanych miejscach odpowienie deskryptory, sposrod 
fd1, fd2, fd3, by uzyskac na terminalu nastepujacy efekt dzialania 
programu:

Hello,
Hello, 12345678
HELLO, 12345678
WITAM! 12345678


Prosze nie wprowadzac innych zmian do programu niz wskazane powyzej w miejscach oznaczonych komentarzami

*/

int main(int argc, char *argv[])
{
    int fd1, fd2, fd3;
#define file "a"
    char cmd[] = "cat " file "; echo";

    /***/ = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 == -1)
    {
        fprintf(stderr, "open fd1");
        exit(-1);
    }
    /***/ = dup(/***/);
    if (/***/ == -1)
    {
        fprintf(stderr, "dup");
        exit(-1);
    }
    /***/ = open(file, O_RDWR);
    if (/***/ == -1)
    {
        fprintf(stderr, "open fd3");
        exit(-1);
    }

    if (write(/***/, "Hello,", 6) == -1)
    {
        fprintf(stderr, "write1");
        exit(-1);
    }
    system(cmd);
    if (write(/***/, " 12345678", 9) == -1)
    {
        fprintf(stderr, "write2");
        exit(-1);
    }
    system(cmd);
    if (lseek(/***/, 0, SEEK_SET) == -1)
    {
        fprintf(stderr, "lseek");
        exit(-1);
    }

    if (write(/***/, "HELLO,", 6) == -1)
    {
        fprintf(stderr, "write3");
        exit(-1);
    }

    system(cmd);
    if (write(/***/, "WITAM!", 6) == -1)
    {
        fprintf(stderr, "write4");
        exit(-1);
    }

    system(cmd);

    if (close(/***/) == -1)
    {
        fprintf(stderr, "close output");
        exit(-1);
    }

    if (close(/***/) == -1)
    {
        fprintf(stderr, "close output");
        exit(-1);
    }

    if (close(/***/) == -1)
    {
        fprintf(stderr, "close output");
        exit(-1);
    }

    exit(EXIT_SUCCESS);
}
