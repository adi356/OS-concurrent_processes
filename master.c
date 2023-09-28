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

void terminationFunc(int signo) {
    printf("Received termination signal. Cleaning up processes\n");

    //third expression might need to be changed to ++i instead of i++ (?)
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (sharedData[i].inCritical) {
            //itoa not working properly, commenting out and using sprintf instead 
            //FILE* logFile = fopen(strcat(LOG_FILE_PREFIX, itoa(i + 1)), "a");
            char fileName[100];
            sprintf(fileName, "%s%d", LOG_FILE_PREFIX, i + 1);
            FILE* logFile = fopen(fileName, "a");

            time_t rawTime;
            time(&rawTime);
            fprintf(logFile, "Termination time: %s", ctime(&rawTime));
            fclose(logFile);
            printf("Terminating child process %d\n", i + 1);
            kill(i + 1, SIGTERM);
        }
    }

    //Block deallocates shared memory and display updated info
    system("ipcs -m");
    shmdt(sharedData);
    //should create shared memory segment, get identifier and then remove it
    shmctl(shmget(IPC_PRIVATE, sizeof(struct SharedData) * MAX_PROCESSES, IPC_CREAT | 0666), IPC_RMID, NULL);
    system("ipcs -m");

    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s -t <timeout_seconds> <number_of_processes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    //signal handlers
    signal(SIGTERM, terminationFunc);
    signal(SIGINT, terminationFunc);

    //handles command line input, makes sure number of processes generated is less than MAX_PROCESS
    int timeout = atoi(argv[2]);
    int numProcesses = atoi(argv[3]);
    numProcesses = (numProcesses > MAX_PROCESSES) ? MAX_PROCESSES : numProcesses;

    //code edited from stackoverflow
    //setup shared memory segment and obtain pointer to it
    //key is used for other processes that need to attach to the same shared memory segment
    key_t key = ftok("shared_memory_key", 1);
    int shmid = shmget(key, sizeof(struct SharedData) * MAX_PROCESSES, IPC_CREAT | 0666);
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
           //execlp("./slave", "slave", itoa(i + 1), NULL);
           char processNumberStr[10];
           sprintf(processNumberStr, "%d", i + 1);
           execlp("./slave", "slave", processNumberStr, NULL);
           //need to be more descriptive
           perror("execlp");
           exit(EXIT_FAILURE); 
        } else if (pid < 0) {
           //need to describe perror better
           perror("fork");
           exit(EXIT_FAILURE); 
        }
    }
}