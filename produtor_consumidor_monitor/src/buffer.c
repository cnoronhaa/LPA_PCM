#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define N_PRODUTORES 2
#define N_CONSUMIDORES 2

int buffer[TAM_BUFFER];
int in = 0, out = 0, count = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_cons = PTHREAD_COND_INITIALIZER;

void* produtor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        int item = rand() % 100; 
        sleep(rand() % 3); 

        pthread_mutex_lock(&lock);

        while (count == TAM_BUFFER) {
            // espera espaço no buffer
            pthread_cond_wait(&cond_prod, &lock);
        }

        // insere no buffer
        buffer[in] = item;
        in = (in + 1) % TAM_BUFFER;
        count++;

        printf("[Produtor %d] Inseriu: %d\n", id, item);

        pthread_cond_signal(&cond_cons); 
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* consumidor(void* arg) {
    int id = *((int*)arg);
    while (1) {
        sleep(rand() % 3); 

        pthread_mutex_lock(&lock);

        while (count == 0) {
            // espera item no buffer
            pthread_cond_wait(&cond_cons, &lock);
        }

        // remove do buffer
        int item = buffer[out];
        out = (out + 1) % TAM_BUFFER;
        count--;

        printf("[Consumidor %d] Removeu: %d\n", id, item);

        pthread_cond_signal(&cond_prod); 
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t produtores[N_PRODUTORES], consumidores[N_CONSUMIDORES];
    int ids_prod[N_PRODUTORES], ids_cons[N_CONSUMIDORES];

    for (int i = 0; i < N_PRODUTORES; i++) {
        ids_prod[i] = i + 1;
        pthread_create(&produtores[i], NULL, produtor, &ids_prod[i]);
    }

    for (int i = 0; i < N_CONSUMIDORES; i++) {
        ids_cons[i] = i + 1;
        pthread_create(&consumidores[i], NULL, consumidor, &ids_cons[i]);
    }

    for (int i = 0; i < N_PRODUTORES; i++)
        pthread_join(produtores[i], NULL);
    for (int i = 0; i < N_CONSUMIDORES; i++)
        pthread_join(consumidores[i], NULL);

    return 0;
}
