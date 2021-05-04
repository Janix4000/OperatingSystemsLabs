#pragma once

#include <stdlib.h>
#define SHM_OVEN "oven"
#define SHM_TABLE "table"

#define SEM_OVEN_GATE "oven_gate"
#define SEM_OVEN_PRODUCENT "oven_prod"
#define SEM_OVEN_CONSUMER "oven_cons"

#define SEM_TABLE_GATE "table_gate"
#define SEM_TABLE_PRODUCENT "table_prod"
#define SEM_TABLE_CONSUMER "table_cons"

#define N_BUFF 5

#define EMPTY -1

typedef struct
{
    int pizzas[N_BUFF];
    int n;
} Oven;

typedef struct
{
    int pizzas[N_BUFF];
    size_t beg;
    size_t end;
    int n;
} Table;

static inline void init_oven(Oven *oven)
{
    for (size_t i = 0; i < N_BUFF; i++)
    {
        oven->pizzas[i] = EMPTY;
    }
    oven->n = 0;
}

static inline void init_table(Table *table)
{
    for (size_t i = 0; i < N_BUFF; i++)
    {
        table->pizzas[i] = EMPTY;
    }
    table->beg = 0;
    table->end = 0;
    table->n = 0;
}

static inline int put_pizza_into_oven(Oven *oven, int val)
{
    for (size_t i = 0; i < N_BUFF; i++)
    {
        if (oven->pizzas[i] == EMPTY)
        {
            oven->pizzas[i] = val;
            oven->n++;
            return i;
        }
    }
    return -1;
}

static inline int get_pizza_from_oven(Oven *oven, int idx)
{
    int val = oven->pizzas[idx];
    oven->pizzas[idx] = EMPTY;
    oven->n--;
    return val;
}

static inline void put_pizza_on_table(Table *table, int val)
{
    table->pizzas[table->end] = val;
    table->end++;
    table->n++;
    if (table->end == N_BUFF)
    {
        table->end = 0;
    }
}

static inline int get_pizza_from_table(Table *table)
{
    int val = table->pizzas[table->beg];
    table->pizzas[table->beg] = EMPTY;
    table->beg++;
    table->n--;
    if (table->beg == N_BUFF)
    {
        table->beg = 0;
    }
    return val;
}

#include "lib/sem.h"

typedef struct
{
    semaphore_t prod;
    semaphore_t cons;
    semaphore_t gate;
} ProdConsSems;

#define create_sems(sems, gate_, cons_, prod_) \
    (sems)->gate = create_sem(gate_, 1, 0);    \
    (sems)->cons = create_sem(cons_, 0, 0);    \
    (sems)->prod = create_sem(prod_, N_BUFF, 0)

#define open_sems(sems, gate_, cons_, prod_) \
    (sems)->gate = open_sem(gate_);          \
    (sems)->cons = open_sem(cons_);          \
    (sems)->prod = open_sem(prod_)

static inline void close_sems(ProdConsSems *sems)
{
    close_sem((sems)->gate);
    close_sem((sems)->cons);
    close_sem((sems)->prod);
}

#define remove_sems(sems, gate_, cons_, prod_) \
    remove_sem(gate_);                         \
    remove_sem(cons_);                         \
    remove_sem(prod_)

#include "lib/shm.h"

int create_pizzeria()
{
    return create_shared("pizzeria", sizeof(Oven) + sizeof(Table));
}
void *open_pizzeria()
{
    return open_shared("pizzeria", 0);
}

void extract_pizzeria(void *ptr, Oven **oven, Table **table)
{
    *oven = ptr;
    *table = ptr + sizeof **oven;
}

void close_pizzeria(void *ptr)
{
    close_shared(ptr);
}
void remove_pizzeria(int shmidt)
{
    remove_shared(shmidt, "pizzeria");
}

void apply_destructor(void (*destructor)(void), void (*sigc)(int))
{
    atexit(destructor);

    struct sigaction act;
    act.sa_handler = sigc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

// http://burtleburtle.net/bob/hash/doobs.html
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a -= b;
    a -= c;
    a ^= (c >> 13);
    b -= c;
    b -= a;
    b ^= (a << 8);
    c -= a;
    c -= b;
    c ^= (b >> 13);
    a -= b;
    a -= c;
    a ^= (c >> 12);
    b -= c;
    b -= a;
    b ^= (a << 16);
    c -= a;
    c -= b;
    c ^= (b >> 5);
    a -= b;
    a -= c;
    a ^= (c >> 3);
    b -= c;
    b -= a;
    b ^= (a << 10);
    c -= a;
    c -= b;
    c ^= (b >> 15);
    return c;
}

#define pt_printf(TEXT, ...)                 \
    printf("(%d, %ld) ", getpid(), clock()); \
    printf(TEXT, __VA_ARGS__)
