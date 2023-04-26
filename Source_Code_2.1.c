#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define SIZE 3

void *print(char *);

struct semData {
    sem_t semID;
    char *str;
    struct semData *next;
};

int main(void) {

    clock_t begin = clock();

    // Initialize threads and structs.
    pthread_t *thr = (pthread_t *) malloc(sizeof(pthread_t) * SIZE);
    struct semData *Data = (struct semData *) malloc(sizeof(struct semData) * SIZE);

    // Initialize struct data.
    sem_init(&Data[0].semID, 0, 1);
    sem_init(&Data[1].semID, 0, 0);
    sem_init(&Data[2].semID, 0, 0);
    Data[0].str = "<one>";
    Data[1].str = "<two>";
    Data[2].str = "<three>";
    Data[0].next = &Data[1];
    Data[1].next = &Data[2];
    Data[2].next = &Data[0];

    // Create threads that call function print and pass structs as data.
    if (pthread_create(&thr[0], NULL, (void *) &print, &Data[0]) != 0) {
        printf("Error in thread creation.\n");
        exit(1);
    }
    if (pthread_create(&thr[1], NULL, (void *) &print, &Data[1]) != 0) {
        printf("Error in thread creation.\n");
        exit(1);
    }
    if (pthread_create(&thr[2], NULL, (void *) &print, &Data[2]) != 0) {
        printf("Error in thread creation.\n");
        exit(1);
    }

    // Join all threads.
    if (pthread_join(thr[0], NULL)) {
        printf("Error joining thread.");
        exit(2);
    }
    if (pthread_join(thr[1], NULL)) {
        printf("Error joining thread.");
        exit(2);
    }
    if (pthread_join(thr[2], NULL)) {
        printf("Εrror joining thread.");
        exit(2);
    }

    // Free allocated memory from heap.
    free(thr);
    free(Data);

    clock_t end = clock();
    printf("\nThe elapsed time is %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);
    return 0;
}

// Function that prints <one> <two> <three>.
void *print(char *temp) {
    struct semData *localData = (struct semData *) temp;
    for (int i = 0; i < 100; i++) {
        sem_wait(&localData->semID);
        printf("%s", localData->str);
        sem_post(&localData->next->semID);
    }
}
