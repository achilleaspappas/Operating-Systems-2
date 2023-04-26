#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char *send_message = "hello from your first child\n";

int main(void) {

    int fd[2], size;
    int pid1, pid2, pid3, pid4, pid5, pid;
    char recv_message[100];
    
    /**
     * Initialize the pipe.
     * If pipe() returns -1 then pipe couldn't be created.
     * Exit with code 1 for pipe error.
    */
    if(pipe(fd)==-1) {
        perror("Pipe NOT created.\n");
        exit(1);
    }

    /** 
     * Forking the process P0 to create P1. 
     * If fork() returns -1 then the fork couldn't be executed.
     * Exit with code 2 for fork error.
    */
    if((pid1=fork())==-1) {
        perror("Fork NOT executed.\n");
        exit(2);
    }

    if(pid1==0) {
        /** 
         * Process P1. 
         * Close the reading end of the pipe.
         * Writing the string to pipe.
         * Close the writing end of pipe.
         * Exit with code 0.
        */
        printf("P1 -> PID: %d, PPID: %d\n", getpid(), getppid());
        close(fd[0]);
        write(fd[1], send_message, strlen(send_message)+1);
        close(fd[1]);
    }   
    else {
        /** 
         * Process P0 
         * Close the writing end of the pipe.
         * Line_Write to stdout.
         * Wait for P1 to write to pipe so P0 can read.
         * Close reading end of pipe.
        */
        close(fd[1]);
        size = read(fd[0], recv_message, sizeof(recv_message));
        close(fd[0]);
        write(1, recv_message, size);

        /**
         * Forking the process P0 to create P2. 
         * If fork() returns -1 then the fork couldn't be executed.
         * Exit with code 2 for fork error. 
        */ 
        if((pid2=fork())==-1) {
            perror("Fork NOT executed.\n");
            exit(2);
        }

        if(pid2==0){
            /**
             * Process P2
             * Forking the process P2 to create P3. 
             * If fork() returns -1 then the fork couldn't be executed.
             * Exit with code 2 for fork error. 
            */ 
            if((pid3=fork())==-1) {
                perror("Fork NOT executed.\n");
                exit(2);
            }
            
            if(pid3==0) {
                /* Process P3 */
                printf("P3 -> PID %d, PPID %d\n", getpid(), getppid());
                return(5);
            }
            else {
                /** 
                 * Process P2
                 * Waiting signal from P3 completion 
                */
                wait(&pid);

                /**
                 * Forking the process P2 to create P4. 
                 * If fork() returns -1 then the fork couldn't be executed.
                 * Exit with code 2 for fork error. 
                */ 
                if((pid4=fork())==-1) {
                    perror("Fork NOT executed.\n");
                    exit(2);
                }
                
                if(pid4==0){
                    /* Process P4 */
                    printf("P4 -> PID %d, PPID %d\n", getpid(), getppid());
                    return(5);
                }
                else {
                    /** 
                     * Process P2
                     * Waiting signal from P4 completion 
                    */
                    wait(&pid);

                    /**
                     * Forking the process P2 to create P5. 
                     * If fork() returns -1 then the fork couldn't be executed.
                     * Exit with code 2 for fork error. 
                    */ 
                    if((pid5=fork())==-1) {
                        perror("Fork NOT executed.\n");
                        exit(2);
                    }

                    if(pid5==0) {
                        /* Process P5 */
                        printf("P5 -> PID %d, PPID %d\n", getpid(), getppid());
                        return(5);
                    }
                    else {
                        /** 
                         * Process P2
                         * Waiting signal from P5 completion 
                        */
                        wait(&pid);
                        printf("P2 -> PID: %d, PPID: %d\n", getpid(), getppid());
                        exit(5);
                    }
                }
            }
        
        }
        else {
            /** 
             * Process P0
             * Waiting signal from P2 completion 
            */
            wait(&pid);
            wait(&pid);
            printf("P0 -> PID: %d, PPID: %d\n", getpid(), getppid());
            /**
             * Check if command execute or return exit code 3 for failure.
             * If command not executed execl returns -1.
            */
            if(execl("/bin/ps", "ps", "-f",NULL) == -1) { 
                exit(3); 
            }
        }
    }
}