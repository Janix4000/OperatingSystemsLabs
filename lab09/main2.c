#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define SANTA_IDX 0

#define N_SANTA_SHIPPED_MAX 3
#define N_REINDEERS 9
#define N_ELVES 10
#define N_ELVES_TRIGGER 3

#define N_TOTAL_WORKERS (1 + N_REINDEERS + N_ELVES)

void mutex_unlock_routine(void *arg);

void *santa_claus(void *);
void *elf(void *id);
void *reindeer(void *id);

#define lock(mutex)                                  \
    for (bool _iter = true;                          \
         _iter && (pthread_mutex_lock(mutex), true); \
         _iter = (pthread_mutex_unlock(mutex), false))

pthread_mutex_t santa_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_wakeup_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_wakeup_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_wakeup_cond = PTHREAD_COND_INITIALIZER;

size_t n_waiting_reindeers = 0;
size_t n_waiting_elves = 0;
size_t waiting_elves_ids[N_ELVES_TRIGGER];

void mutex_unlock_routine(void *arg);
int rand_sleep(time_t secs);

int main(int argc, char const *argv[])
{
    int result = EXIT_FAILURE;

    pthread_t invalid_thread = pthread_self();
    pthread_t workers[N_TOTAL_WORKERS];

    if (pthread_create(&workers[SANTA_IDX], NULL, santa_claus, NULL) != 0)
    {
        workers[SANTA_IDX] = invalid_thread;
        goto cleanup;
    }

    for (size_t i = 1; i < 1 + N_REINDEERS; i++)
    {
        if (pthread_create(&workers[i], NULL, reindeer, (void *)i) != 0)
        {
            workers[i] = invalid_thread;
            goto cleanup;
        }
    }

    for (size_t i = 1 + N_REINDEERS; i < 1 + N_REINDEERS + N_ELVES; i++)
    {
        if (pthread_create(&workers[i], NULL, elf, (void *)i) != 0)
        {
            workers[i] = invalid_thread;
            goto cleanup;
        }
    }

    //wait for Santa to finish and kill other threads
    pthread_join(workers[SANTA_IDX], NULL);
    workers[SANTA_IDX] = invalid_thread;

    result = EXIT_SUCCESS;

cleanup:
    for (size_t i = 1; i < N_TOTAL_WORKERS && !pthread_equal(invalid_thread, workers[i]); i++)
    {
        pthread_cancel(workers[i]);
    }

    for (size_t i = 1; i < N_TOTAL_WORKERS && !pthread_equal(invalid_thread, workers[i]); i++)
    {
        pthread_join(workers[i], NULL);
    }

    return result;
}

void *santa_claus(void *none)
{
    size_t n_gifted_presents = 0;
    while (n_gifted_presents < N_SANTA_SHIPPED_MAX)
    {

        lock(&santa_mut)
        {
            pthread_cleanup_push(mutex_unlock_routine, &santa_mut);

            while (n_waiting_elves != N_ELVES_TRIGGER && n_waiting_reindeers != N_REINDEERS)
            {
                pthread_cond_wait(&santa_wakeup_cond, &santa_mut);
            }

            printf("Mikołaj: \"Wstaję z kanapy.\"\n");

            if (n_waiting_reindeers == N_REINDEERS)
            {
                printf("Mikołaj: \"Dostarczam zabawki z reniferami.\"\n");
                rand_sleep(2);
                printf("Mikołaj: \"Dostarczyłem zabawki.\"\n");

                n_waiting_reindeers = 0;
                n_gifted_presents++;

                pthread_cond_broadcast(&reindeers_wakeup_cond);
            }
            else if (n_waiting_elves == N_ELVES_TRIGGER)
            {
                printf(
                    "Mikołaj: \"Rozwiązuję problemy elfów stażystów %zu, %zu, %zu.\"\n",
                    waiting_elves_ids[0],
                    waiting_elves_ids[1],
                    waiting_elves_ids[2]);
                rand_sleep(2);

                n_waiting_elves = 0;
                pthread_cond_broadcast(&elves_wakeup_cond);
            }

            pthread_cleanup_pop(false);
        }
    }

    return NULL;
}

void *elf(void *id)
{
    size_t worker_id = (size_t)id;

    size_t last_spot = N_ELVES_TRIGGER + 1;

    while (true)
    {
        lock(&santa_mut)
        {
            pthread_cleanup_push(mutex_unlock_routine, &santa_mut);

            //wait until Santa Claus is done dealing with me
            while (last_spot < n_waiting_elves && waiting_elves_ids[last_spot] == worker_id)
            {
                pthread_cond_wait(&elves_wakeup_cond, &santa_mut);
            }

            pthread_cleanup_pop(false);
        }

        printf("Elf: \"Praca praca.\", %zu\n", worker_id);
        rand_sleep(2);

        lock(&santa_mut)
        {
            pthread_cleanup_push(mutex_unlock_routine, &santa_mut);

            //try to find a spot in stuck_elves_ids
            bool first_try = true;
            while (n_waiting_elves == N_ELVES_TRIGGER)
            {
                if (first_try)
                {
                    printf("Elf: \"Czekam na powrót innych elfów.\", %zu\n", worker_id);
                    first_try = false;
                }

                pthread_cond_wait(&elves_wakeup_cond, &santa_mut);
            }

            last_spot = n_waiting_elves;
            waiting_elves_ids[n_waiting_elves++] = worker_id;
            printf("Elf: \"Na Mikołaja czeka %zu elfów.\", %zu\n", n_waiting_elves, worker_id);

            if (n_waiting_elves == N_ELVES_TRIGGER)
            {
                pthread_cond_signal(&santa_wakeup_cond);
            }

            pthread_cleanup_pop(false);
        }
    }

    return NULL;
}

void *reindeer(void *id)
{
    size_t reinder_id = (size_t)id;

    while (true)
    {
        lock(&santa_mut)
        {
            pthread_cleanup_push(mutex_unlock_routine, &santa_mut);

            //wait until reindeers are dealt with
            while (n_waiting_reindeers > 0)
            {
                pthread_cond_wait(&reindeers_wakeup_cond, &santa_mut);
            }

            pthread_cleanup_pop(false);
        }

        printf("Renifer: \"Dostarczam zabawki.\", %zu\n", reinder_id);
        rand_sleep(2);
        printf("Renifer: \"Pora na zasłużone wakacje.\", %zu\n", reinder_id);
        rand_sleep(5);

        lock(&santa_mut)
        {
            pthread_cleanup_push(mutex_unlock_routine, &santa_mut);

            n_waiting_reindeers++;
            printf("Renifer: \"Na Mikołaja czeka ze mną %zu reniferów.\", %zu\n", n_waiting_reindeers, reinder_id);

            if (n_waiting_reindeers == N_REINDEERS)
            {
                pthread_cond_signal(&santa_wakeup_cond);
            }

            pthread_cleanup_pop(false);
        }
    }

    return NULL;
}

void mutex_unlock_routine(void *arg)
{
    pthread_mutex_t *mut = arg;
    pthread_mutex_unlock(mut);
}