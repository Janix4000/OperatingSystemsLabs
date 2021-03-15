#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    size_t capacity;
    size_t size;
    void **container;
} Vector;

void *vecPushBack(Vector *vector, void *obj);
void *vecPopBack(Vector *vector);
void *vecEraseAt(Vector *vector, int idx);
Vector *vecInit(Vector *vector);
void vecClear(Vector *vector);
void vecFree(Vector *vector);
