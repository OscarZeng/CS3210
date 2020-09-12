#include <pthread.h>
#include <queue>          
#include <stdio.h>
#include <stdlib.h>
#define PRODUCERS 8
#define CONSUMERS 8

//For global varibles
char *seq1;
char *seq2;
int *mat[2];
int M, N;
/*
//Generates tuple to input into the key
void* producer(void* threadid)
{
    long pid;
    pid = (long) threadid;
    //When using the producer_buffer, it enters the critical session.
    while(1){
        if (ind < 10){
            //Generate a random number between 1 to 10
            int inputNum = 1+(rand()%10);
            producer_buffer[ind] = inputNum;
            ind = ind + 1;
            printf("producer thread #%d added a number %d to the producer_buffer now with size: %d\n",pid, inputNum, ind);
        }
        sleep(1);
    }

}

void* consumer(void* threadid)
{
    long cid;
    cid = (long) threadid;
    while(1){
        if (ind > 0){
            //Get the number and update the sum
            ind = ind - 1;
            int frontNumber = producer_buffer[ind];
            producer_buffer[ind] = 0;
            consumer_sum = consumer_sum + frontNumber;
            printf("consumer thread #%d added a number %d from the producer_buffer now with size: %d, consumer_sum is :%d\n",cid, frontNumber, ind, consumer_sum);
        }
        sleep(1);
    }

}
*/
int main(int argc, char *argv[]) {
    // Accepts filepaths to two input DNA sequences as command-line arguments
    if (argc != 3) {
        printf("Usage: [executable] [file 1] [file 2]\n");
        return 1;
    }

    // Validate both filepaths
    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");
    if (!file1 || !file2) {
        printf("Input files are not found!\n");
        return 1;
    }


    fscanf(file1, "%d", &M);
    fscanf(file2, "%d", &N);

    // Read two input sequences
    char *seq1 = (char*) malloc(M + 1);
    char *seq2 = (char*) malloc(N + 1);
    fscanf(file1, "%s", seq1);
    fscanf(file2, "%s", seq2);
    fclose(file1);
    fclose(file2);
/*
    //Before this line is file reading part
    pthread_t producer_threads[PRODUCERS];
    pthread_t consumer_threads[CONSUMERS];
    long producer_threadid[PRODUCERS];
    long consumer_threadid[CONSUMERS];
*/
    // Initialise two rows of the matrix with (M + 1) rows x (N + 1) columns
    
    mat[0] = calloc(N + 1, sizeof(int));
    mat[1] = malloc((N + 1) * sizeof(int));
    mat[1][0] = 0;

    int old = 0;
    int new = 1;
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            if (seq1[i - 1] == seq2[j - 1]) {
                mat[new][j] = mat[old][j - 1] + 1;
            } else {
                int left = mat[new][j - 1];
                int down = mat[old][j];
                mat[new][j] = left > down ? left : down;
            }
            printf("%d   ", mat[i][j]);
        }
        // Swap the previous and current row
        old = old ^ new;
        new = new ^ old;
        old = old ^ new;
        printf("\n");
    }

    printf("%d", mat[old][N]);

    free(seq1);
    free(seq2);
    free(mat[0]);
    free(mat[1]);
};
