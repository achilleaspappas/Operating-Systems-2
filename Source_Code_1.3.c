#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h> 

void* add(double*);
double result = 0.0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void) { 
    double time_spent = 0.0;
    
    /** p is the number of threads.
     * n is the number of elements.
     * All numbers are in input.txt.
     * n is calculated automatically based on the number 
     * of elements in input.txt.
    */

    /* Initialize */
    int p, n=0;
    
    printf("Give number of threads: ");
    scanf("%d", &p);

	clock_t begin = clock();
    /* Allocate dynamically memory for the elements. */
    double* a_array;
    a_array = (double*)malloc(sizeof(double));

    FILE *fp; // Create a pointer of type FILE.
    char buff[100]; // Create a buffer (char array).
    if((fp = fopen("input.txt", "r"))==NULL){ // Open the file and choose mode.
        printf("File not found.\n");
        exit(1);
    } 
    while(1) {
        /** Read data and place them to the buffer 
         * If fp becomes EOF then reading data stops. 
        */
        fscanf(fp,"%s", buff);
        if(feof(fp))
            break;
        a_array = (double*)realloc(a_array, sizeof(double)*n+1);
        a_array[n] = strtof(buff, NULL);
        n++;
    }
    fclose(fp); // Close the file.

    /**
     * Calculate the size of based on number of elements and number of threads.
     * Because we need to have a not fixed number of threads
     * we allocate them dynamically based on p.
     * The same goes for the array that we will pass to threads. 
    */

    int size=n/p;
    pthread_t* thr_array;
    thr_array = (pthread_t*)malloc(sizeof(pthread_t)*p);
    
    double * arg_array;
    arg_array = (double*)malloc(sizeof(double)*size+1); // We create one more space so that we can store the size of the array.
    arg_array[0]=size; 
    
    

    int index=0;
    for(int i=0; i<p; i++){

        /* Copy elements from a_array to arg_array so we can pass it to threads. */
        for(int j=0; j<size; j++) 
            arg_array[j+1]=a_array[j+index];
        index+=size;

        // Create thread and then join thread.
        if(pthread_create(thr_array+i, NULL, (void*)&add, arg_array) != 0) {
            printf("Error in thread creation.\n");
            exit(3);
        }
        if (pthread_join(*(thr_array+i), NULL)){
            printf("Εrror joining thread.");
            exit(2);
        }
    }
  
    printf("Result: %lf\n", result);
    
    /* Free the allocated memory. */
    free(arg_array);
    free(a_array);
    free(thr_array);
    
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds\n", time_spent);
    return 0;
}

/* This function will be executed from threads. */
void* add(double* arg_array){
    double local_sum=0;
    int local_size=(int)arg_array[0]; // Cast... just because you know..., its C.
    for(int k=1; k<=local_size; k++) 
        local_sum+=pow(arg_array[k],2); // Use the power function to calculate the power of 2.
    pthread_mutex_lock(&mutex); // Lock because result will be used from all threads.
    result+=local_sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}
