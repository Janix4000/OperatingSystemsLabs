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
} Oven;

typedef struct
{
    int pizzas[N_BUFF];
    size_t beg;
    size_t end;
} Table;

static inline void init_table(Table *table)
{
    for (size_t i = 0; i < N_BUFF; i++)
    {
        table->pizzas[i] = EMPTY;
    }
    table->beg = 0;
    table->end = 0;
}

static inline int put_pizza_into_oven(Oven *oven, int val)
{
    for (size_t i = 0; i < N_BUFF; i++)
    {
        if (oven->pizzas[i] == EMPTY)
        {
            oven->pizzas[i] = val;
            return i;
        }
    }
    return -1;
}

static inline int get_pizza_from_oven(Oven *oven, int idx)
{
    int val = oven->pizzas[idx];
    oven->pizzas[idx] = EMPTY;
    return val;
}

static inline void put_pizza_on_table(Table *table, int val)
{
    table->pizzas[table->end] = val;
    table->end++;
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