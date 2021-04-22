#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void sleep_random_up_to(int ms)
{
    usleep((rand() % ms) * 1000);
}

int main(int argc, char const *argv[])
{
    if (argc != 1 + 4)
    {
        return EXIT_FAILURE;
    }
    srand(time(NULL));

    const char *pipe_path = argv[1];
    const long line_n = strtol(argv[2], NULL, 10);
    assert(errno == 0);
    const char *path = argv[3];
    const long n = strtol(argv[4], NULL, 10);
    assert(errno == 0);

    FILE *pipe = fopen(pipe_path, "w");
    assert(pipe != NULL);
    FILE *file = fopen(path, "r");
    assert(file != NULL);
    char *buffer = malloc(n + 1);
    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, n, file)) != 0)
    {
        buffer[read_bytes] = 0;
        sleep_random_up_to(500);
        fprintf(pipe, "%ld %s\n", line_n, buffer);
    }
    free(buffer);
    fclose(pipe);
    fclose(file);
    return 0;
}