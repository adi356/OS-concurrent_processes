#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

struct SharedData {
    int turn;
    int inCritical;
};

struct SharedData* sharedData;

void critical_section(int processNumber) {
    //create message format HH:MM:SS (?)
    time_t t;
    time(&t);
    char message[50];
    strftime(message, sizeof(message), "%H:%M:%S", localtime(&t));
    printf("%s File modified by process number %d\n", message, processNumber);
}

void entry_section (int processNumber) {
    printf("Process %d entering critical section. \n", processNumber);
    sharedData[processNumber - 1].inCritical = 1;
}

void exit_section (int processNumber) {
    printf("Process %d exiting critical section. \n", processNumber);
    sharedData[processNumber - 1].inCritical = 0;
}

void remainder_section() {
    //sleep for random amount of time between 1 and 3 seconds
    sleep(rand() % 3 + 1);
}

int main(int argc, char* argv[]) {
    int processNumber = atoi(argv[1]);

    key_t key = ftok("shared_memory_key", 1);
    int shmid = shmget(key, sizeof(struct SharedData) * MAX_PROCESSES, 0666);
    sharedData = (struct SharedData*)shmat(shmid, NULL, 0);

    for (int i = 0; i < 5; ++i) {
        entry_section(processNumber);
        sleep(rand() % 3 + 1);
        critical_section(processNumber);
        sleep(rand() % 3 + 1);
        exit_section(processNumber);
    }

    shmdt (sharedData);

    return 0;
}