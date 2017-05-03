#include "module.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>

/**
* Alunos: Hiago Medeiros e Rodrigo Schmitz
**/
pthread_t threads[PROCESSORS_COUNT];
int cancelled;
int setup = 0;

pthread_mutex_t read;
pthread_mutex_t deliver;

static void* worker(void* ignored)
{
    while (cancelled == 0)
    {
        pthread_mutex_lock(&read);
        rock_t rock = pd_read();
        pthread_mutex_unlock(&read);

        oil_t oil = pd_process(rock);

        pthread_mutex_lock(&deliver);
        pd_deliver(oil);
        pthread_mutex_unlock(&deliver);
    }
    return NULL;
}

void mod_setup()
{
    assert(setup == 0);
    setup = 1;
    cancelled = 0;

    int i;
    for(i = 0; i < PROCESSORS_COUNT; i++)
    {
        pthread_create(&threads[i], NULL, &worker, NULL);
    }

}

char* mod_name()
{
    return "grupo11_control";
}

void mod_shutdown()
{
    assert(setup == 1);
    cancelled = 1;

    int i;
    for(i = 0; i < PROCESSORS_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }

}
