#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_READERS 4
#define NUM_WRITERS 2

pthread_t readers[NUM_READERS];
pthread_t writers[NUM_WRITERS];

pthread_mutex_t mreaders;
pthread_mutex_t mwriters;

pthread_cond_t TerminarLeer;
int lectores = 0;
int data = 0;

void* Reader(void) {
    while (1) {
        pthread_mutex_lock(&mreaders);

        if(lectores == 0) {
            pthread_mutex_lock(&mwriters);
        }
        lectores = lectores + 1;
        pthread_mutex_unlock(&mreaders);

        printf("leyendo %d\n", data);

        pthread_mutex_lock(&mreaders);
        lectores = lectores - 1;
        pthread_cond_broadcast(&TerminarLeer);
        if(lectores == 0) {
            pthread_mutex_unlock(&mwriters);
        }
        pthread_mutex_unlock(&mreaders);
        printf("Deja de leer\n");
    }
}

void* Writer(void) {
    while (1) {
        pthread_mutex_lock(&mwriters);
        while(lectores > 0)
            pthread_cond_wait(&TerminarLeer, &mwriters);
        printf("escribiendo %d\n", data);
        data = data + 2;
        pthread_mutex_unlock(&mwriters);
    }
}

int main(int argc, char argv[]) {
    int i;


    for(i=0; i<NUM_READERS; i++) pthread_create(&readers[i], NULL, Reader, (void*)i);
    for(i=0; i<NUM_WRITERS; i++) pthread_create(&writers[i], NULL, Writer, (void*)i);

    // Esperar terminación de los hilos
    for(i=0; i<NUM_READERS; i++) pthread_join(readers[i],NULL);
    for(i=0; i<NUM_WRITERS; i++) pthread_join(writers[i],NULL);

    return 0;
}