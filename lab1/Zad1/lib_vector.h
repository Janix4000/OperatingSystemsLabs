#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    size_t capacity;
    size_t size;
    void **container;
} LibVector;

void *vecPushBack(LibVector *vector, void *obj);
void *vecPopBack(LibVector *vector);
void *vecEraseAt(LibVector *vector, int idx);
LibVector *vecInit(LibVector *vector);
void vecClear(LibVector *vector);
void vecFree(LibVector *vector);
