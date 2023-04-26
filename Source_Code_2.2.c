#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barr;

void *calculateMax(void *tmp);

int maxValue = 0;
int **arrayOne;
int **arrayTwo;

struct thrData {
    int firstElem, lastElem, lineLength;
};

int main() {

    clock_t begin = clock();

    int n, p, size;
    pthread_t *thr;

    // Open file
    FILE *myFile;
    myFile = fopen("input.txt", "r");

    // Read data
    printf("Give dimension: ");
    scanf("%d", &n);

    printf("Give threads: ");
    scanf("%d", &p);

    size = n / p;

    // Initialize thread array
    thr = (pthread_t *) malloc(sizeof(pthread_t) * p);

    // Initialize the first array
    arrayOne = (int **) malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        arrayOne[i] = (int *) malloc(sizeof(int) * n);
        printf("\n");
        for (int j = 0; j < n; j++) {
            fscanf(myFile, "%d", &arrayOne[i][j]);
            printf("%d", arrayOne[i][j]);
            printf("\t");
        }
    }

    // Initialize the second array
    arrayTwo = (int **) malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        arrayTwo[i] = (int *) malloc(sizeof(int) * n);
    }

    // Create struct array
    struct thrData Data[p];

    // Initialize barrier
    pthread_barrier_init(&barr, NULL, p);

    // Get the first element, the last element and the line length
    for (int i = 0; i < p; i++) {
        Data[i].firstElem = i * size;
        Data[i].lastElem = (i + 1) * size;
        Data[i].lineLength = n;
        // Create threads
        pthread_create(&thr[i], NULL, calculateMax, &Data[i]);
    }

    // Join threads
    for (int i = 0; i < p; i++)
        pthread_join(thr[i], NULL);

    // Print max value
    printf("\n");
    printf("\n");
    printf("Max value: %d", maxValue);
    printf("\n");

    // Print second array
    for (int i = 0; i < n; i++) {
        printf("\n");
        for (int j = 0; j < n; j++) {
            printf("%d", arrayTwo[i][j]);
            printf("\t");
        }
    }

    clock_t end = clock();
    printf("\nThe elapsed time is %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
    return 0;
}

// Calculate every line's max value compare it to the global max value
void *calculateMax(void *temp) {
    struct thrData *localData = (struct thrData *) temp;
    int local_max = 0;
    for (int i = localData->firstElem; i < localData->lastElem; i++) {
        for (int j = 0; j < localData->lineLength; j++) {
            if (arrayOne[i][j] > local_max)
                local_max = arrayOne[i][j];
        }
    }

    // Lock global max value
    pthread_mutex_lock(&mutex);
    if (local_max > maxValue)
        maxValue = local_max;
    // Unlock global max value
    pthread_mutex_unlock(&mutex);
    // Barrier wait
    pthread_barrier_wait(&barr);

    // Calculate distance between global max value and current element
    for (int i = localData->firstElem; i < localData->lastElem; i++) {
        for (int j = 0; j < localData->lineLength; j++)
            arrayTwo[i][j] = maxValue - arrayOne[i][j];
    }

}
