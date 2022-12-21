#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct semaphore
{
    int val;
    pthread_mutex_t lock;
    pthread_cond_t c;
} sem_t;

void sem_wait(sem_t *s)
{
    // decrement the value of s by one
    // wait if the value of s is negative
    pthread_mutex_lock(&s->lock);
    s->val--;
    if (s->val < 0)
        pthread_cond_wait(&s->c, &s->lock);
    pthread_mutex_unlock(&s->lock);
}

int teamA, teamB, totalpeople;
pthread_barrier_t waitAll,waitDriver;
pthread_mutex_t lock2=PTHREAD_MUTEX_INITIALIZER;
sem_t semA,semB,semGlob;


void sem_unlock(sem_t *s){
    pthread_mutex_lock(&s->lock);
    pthread_mutex_unlock(&lock2);
    s->val--;
    if (s->val < 0)
        pthread_cond_wait(&s->c, &s->lock);
    pthread_mutex_unlock(&s->lock);
    

}

void sem_post(sem_t *s)
{
    // increment the value of s by one
    // wake a sleeping thread if exists
    pthread_mutex_lock(&s->lock);
    s->val++;
    pthread_cond_signal(&s->c);
    pthread_mutex_unlock(&s->lock);
}

void sem_init(sem_t *sem, int pshared, unsigned int value)
{
    sem->val = value;
}


/*
void *bus_function(void *arg)
{
    char *teamtemp = (char *)arg;
    char team = *teamtemp;
    printf("wtf\n");
    pthread_barrier_wait(&waitAll);
    printf("Thread ID: %ld, Team: %c, I am looking for a car\n", pthread_self(), team);
    printf("wtf2\n");
    share(arg);
    printf("wtf3\n");
 }
 */

void * share(void *arg)
{   
    pthread_barrier_wait(&waitAll);
    char *teamtemp = (char *)arg;
    char team = *teamtemp;
    bool driver=false;
    sem_wait(&semGlob);
    pthread_mutex_lock(&lock2);
    printf("Thread ID: %ld, Team: %c, I am looking for a car\n", pthread_self(), team);
    if(team =='B'&& (semB.val<=-1 && semA.val<=-2) ){
        driver=true;
        printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), team);
        pthread_barrier_destroy(&waitDriver);
        pthread_barrier_init(&waitDriver,NULL,4);
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semB);
        pthread_mutex_unlock(&lock2);
    }
    
    else if(team=='A'&&(semA.val<=-1 && semB.val<=-2)){
        driver=true;
        printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), team);
        pthread_barrier_destroy(&waitDriver);
        pthread_barrier_init(&waitDriver,NULL,4);
        sem_post(&semB);
        sem_post(&semB);
        sem_post(&semA);
        pthread_mutex_unlock(&lock2);

    }
    else if(team=='B' && semB.val<=-3){
        driver=true;
        printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), team);
        pthread_barrier_destroy(&waitDriver);
        pthread_barrier_init(&waitDriver,NULL,4);
        sem_post(&semB);
        sem_post(&semB);
        sem_post(&semB);
        pthread_mutex_unlock(&lock2);
    }
    else if( team=='A' && semA.val<=-3 ){
        driver=true;
        printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), team);

        pthread_barrier_destroy(&waitDriver);
        pthread_barrier_init(&waitDriver,NULL,4);
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semA);
        
        pthread_mutex_unlock(&lock2);
    }
    else{
        if(team=='B'){
         //   printf("%lu sleep",pthread_self());
            sem_unlock(&semB);
        }
        else{
            sem_unlock(&semA);
        }
       // printf("%lu sleep",pthread_self());
        printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), team);
    }
    pthread_barrier_wait(&waitDriver);
    pthread_mutex_lock(&lock2);
    if(driver){
            driver=false;
            printf("Thread ID: %ld, Team: %c, I am the captain and driving the car\n", pthread_self(), team);
           // printf("lock12\n");
           int i=0;
           for(i=0;i<4;i++){
           	sem_post(&semGlob);
           }
    }
    pthread_mutex_unlock(&lock2);
    
}

int main(int argc, char *args[])
{
    teamA = atoi(args[1]);
    teamB = atoi(args[2]);
    totalpeople = teamA + teamB;
    //pthread_mutex_init(&lock2,NULL);
    if (teamA % 2 == 0 && teamB % 2 == 0 && totalpeople % 4 == 0 && totalpeople >=4)
    {
        sem_init(&semA,0,0);
        sem_init(&semB,0,0);
        sem_init(&semGlob,0,5);
        pthread_barrier_init(&waitAll, NULL, totalpeople);
        pthread_barrier_init(&waitDriver,NULL,4);
        int i;
        char a = 'A';
        char b = 'B';
        pthread_t threadarr[totalpeople];
        for (i = 0; i < teamA; i++)
        {
            pthread_create(threadarr + i, NULL, &share, &a);
        }
        for (i = 0; i < teamB; i++)
        {
            pthread_create(threadarr + teamA + i, NULL, &share, &b);
        }
        for (i = 0; i < totalpeople; i++)
        {
            pthread_join(threadarr[i], NULL);
        
        }

        printf("The main terminates\n");
        pthread_barrier_destroy(&waitAll);
        pthread_barrier_destroy(&waitDriver);
    }
    else
    {
        printf("The main terminates\n");
    }

    return 0;
}
