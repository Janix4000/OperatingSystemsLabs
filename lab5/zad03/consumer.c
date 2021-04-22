#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define CMD_SIZE 65536
#define BUFFER_SIZE 32767

int main(int argc, char const *argv[])
{
    if (argc != 1 + 3)
    {
        return EXIT_FAILURE;
    }
    const char *pipe_path = argv[1];
    const char *path = argv[2];
    const long n = strtol(argv[3], NULL, 10);
    assert(errno == 0);

    FILE *pipe = fopen(pipe_path, "r");
    assert(pipe != NULL);

    char cmd[CMD_SIZE];
    sprintf(cmd, "yes '' | head -n 100 > %s", path);
    system(cmd);

    int line_n = -1;
    char buf[BUFFER_SIZE];
    while (fscanf(pipe, "%d %s\n", &line_n, buf) > 0)
    {
        sprintf(cmd, "sed -i '%ds/^/%s/' %s", line_n, buf, path);
        system(cmd);
    }
    sprintf(cmd, "_x_=$(cat %s); printf '%%s\\n' \"$_x_\" > %s", path, path);
    system(cmd);
    fclose(pipe);
    return 0;
}