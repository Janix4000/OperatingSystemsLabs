#include "pizza_rules.h"

#include <stdlib.h>
#include <time.h>

ProdConsSems oven_sem;
ProdConsSems table_sem;
int shmid;
void *shared_ptr;
Oven *oven;
Table *table;

int pizza = -1;

void init();
void destructor();
void sigc(int sig_no);

void make_pizza();
void serve_pizza();

int main(int argc, char const *argv[])
{
    init();
    apply_destructor(destructor, sigc);

    while (true)
    {
        make_pizza();
        serve_pizza();
    }

    return 0;
}

void init()
{
    srand(mix(clock(), time(NULL), getpid()));

    open_sems(&oven_sem, SEM_OVEN_GATE, SEM_OVEN_CONSUMER, SEM_OVEN_PRODUCENT);
    open_sems(&table_sem, SEM_TABLE_GATE, SEM_TABLE_CONSUMER, SEM_TABLE_PRODUCENT);

    shared_ptr = open_pizzeria();
    extract_pizzeria(shared_ptr, &oven, &table);

    if (errno != 0)
    {
        perror("failure");
        exit(-1);
    }
}

void destructor()
{
    close_sems(&oven_sem);
    close_sems(&table_sem);

    close_pizzeria(shared_ptr);
    pt_printf("Cooker down %d\n", 0);
}

void sigc(int sig_no)
{
    exit(0);
}

void make_pizza()
{
    pizza = 0 + rand() % 9;
    int making_time = 1 + rand() % 2;

    pt_printf("Przygotowywuje pizze [%d].\n", pizza);
    sleep(making_time);

    int cooking_time = 4 + rand() % 2;

    decr_sem(oven_sem.prod);

    decr_sem(oven_sem.gate);
    int idx = put_pizza_into_oven(oven, pizza);
    pt_printf("Dodałem pizze [%d] do pieca. Liczba pizz aktualnie w piecu: %d.\n", pizza, oven->n);
    incr_sem(oven_sem.gate);

    sleep(cooking_time);

    decr_sem(oven_sem.gate);

    pizza = get_pizza_from_oven(oven, idx);
    pt_printf("Wyjmuję pizze [%d] z pieca. Liczba pozostalych pizz w piecu: %d.\n", pizza, oven->n);

    incr_sem(oven_sem.gate);

    incr_sem(oven_sem.prod);
}

void serve_pizza()
{
    decr_sem(table_sem.prod);

    decr_sem(table_sem.gate);
    pt_printf("Kladę pizze [%d] na stole. Liczba pizz na stole przed polozeniem: %d.\n", pizza, table->n);
    put_pizza_on_table(table, pizza);
    incr_sem(table_sem.gate);

    incr_sem(table_sem.cons);
}