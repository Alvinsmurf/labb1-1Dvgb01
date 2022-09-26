#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

sem_t semEmpty;
sem_t semFull;
int nConsumers = 0;
int bufferSize = 0;
int timeInterval = 0;
int Index = 0;

pthread_mutex_t mutexLock;

void input();
void *producer();
void *consumer(void *i);
void push();
void pop();
void bufferPrint();

int main() {
  // main function
  input();
  sem_init(&semEmpty, 0, bufferSize);
  sem_init(&semFull, 0, 0);
  // initating the producer thread? unsure
  pthread_t producerThread;
  pthread_create(&producerThread, NULL, &producer, NULL);
  // initating the consumer thread? unsure
  pthread_t consumerThread[nConsumers];
  for (long i = 0; i < nConsumers; i++)
    pthread_create(&consumerThread[i], NULL, &consumer, (void *)i);

  for (int i = 0; i < nConsumers; i++)
    pthread_join(consumerThread[i], NULL);
  pthread_join(producerThread, NULL);
  pthread_mutex_destroy(&mutexLock);
  pthread_exit(NULL);
}

void input() {
  // input function for the user
  printf("Number of consumers? ");
  scanf("%d", &nConsumers);
  printf("Size of buffer? ");
  scanf("%d", &bufferSize);
  printf("Producer time interval? ");
  scanf("%d", &timeInterval);
}

void *producer() {
  // creating the thread
  printf("Producer thread created \n");
  push();
}

void *consumer(void *i) {
  long id = (long)i;
  printf("Consumer thread %ld created \n", id);
  pop(id);
}

void push() {
  while (1) {
    // locking thread
    sem_wait(&semEmpty);
    pthread_mutex_lock(&mutexLock);
    // creates a new element in the buffer
    printf("Producer created new element at Index %d \n", Index);
    Index++;
    bufferPrint();
    
    pthread_mutex_unlock(&mutexLock);
    sem_post(&semFull);
  }
}

void pop(long id) {
  while (1) {
    // locking thread
    sem_wait(&semFull);
    pthread_mutex_lock(&mutexLock);
    // removing a element from the buffer
    Index--;
    printf("Consumer %ld removed element at Index %d \n", id, Index);
    bufferPrint();
    pthread_mutex_unlock(&mutexLock);
    sem_post(&semEmpty);
    sleep(timeInterval);
    // unlocking thread
  }
}

void bufferPrint() {
  // shows the buffer
  srand(time(NULL));
  for (int i = 0; i < Index; i++) {
    int r = rand()%10;
    printf("%d", r);
  }
  for (int i = 0; i < bufferSize - Index; i++)
    printf("_");
  printf("\n");
}