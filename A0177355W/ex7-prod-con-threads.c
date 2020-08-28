//Name: Zeng Hao Student Number: A0177355W
/*******************************************************************
* prod-con-threads.c
* Producer Consumer With C
* Compile: gcc -pthread -o prodcont prod-con-threads.c
* Run: ./prodcont
*******************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define PRODUCERS 2
#define CONSUMERS 1

//The buffer can only have 10 numbers inside
int ind = 0;
int producer_buffer[10] = {0,0,0,0,0,0,0,0,0,0};
int consumer_sum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//Generates a random number between 1 and 10
void* producer(void* threadid)
{
    long pid;
    pid = (long) threadid;
    //When using the producer_buffer, it enters the critical session.
    while(1){
        pthread_mutex_lock(&lock);
        if (ind < 10){
            //Generate a random number between 1 to 10
            int inputNum = 1+(rand()%10);
            producer_buffer[ind] = inputNum;
            ind = ind + 1;
            printf("producer thread #%d added a number %d to the producer_buffer now with size: %d\n",pid, inputNum, ind);
        }
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

}

void* consumer(void* threadid)
{
    long cid;
    cid = (long) threadid;
    while(1){
        pthread_mutex_lock(&lock);
        if (ind > 0){
            //Get the number and update the sum
            ind = ind - 1;
            int frontNumber = producer_buffer[ind];
            producer_buffer[ind] = 0;
            consumer_sum = consumer_sum + frontNumber;
            printf("consumer thread #%d added a number %d from the producer_buffer now with size: %d, consumer_sum is :%d\n",cid, frontNumber, ind, consumer_sum);
        }
        pthread_mutex_unlock(&lock);
        sleep(1);
    }

}

int main(int argc, char* argv[])
{
    pthread_t producer_threads[PRODUCERS];
    pthread_t consumer_threads[CONSUMERS];
    long producer_threadid[PRODUCERS];
    long consumer_threadid[CONSUMERS];

    int rc;
    long t1, t2;
    for (t1 = 0; t1 < PRODUCERS; t1++) {
        int tid = t1;
        producer_threadid[tid] = tid;
        printf("creating producer %d\n", tid);
        rc = pthread_create(&producer_threads[tid], NULL, producer,
            (void*)producer_threadid[tid]);
        pthread_join(producer_threadid[tid],NULL);
        if (rc) {
            printf("Error: Return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t2 = 0; t2 < CONSUMERS; t2++) {
        int tid = t2;
        consumer_threadid[tid] = tid;
        printf("creating consumer %d\n", tid);
        rc = pthread_create(&consumer_threads[tid], NULL, consumer,
            (void*)consumer_threadid[tid]);
        pthread_join(consumer_threadid[tid],NULL);
        if (rc) {
            printf("Error: Return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    //This sleep make sure that the final result will be output at last
    sleep(2);
    printf("### consumer_sum final value = %d ###\n",
        consumer_sum);
    pthread_exit(NULL);
}
