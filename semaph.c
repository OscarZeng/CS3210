/*******************************************************************
* semaph.c
* Demonstrates using semaphores to synchromize Linux processes created with fork()
* Compile: gcc -pthread -o semaph semaph.c
* Run: ./semaph
* Partily adapted from https://stackoverflow.com/questions/16400820/c-how-to-use-posix-semaphores-on-forked-processes
*******************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    /*      loop variables          */
    int i;
    /*      shared memory key       */
    key_t shmkey;
    /*      shared memory id        */
    int shmid;
    /*      synch semaphore         */ /*shared */
    sem_t* sem;
    /*      fork pid                */
    pid_t pid;
    /*      shared variable         */ /*shared */
    int* p;
    /*      shared variable         */ /*shared */
    int* TheIndex;
    /*      shared variable         */ /*shared */
    int* producer_buffer;
    /*      buffer size             */
    int buffer_size = 10;
    /*      fork count              */
    unsigned int n;
    /*      semaphore value         */
    unsigned int value;

    /* initialize a shared variable in shared memory */
    shmkey = ftok("/dev/null", 5); /* valid directory name and a number */
    printf("shmkey for p = %d\n", shmkey);
    shmid = shmget(shmkey, sizeof(int)*(buffer_size+10), 0644 | IPC_CREAT);
    if (shmid < 0) { /* shared memory error check */
        perror("shmget\n");
        exit(1);
    }
    //Here we use the variable p as the sum of values
    p = (int*)shmat(shmid, NULL, 0); /* attach p to shared memory */
    *p = 0;
    printf("p=%d is allocated in shared memory.\n\n", *p);
    //Keep the record where the TheIndexex is
    TheIndex = (int*)shmat(shmid+2*sizeof(int), NULL, 0);
    *TheIndex = 0;
    printf("TheIndex=%d is allocated in shared memory.\n\n", *TheIndex);
    //The producer buffer
    producer_buffer = shmat(shmid+4*sizeof(int), NULL, 0);
    
    printf("producer_buffer is allocated in shared memory.\n\n");

    /********************************************************/

    //printf("How many children do you want to fork?\n");
    //printf("Fork count: ");
    //scanf("%u", &n);
    //Here the num of child processes should be 2(Producers)
    n = 2;

    //printf("What do you want the semaphore value to be?\n");
    //printf("Semaphore value: ");
    //scanf("%u", &value);
    //semaphores should be 1, only one process allowed in cirtical session
    value = 1;

    /* initialize semaphores for shared processes */
    sem = sem_open("pSem", O_CREAT | O_EXCL, 0644, value);
    /* name of semaphore is "pSem", semaphore is reached using this name */

    printf("semaphores initialized.\n\n");

    /* fork child processes */
    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            /* check for error      */
            sem_unlink("pSem");
            sem_close(sem);
            /* unlink prevents the semaphore existing forever */
            /* if a crash occurs during the execution         */
            printf("Fork error.\n");
        } else if (pid == 0)
            break; /* child processes */
    }

    /******************************************************/
    /******************   PARENT PROCESS   ****************/
    /******************************************************/
    if (pid != 0) {
        /* wait for all children to exit */
        /*while (pid = waitpid(-1, NULL, 0)) {
            if (errno == ECHILD)
                break;
        }

        printf("\nParent: All children have exited.\n");
        */

        while(1){
        sem_wait(sem); /* P operation */
        printf("  Consumer(%d) is in critical section.\n", i);
        if (*TheIndex < 10){
            //Generate a random number between 1 to 10
            int inputNum = 1+(rand()%10);
            producer_buffer[*TheIndex] = inputNum;
            *TheIndex = *TheIndex + 1;
            printf("producer process #%d added a number %d to the producer_buffer now with size: %d\n",i, inputNum, *TheIndex);
        }
        sleep(1);
        printf("  Consumer(%d) new value of *p=%d.\n", i, *p);
        sem_post(sem); /* V operation */
        }
        
        /* shared memory detach */
        shmdt(p);
        shmctl(shmid, IPC_RMID, 0);

        /* cleanup semaphores */
        sem_unlink("pSem");
        sem_close(sem);
        /* unlink prevents the semaphore existing forever */
        /* if a crash occurs during the execution         */
        exit(0);
    }

    /******************************************************/
    /******************   CHILD PROCESS   *****************/
    /******************************************************/
    else {
        //Producer will work forever
        while(1){
        sem_wait(sem); /* P operation */
        printf("  Producer(%d) is in critical section.\n", i);
        if (*TheIndex > 0){
            //Get the number and update the sum
            *TheIndex = *TheIndex - 1;
            int frontNumber = producer_buffer[*TheIndex];
            producer_buffer[*TheIndex] = 0;
            *p += frontNumber;
            printf("consumer process #%d added a number %d from the producer_buffer now with size: %d, consumer_sum is :%d\n",i, frontNumber, *TheIndex, *p);
        }
        sleep(1);
        printf("  Producer(%d) new value of *p=%d.\n", i, *p);
        sem_post(sem); /* V operation */
        }
    }
}
