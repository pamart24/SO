#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
pthread_mutex_t forks[NR_PHILOSOPHERS];

//Para semaforos
sem_t forksSem[NR_PHILOSOPHERS];

void init()
{
    int i;
    for(i=0; i<NR_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i],NULL);
	sem_init(&forksSem[i], 0, 1);
    }
}

void think(int i) {
    printf("Philosopher %d thinking... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n" , i);

}

void eat(int i) {
    printf("Philosopher %d eating... \n" , i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n" , i);

}

void toSleep(int i) {
    printf("Philosopher %d sleeping... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n" , i);
    
}

//Filosofos con mutex
void* philosopher(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    while(1)
    {        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)
	//Cogemos primero el tenedor con valor menor
	if (right < left) {
		pthread_mutex_lock(&forks[right]);
		pthread_mutex_lock(&forks[left]);
	}
	else {
		pthread_mutex_lock(&forks[left]);
		pthread_mutex_lock(&forks[right]);
	}

        eat(nPhilosopher);
        
        // PUT FORKS BACK ON THE TABLE
	//Desbloqueamos los dos tenedores
        //Hay que soltarlos en el orden inverso en que lo coges
	//para que el de tu lado menor pueda comer
	if (right > left) {
		pthread_mutex_unlock(&forks[right]);
		pthread_mutex_unlock(&forks[left]);
	}
	else {
		pthread_mutex_unlock(&forks[left]);
		pthread_mutex_unlock(&forks[right]);
	}
        toSleep(nPhilosopher);
   }
}

//Filosofos con semaforos
void* philosopherSem(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    while(1)
    {        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)
	//Cogemos primero el tenedor con valor menor
	if (right < left) {
		sem_wait(&forksSem[right]);
		sem_wait(&forksSem[left]);
	}
	else {
		sem_wait(&forksSem[left]);
		sem_wait(&forksSem[right]);
	}

        eat(nPhilosopher);
        
        // PUT FORKS BACK ON THE TABLE
	//Desbloqueamos los dos tenedores
        //Hay que soltarlos en el orden inverso en que lo coges
	//para que el de tu lado menor pueda comer
	if (right > left) {
		sem_post(&forksSem[right]);
		sem_post(&forksSem[left]);
	}
	else {
		sem_post(&forksSem[left]);
		sem_post(&forksSem[right]);
	}
        toSleep(nPhilosopher);
   }
}

int main()
{
    init();
    unsigned long i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopherSem, (void*)i);
    
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);
    return 0;
} 
