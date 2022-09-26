//FCFS
//SJF
//RR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>

//David Ekstedt och Samer Dawood

//global variables
sem_t semEmpty;
sem_t semFull;
int TimeInterval;
pthread_mutex_t myLock;
int *buffer;
int count = 0;

//terminate the program
void terminate (int sig){
  printf("::::Terminated ::::\n");
  (void) signal (SIGINT, SIG_DFL);
  sem_destroy(&semEmpty);
  sem_destroy(&semFull);
  pthread_mutex_destroy(&myLock);
  exit(0);
}

void* producer(void* args) {
    while (1) {
        // Making a random int between 0-99
        int x = rand() % 100;
        //sleep(TimeInterval);
/*--- Critical Section begins! */
        // Add to the buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&myLock);
        buffer[count] = x;
        count++;
        pthread_mutex_unlock(&myLock);
        sem_post(&semFull);
        printf("Produced %d\n", x);
    }
}

void* consumer(void* ind) {
  int index = *(int *)ind;
    while (1) {
      int y;

      // Remove from the buffer
      sem_wait(&semFull);
      pthread_mutex_lock(&myLock);
      y = buffer[count - 1];
      count--;
      pthread_mutex_unlock(&myLock);
      sem_post(&semEmpty);
/*--- Critical Section ends ! */
      // prints consumer thread
      printf("Consumer #%d ==> %d \n", index, y);
      sleep(TimeInterval);
    }
  
}

int main(int argc, char* argv[]) {
  (void) signal (SIGINT, terminate);
    //initialzie variables
    int N;
    int BufferSize;
    int i;

    //printing and scanning input
    printf("N consumer threads = ");
    scanf("%d",&N);

    //printing and scanning input
    printf("BufferSize = ");
    scanf("%d",&BufferSize);
    buffer = malloc(BufferSize*sizeof(int));

    //printing and scanning input
    printf("TimeInterval = ");
    scanf("%d",&TimeInterval);

    //getting a random number with the seed time
    //srand(time(NULL));
    pthread_t produc;
    pthread_t comsum[N];
    pthread_mutex_init(&myLock, NULL);
    sem_init(&semEmpty, 0, BufferSize);
    sem_init(&semFull, 0, 0);
    

    //kollar om producer är ledig
    if (pthread_create(&produc, NULL, &producer, NULL) != 0) {
      fprintf(stderr, "Failed to create the producer thread");
    }
    //check if consumer is empty
    for (i = 0; i < N; i++) {
      if (pthread_create(&comsum[i], NULL, &consumer, &i) != 0){
        fprintf(stderr, "Failed to create the consumer thread");
      }
    }
    
    // Vänta alla barn trådar för att förena sig med de
    for (i = 0; i < N; i++) {
      //kollar om det finns en thread annars 
      if (pthread_join(comsum[i], NULL) != 0) {
        fprintf(stderr, "Failed to join thread");
      }
    }
    if (pthread_join(produc, NULL) != 0) {
      fprintf(stderr, "Failed to join thread");
    }

    
    return 0;
}


