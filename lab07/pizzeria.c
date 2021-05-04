#include "lib/sem.h"
#include "lib/shm.h"

#include "pizza_rules.h"

ProdConsSems oven_sem;
ProdConsSems table_sem;
int shmid;
void *shared_ptr;
Oven *oven;
Table *table;

void init();
void destructor();
void sigc(int sig_no);

int main(int argc, char const *argv[])
{
    init();

    apply_destructor(destructor, sigc);
    return 0;
}

void init()
{
    create_sems(&oven_sem, SEM_OVEN_GATE, SEM_OVEN_CONSUMER, SEM_OVEN_PRODUCENT);
    create_sems(&table_sem, SEM_TABLE_GATE, SEM_TABLE_CONSUMER, SEM_TABLE_PRODUCENT);

    shmid = create_pizzeria();
    shared_ptr = open_pizzeria();
    extract_pizzeria(shared_ptr, &oven, &table);
    init_table(table);
    init_oven(oven);
}

void destructor()
{
    close_sems(&oven_sem);
    close_sems(&table_sem);
    remove_sems(&oven_sem, SEM_OVEN_GATE, SEM_OVEN_CONSUMER, SEM_OVEN_PRODUCENT);
    remove_sems(&table_sem, SEM_TABLE_GATE, SEM_TABLE_CONSUMER, SEM_TABLE_PRODUCENT);

    close_pizzeria(shared_ptr);
    remove_pizzeria(shmid);
}

void sigc(int sig_no)
{
    printf("trl + C\n");
    exit(0);
}