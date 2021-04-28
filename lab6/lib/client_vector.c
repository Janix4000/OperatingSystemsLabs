#include "client_vector.h"
#include <string.h>
#include <stdio.h>

void vecPushBack(LibVector *vec, client c)
{
    if (!vec->capacity)
    {
        vec->capacity = 4;
        vec->container = calloc(sizeof *vec->container,
                                vec->capacity);
    }
    if (vec->size == vec->capacity)
    {
        vecReserve(vec, vec->capacity * 2);
    }
    vec->container[vec->size] = c;
    vec->size++;
}

client vecPopBack(LibVector *vector)
{
    return vecEraseAt(vector, vector->size - 1);
}

client vecEraseAt(LibVector *vec, int idx)
{
    if (!vec || idx >= vec->size)
    {
        fprintf(stderr, "Bad input\n");
    }
    client obj = vec->container[idx];
    vec->size--;
    memmove(vec->container + idx,
            vec->container + idx + 1, sizeof *vec->container * (vec->size - idx));
    // if (vec->size * 4 < vec->capacity)
    // {
    //     if (vec->capacity > 4)
    //     {
    //         vec->capacity = vec->capacity / 2;
    //         vec->container = realloc(vec->container,
    //                                  sizeof *vec->container * vec->capacity);
    //     }
    // }
    return obj;
}

LibVector *vecInit(LibVector *vector, size_t capacity)
{
    vector->capacity = capacity;
    vector->size = 0;
    vector->container = calloc(sizeof *vector->container, vector->capacity);
    return vector;
}

void vecClear(LibVector *vector)
{
    free(vector->container);
    vector->capacity = 0;
    vector->size = 0;
    vector->container = NULL;
}
void vecFree(LibVector *vector)
{
    vecClear(vector);
}

void vecReserve(LibVector *vec, size_t newCapacity)
{
    if (newCapacity > vec->capacity)
    {
        vec->container =
            realloc(vec->container, sizeof *vec->container * newCapacity);
        vec->capacity = newCapacity;
    }
}