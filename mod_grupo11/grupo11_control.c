#include "module.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>

/**
* Alunos: Hiago Medeiros e Rodrigo Schmitz
**/

pthread_t threads[PROCESSORS_COUNT], reader;

int rocks[PROCESSORS_COUNT];
int cancelled, i, f, contador, setup = 0;

pthread_mutex_t transportar;
pthread_mutex_t controlar_pedras;
pthread_cond_t tem_pedras;
pthread_cond_t tem_espaco;

rock_t pegarPedra()
{
    pthread_mutex_lock(&controlar_pedras);

    while(contador == 0)
    {
        pthread_cond_wait(&tem_pedras, &controlar_pedras);
    }

    i = ( i + 1 ) % PROCESSORS_COUNT;
    rock_t rock = rocks[i];
    contador--;

    pthread_mutex_unlock(&controlar_pedras);
    pthread_cond_signal(&tem_espaco);

    return rock;
}

void guardarPedra(rock_t rock)
{
    pthread_mutex_lock(&controlar_pedras);

    while(contador == PROCESSORS_COUNT)
    {
        pthread_cond_wait(&tem_espaco, &controlar_pedras);
    }

    f = ( f + 1 ) % PROCESSORS_COUNT;
    rocks[f] = rock;
    contador++;

    pthread_mutex_unlock(&controlar_pedras);
    pthread_cond_signal(&tem_pedras);
}

static void* worker(void* ignored)
{
    while (cancelled == 0)
    {
        rock_t rock = pegarPedra();

        oil_t oil = pd_process(rock);

        pthread_mutex_lock(&transportar);
        pd_deliver(oil);
        pthread_mutex_unlock(&transportar);
    }

    return NULL;
}

static void* lerPedras(void* ignored)
{
    while(cancelled == 0)
    {
        rock_t rock = pd_read();
        guardarPedra(rock);
    }
}

void mod_setup()
{
    assert(setup == 0);
    setup = 1;
    cancelled = 0;
    i = 0;
    f = 0;
    contador = 0;

    pthread_mutex_init(&transportar, NULL);
    pthread_mutex_init(&controlar_pedras, NULL);
    pthread_cond_init(&tem_pedras, NULL);
    pthread_cond_init(&tem_espaco, NULL);
    pthread_create(&reader, NULL, lerPedras, NULL);

    int i;
    for(i = 0; i < PROCESSORS_COUNT; i++)
    {
        pthread_create(&threads[i], NULL, worker, NULL);
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

    pthread_join(reader, NULL);
    pthread_mutex_destroy(&transportar);
    pthread_mutex_destroy(&controlar_pedras);
    pthread_cond_destroy(&tem_pedras);
    pthread_cond_destroy(&tem_espaco);

}
