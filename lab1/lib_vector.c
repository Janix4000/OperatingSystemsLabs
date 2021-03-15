#include "lib_vector.h"
#include <string.h>

void *vecPushBack(Vector *vec, void *obj)
{
    if (!vec)
    {
        vec->capacity = 4;
        vec->container = calloc(sizeof *vec->container,
                                vec->capacity);
    }
    if (vec->size == vec->capacity)
    {
        vec->capacity *= 2;
        vec->container =
            realloc(vec->container, sizeof *vec->container * vec->capacity);
    }
    vec->container[vec->size] = obj;
    vec->size++;
    return obj;
}
void *vecPopBack(Vector *vector)
{
    return vecEraseAt(vector, vector->size - 1);
}
void *vecEraseAt(Vector *vec, int idx)
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

Vector *vecInit(Vector *vector)
{
    vector->capacity = 0;
    vector->size = 0;
    vector->container = NULL;
}

void vecClear(Vector *vector)
{
    free(vector->container);
    vecInit(vector);
}
void vecFree(Vector *vector)
{
    vecClear(vector);
}