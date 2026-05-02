
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock;
float suhu_global = 0.0;

void* sensor(void* arg) {
    char* nama_sensor = (char*) arg;

    pthread_mutex_lock(&lock);

    // TODO: Generate suhu random antara 20 sampai 40 (dalam float)
    // Hint: ref https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
    // Source - https://stackoverflow.com/q/13408990

    float suhu = ((float)rand()/(float)(RAND_MAX)) * (20-40);


    // TODO: Update suhu_global dengan nilai suhu yang didapat
    suhu_global = suhu;
    // TODO: Print nama sensor dan suhu yang dibaca
    printf("[%s] Membaca suhu: %.2f C \n", nama_sensor, suhu);
    // Contoh output: "[Sensor A] Membaca suhu: 27.50 C"
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    pthread_t t1, t2, t3;
    pthread_mutex_init(&lock, NULL);

    // TODO: Buat 3 thread dengan nama "Sensor A", "Sensor B", "Sensor C"
    pthread_create(&t1, NULL, sensor, "Sensor A");
    pthread_create(&t2, NULL, sensor, "Sensor B");
    pthread_create(&t3, NULL, sensor, "Sensor C");
    // TODO: Join ketiga thread
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("Suhu akhir tercatat: %.2f C\n", suhu_global);
    pthread_mutex_destroy(&lock);
    return 0;
}

