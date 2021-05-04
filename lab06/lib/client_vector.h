#pragma once

#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    size_t capacity;
    size_t size;
    client *container;
} LibVector;

void vecPushBack(LibVector *vector, client c);
client vecPopBack(LibVector *vector);
client vecEraseAt(LibVector *vector, int idx);
LibVector *vecInit(LibVector *vector, size_t capacity);
void vecClear(LibVector *vector);
void vecFree(LibVector *vector);
void vecReserve(LibVector *vector, size_t newCapacity);