#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include "arena.h"

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    SharedData *data = (SharedData*) shmat(shmid, NULL, 0);
    data->count = 0;
    data->waiting_player = -1;
    pthread_mutex_init(&data->lock, NULL);
    printf("Orion is ready (PID: %d)\n", getpid());

    while (1) {
        sleep(5);
    }
    return 0;
}