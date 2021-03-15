#include "lib_vector.h"
#include <string.h>
#include <stdio.h>

void *vecPushBack(LibVector *vec, void *obj)
{
    if (!vec)
    {
        vec->capacity = 4;
        vec->container = calloc(sizeof *vec->container,
                                vec->capacity);
    }
    if (vec->size == vec->capacity)
    {
        vecReserve(vec, vec->capacity * 2);
    }
    vec->container[vec->size] = obj;
    vec->size++;
    return obj;
}
void *vecPopBack(LibVector *vector)
{
    return vecEraseAt(vector, vector->size - 1);
}
void *vecEraseAt(LibVector *vec, int idx)
{
    if (!vec || idx >= vec->size)
    {
        return NULL;
    }
    void *obj = vec->container[idx];
    memmove(vec->container + idx,
            vec->container + idx + 1, vec->size - idx - 1);
    vec->size--;
    if (vec->size * 4 < vec->capacity)
    {
        if (vec->size == 0)
        {
            vecFree(vec);
        }
        else
        {
            vec->capacity = vec->capacity / 2;
            vec->container = realloc(vec->container,
                                     sizeof *vec->container * vec->capacity);
        }
    }
    return obj;
}

LibVector *vecInit(LibVector *vector)
{
    vector->capacity = 0;
    vector->size = 0;
    vector->container = NULL;
    return vector;
}

void vecClear(LibVector *vector)
{
    free(vector->container);
    vecInit(vector);
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