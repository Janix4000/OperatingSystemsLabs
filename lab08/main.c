#include <stdio.h>
#include <stdlib.h>

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
    return 0;
}

void negate(const Image *src, Image *dest)
{
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
    return 0;
}

int main(int argc, char const *argv[])
{
    Image img;
    load_pgm("file.pgm", &img);
    save_pgm("file2.pgm", &img);
    free_image(&img);
    return 0;
}
