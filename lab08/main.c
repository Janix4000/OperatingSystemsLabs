#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct
{
    unsigned char *img;
    int width;
    int height;
} Image;

void free_image(Image *img)
{
    free(img->img);
}

int save_pgm(const char *filename, const Image *img)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("fopen");
        return -1;
    }
    const char pred[] = "P2\n";
    fputs(pred, file);
    const int max_col = 255;
    fprintf(file, "%d %d\n%d\n", (int)img->width, (int)img->height, max_col);
    for (size_t y = 0; y < img->height; y++)
    {
        for (size_t x = 0; x < img->width; x++)
        {
            int tmp = (int)img->img[x + y * img->height];
            fprintf(file, "%d ", tmp);
        }
        fprintf(file, "\n");
    }
    fclose(file);
    return 0;
}

void negate(const Image *src, Image *dest)
{
    *dest = *src;
    dest->img = malloc(dest->width * dest->height);
    for (size_t y = 0; y < src->height; y++)
    {
        for (size_t x = 0; x < src->width; x++)
        {
            unsigned char tmp = src->img[x + y * src->height];
            dest->img[x + y * src->height] = 255 - tmp;
        }
    }
}

int load_pgm(const char *filename, Image *img)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("fopen");
        return -1;
    }
    char pred[3];
    fgets(pred, sizeof(pred), file);
    int max_col;
    fscanf(file, "%d %d %d", &img->width, &img->height, &max_col);
    img->img = malloc(img->width * img->height);
    for (size_t y = 0; y < img->height; y++)
    {
        for (size_t x = 0; x < img->width; x++)
        {
            int tmp;
            fscanf(file, "%d", &tmp);
            img->img[x + y * img->height] = (unsigned char)tmp;
            // printf("%d ", tmp);
        }
        // printf("\n");
    }
    fclose(file);
    return 0;
}

typedef struct
{
    int n_threads;
    int id;
    const Image *src;
    Image *dest;
} ThreadArgs;

void *negate_numbers(void *args)
{
    ThreadArgs *number_args = args;

    const Image *src = number_args->src;
    Image *dest = number_args->dest;

    for (size_t y = 0; y < src->height; y++)
    {
        for (size_t x = 0; x < src->width; x++)
        {
            unsigned char tmp = src->img[x + y * src->height];
            if (tmp % number_args->n_threads == number_args->id)
            {
                dest->img[x + y * src->height] = 255 - tmp;
            }
        }
    }
    return NULL;
}

void *negate_blocks(void *args)
{
    ThreadArgs *number_args = args;

    const Image *src = number_args->src;
    Image *dest = number_args->dest;

    const int n_threads = number_args->n_threads;
    const int id = number_args->id;
    const size_t min_x = id * (src->width / n_threads);
    const size_t max_x = id < n_threads - 1 ? (id + 1) * (src->width / n_threads) : src->width;

    printf("Thread %d from %ld to %ld\n", id, min_x, max_x);

    for (size_t y = 0; y < src->height; y++)
    {
        for (size_t x = min_x; x < max_x; x++)
        {
            unsigned char tmp = src->img[x + y * src->height];
            dest->img[x + y * src->height] = 255 - tmp;
        }
    }
    printf("Thread %d done\n", id);
    return NULL;
}

void run_method(int n_threads, Image *src, Image *dest, void *(*method)(void *))
{
    pthread_t *threads = calloc(sizeof *threads, n_threads);
    ThreadArgs *args = calloc(sizeof *threads, n_threads);

    for (size_t i = 0; i < n_threads; i++)
    {
        args[i] = (ThreadArgs){.src = src, .dest = dest, .id = i, .n_threads = n_threads};
        pthread_create(threads + i, NULL, method, &args[i]);
    }
    printf("Waiting for join\n");
    for (size_t i = 0; i < n_threads; i++)
    {
        int res = pthread_join(threads[i], NULL);
        if (res == -1)
        {
            perror("pthread_join");
        }
        printf("Joined thread %ld \n", i);
    }
    printf("Operations done\n");
    free(threads);
    free(args);
}

int main(int argc, char **argv)
{
    argc--;
    argv++;
    if (argc != 4)
    {
        perror("Bad args");
        exit(-1);
    }
    int n_threads = atoi(argv[0]);
    if (n_threads <= 0)
    {
        perror("Bad n_threads");
        exit(-1);
    }
    const char *type = argv[1];
    const char *in_name = argv[2];
    const char *out_name = argv[3];

    Image src, dest;
    if (load_pgm(in_name, &src) == -1)
    {
        exit(-1);
    }
    dest = src;
    dest.img = malloc(dest.width * dest.height);

    if (strcmp(type, "numbers") == 0)
    {
        run_method(n_threads, &src, &dest, negate_numbers);
    }
    else if (strcmp(type, "block") == 0)
    {
        run_method(n_threads, &src, &dest, negate_blocks);
    }
    else
    {
        perror("Bad type");
        exit(-1);
    }

    if (save_pgm(out_name, &dest) == -1)
    {
        exit(-1);
    }

    free_image(&src);
    free_image(&dest);
    return 0;
}
