#include "lib/sem.h"
#include "lib/shm.h"

#include <sys/wait.h>

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

void exec_prog(const char *filename);

int main(int argc, char const *argv[])
{
    argc--;
    argv++;
    if (argc != 2)
    {
        fprintf(stderr, "Expected two args\n");
        exit(-1);
    }
    const int n = atoi(argv[0]);
    const int m = atoi(argv[1]);

    if (n <= 0 || m <= 0)
    {
        fprintf(stderr, "Bad args\n");
        exit(-1);
    }

    init();

    for (size_t i = 0; i < n; i++)
    {
        exec_prog("./cooker.out");
    }
    for (size_t i = 0; i < m; i++)
    {
        exec_prog("./delivery_man.out");
    }

    apply_destructor(destructor, sigc);
    while (true)
    {
        ;
    }

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

static inline void kill_children()
{
    signal(SIGQUIT, SIG_IGN);
    kill(-getpid(), SIGQUIT);

    while (wait(NULL) > 0)
        ;
}

void destructor()
{
    kill_children();

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

void exec_prog(const char *filename)
{
    int pid;
    if ((pid = fork()) == 0)
    {
        execlp(filename, filename, NULL);
        perror("execlp");
        exit(-1);
    }
    else if (pid == -1)
    {
        perror("fork");
        exit(-1);
    }
}