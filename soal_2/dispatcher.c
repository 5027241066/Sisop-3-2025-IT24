#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>  
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

void tulis_log(const char *agen, const char *nama, const char *alamat) {
    FILE *log = fopen("delivery.log", "a");
    if (!log) return;

    time_t sekarang = time(NULL);
    struct tm *tm_info = localtime(&sekarang);
    char waktu[64];
    strftime(waktu, sizeof(waktu), "%d/%m/%Y %H:%M:%S", tm_info);

    fprintf(log, "[%s] [AGENT %s] Reguler package delivered to %s in %s\n",
            waktu, agen, nama, alamat);

    fclose(log);
}

int main(int argc, char *argv[]) {
    key_t key = ftok("delivery_agent.c", 123);
    int shmid = shmget(key, sizeof(shared_data), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    shared_data *data = (shared_data *)shmat(shmid, NULL, 0);
    if (data == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    if (argc < 2) {
        printf("Usage:\n");
        printf("./dispatcher -deliver [Name]\n");
        printf("./dispatcher -status [Name]\n");
        printf("./dispatcher -list\n");
        return 0;
    }

    if (strcmp(argv[1], "-deliver") == 0) {
        printf("============ Reguler Delivery ============\n\n");
        if (argc < 3) {
            printf("Enter the customer's name\n");
            return 0;
        }

        int ketemu = 0;
        for (int i = 0; i < data->jumlah_order; i++) {
            if (strcmp(data->daftar_order[i].nama, argv[2]) == 0 &&
                strcmp(data->daftar_order[i].jenis, "Reguler") == 0 &&
                strcmp(data->daftar_order[i].status, "PENDING") == 0) {

                strcpy(data->daftar_order[i].status, "DELIVERED");
                
                char *user = getenv("USER");  
                if (user == NULL) {
                    user = "Anomali";
                }
                strcpy(data->daftar_order[i].agen, user);
                
                printf("%s's order was successfully sent manually.\n", argv[2]);

                tulis_log(user, data->daftar_order[i].nama, data->daftar_order[i].alamat);

                ketemu = 1;
                break;
            }
        }

        if (!ketemu) {
            printf("Order not found or already shipped.\n");
        }
        printf("\n=============================================\n");

    } else if (strcmp(argv[1], "-status") == 0) {
        printf("=============== Order Status  ===============\n\n");
        if (argc < 3) {
            printf("Enter the customer's name.\n");
            return 0;
        }

        int ketemu = 0;
        for (int i = 0; i < data->jumlah_order; i++) {
            if (strcmp(data->daftar_order[i].nama, argv[2]) == 0) {
                if(strcmp(data->daftar_order[i].status, "PENDING") == 0) {
                    printf("Status for %s : %s\n", 
                        data->daftar_order[i].nama,
                        data->daftar_order[i].status);
                    ketemu = 1;
                    break;
                } else if (strcmp(data->daftar_order[i].status, "DELIVERED") == 0) {
                    printf("Status for %s : %s by Agent %s\n", 
                        data->daftar_order[i].nama,
                        data->daftar_order[i].status,
                        data->daftar_order[i].agen);
                    ketemu = 1;
                    break;
                }
                
            }
        }

        if (!ketemu) {
            printf("Order not found.\n");
        }
        printf("\n=============================================\n");

    } else if (strcmp(argv[1], "-list") == 0) {
        printf("============= List Semua Pesanan =================\n\n");
        for (int i = 0; i < data->jumlah_order; i++) {
            printf("%d. %s (%s) | %s\n",
                   i + 1,
                   data->daftar_order[i].nama,
                   data->daftar_order[i].jenis,
                   data->daftar_order[i].status);
        }
        printf("\n==================================================\n");

    } else {
        printf("Command not found. Use -deliver, -status, or -list.\n");
    }

    shmdt(data);
    return 0;
}
