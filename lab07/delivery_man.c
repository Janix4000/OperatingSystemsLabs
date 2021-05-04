#include "pizza_rules.h"

#include <stdlib.h>
#include <time.h>

ProdConsSems table_sem;
int shmid;
void *shared_ptr;
Oven *oven;
Table *table;

int pizza = -1;

void init();
void destructor();
void sigc(int sig_no);

void take_pizza();
void deliver_pizza();

int main(int argc, char const *argv[])
{
    init();
    apply_destructor(destructor, sigc);

    while (true)
    {
        take_pizza();
        deliver_pizza();
    }

    return 0;
}

void init()
{
    srand(mix(clock(), time(NULL), getpid()));

    open_sems(&table_sem, SEM_TABLE_GATE, SEM_TABLE_CONSUMER, SEM_TABLE_PRODUCENT);

    shared_ptr = open_pizzeria();
    extract_pizzeria(shared_ptr, &oven, &table);
}

void destructor()
{
    close_sems(&table_sem);

    close_pizzeria(shared_ptr);
    pt_printf("Delivery man down %d\n", 0);
}

void sigc(int sig_no)
{
    exit(0);
}

void take_pizza()
{
    decr_sem(table_sem.cons);

    decr_sem(table_sem.gate);
    pizza = get_pizza_from_table(table);
    pt_printf("Pobieram pizze [%d] ze stolu. Liczba pozostalych pizz na stole: %d.\n", pizza, table->n);
    incr_sem(table_sem.gate);

    incr_sem(table_sem.prod);
}

void deliver_pizza()
{
    int deliver_time = 4 + rand() % 2;
    sleep(deliver_time);
    pt_printf("Dostarczam pizze [%d].\n", pizza);

    int return_time = 4 + rand() % 2;
    sleep(return_time);
}