#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>

#define MAKS_ORDER 50
#define MAKS_NAMA 50
#define MAKS_ALAMAT 50
#define MAKS_JENIS 10

typedef struct {
    char nama[MAKS_NAMA];
    char alamat[MAKS_ALAMAT];
    char jenis[MAKS_JENIS];
    char status[15]; 
    char agen[20]; 
} order;

typedef struct {
    order daftar_order[MAKS_ORDER];
    int jumlah_order;
} shared_data;

shared_data *data;
int shmid;

void muat_file_csv_ke_memori(const char *namafile) {
    FILE *fp = fopen(namafile, "r");
    if (!fp) {
        perror("Failed to open CSV file");
        exit(EXIT_FAILURE);
    }

    char baris[256];
    int is_header = 1;  

    while (fgets(baris, sizeof(baris), fp)) {
        if (is_header) {
            is_header = 0; 
            continue;
        }

        if (data->jumlah_order >= MAKS_ORDER) break;

        sscanf(baris, "%49[^,],%49[^,],%9[^\n]",
               data->daftar_order[data->jumlah_order].nama,
               data->daftar_order[data->jumlah_order].alamat,
               data->daftar_order[data->jumlah_order].jenis);

        strcpy(data->daftar_order[data->jumlah_order].status, "PENDING");
        strcpy(data->daftar_order[data->jumlah_order].agen, "-");
        data->jumlah_order++;
    }

    fclose(fp);
}

void tulis_log(const char *agen, const char *nama, const char *alamat) {
    FILE *log = fopen("delivery.log", "a");
    if (!log) return;

    time_t sekarang = time(NULL);
    struct tm *tm_info = localtime(&sekarang);
    char waktu[64];
    strftime(waktu, sizeof(waktu), "%d/%m/%Y %H:%M:%S", tm_info);

    fprintf(log, "[%s] [AGENT %s] Express package delivered to %s in %s\n",
            waktu, agen, nama, alamat);

    fclose(log);
}

void *agen_jalan(void *arg) {
    char *agen = (char *)arg;
    while (1) {
        for (int i = 0; i < data->jumlah_order; i++) {
            if (strcmp(data->daftar_order[i].jenis, "Express") == 0 &&
                strcmp(data->daftar_order[i].status, "PENDING") == 0) {

                strcpy(data->daftar_order[i].status, "DELIVERED");
                strcpy(data->daftar_order[i].agen, agen);

                tulis_log(agen, data->daftar_order[i].nama, data->daftar_order[i].alamat);
                sleep(1); 
            }
        }
        sleep(1); 
    }
    return NULL;
}

int main() {
    key_t key = ftok("delivery_agent.c", 123); 
    shmid = shmget(key, sizeof(shared_data), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    data = (shared_data *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    data->jumlah_order = 0;

    muat_file_csv_ke_memori("delivery_order.csv");

    pthread_t agen_a, agen_b, agen_c;
    pthread_create(&agen_a, NULL, agen_jalan, "A");
    pthread_create(&agen_b, NULL, agen_jalan, "B");
    pthread_create(&agen_c, NULL, agen_jalan, "C");

    pthread_join(agen_a, NULL);
    pthread_join(agen_b, NULL);
    pthread_join(agen_c, NULL);

    shmdt(data);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
