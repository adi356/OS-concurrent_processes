/**
 * @file master.c
 * @author Andrew Irvine (adi356@umsystem.edu)
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include "config.h"

//shared data struct - not sure if this is correct implementation
//still testing
struct SharedData {
    //turn variable to make sure each process has a unique turn
    int turn;
    int inCritical;
};

struct SharedData* sharedData;
int shmid;

void terminationFunc(int signo) {
    printf("Received termination signal. Cleaning up processes\n");

    //deallocate shared memory
    shmdt(sharedData);
    shmctl(shmid, IPC_RMID, NULL);
    //shmctl(shmget(ftok("shared_memory_key", 1), sizeof(struct SharedData) * MAX_PROCESSES, IPC_CREAT | 0666), IPC_RMID, NULL);
    
    //print termination time
    time_t t;
    time(&t);
    printf("Terminated at: %s\n", ctime(&t));

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
  
    //signal handlers
    signal(SIGTERM, terminationFunc);
    signal(SIGINT, terminationFunc);

    //check command line input
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -t ss n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //timeout try to different ways
    //way 1
    // int timeout = (argc > 2) ? atoi(argv[2]) : DEFAULT_TIMEOUT;
    // int numProcesses = (argc > 3) ? atoi(argv[3]) : MAX_PROCESSES;
    // numProcesses = (numProcesses > MAX_PROCESSES) ? MAX_PROCESSES : numProcesses;
    //way 2
    int timeout = atoi(argv[2]);
    int numProcesses = atoi(argv[3]);
    numProcesses = (numProcesses > MAX_PROCESSES) ? MAX_PROCESSES : numProcesses;

    //alocate shared memory and initialize
    key_t key = ftok("shared_memory_key", 1);
    shmid = shmget(key, sizeof(struct SharedData) * MAX_PROCESSES, IPC_CREAT | 0666);
    sharedData = (struct SharedData*)shmat(shmid, NULL, 0);

    //initialize shared memory data for each process
    for (int i = 0; i < numProcesses; ++i) {
        sharedData[i].turn = i;
        sharedData[i].inCritical = 0;
    }

    //create the multiple child processes using fork
    for (int i = 0; i < numProcesses; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
           execlp("./slave", "slave", argv[1], NULL);
           //TODO: write more descriptive error
           perror("execlp");
           exit(EXIT_FAILURE);
        } else if (pid < 0) {
            //TODO: write more descriptive error
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    sleep(timeout);

    //terminate child processes
    for (int i = 0; i < numProcesses; ++i) {
        kill(0, SIGTERM);
        usleep(5000); //delay between signals to each child process
    }

    //wait for child processes to terminate
    int status;
    while (wait(&status) > 0);

    //deallocate shared memory
    shmdt(sharedData);
    //shmctl(shmget(key, sizeof(struct SharedData) * MAX_PROCESSES, IPC_CREAT | 0666), IPC_RMID, NULL);
    shmctl(shmid, IPC_RMID, NULL);

    //print time
    time_t t;
    time(&t);
    printf("Terminated at: %s\n", ctime(&t));

    return 0;
}