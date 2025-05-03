#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER 8192
#define INPUT_FOLDER "client/secrets"
#define OUTPUT_FOLDER "client"

void binary_ke_hex(const unsigned char *data, int len, char *output) {
    for (int i = 0; i < len; i++) {
        sprintf(output + i * 2, "%02x", data[i]);
    }
    output[len * 2] = '\0';
}

void reverse(char *str) {
    int i, j;
    char temp;
    for (i = 0, j = strlen(str)-1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int connect_ke_server() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Gagal connect ke server");
        exit(1);
    }

    return sock;
}


void kirim_file() {
    char filename[100];
    printf("Masukkan nama file: ");
    scanf("%s", filename);

    char path[256];
    snprintf(path, sizeof(path), "%s/%s", INPUT_FOLDER, filename);
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("Gagal buka file %s\n", path);
        return;
    }

    unsigned char buffer[MAX_BUFFER];
    int len = fread(buffer, 1, MAX_BUFFER, file);
    fclose(file);

    char packet[MAX_BUFFER + 256]; 
    snprintf(packet, sizeof(packet), "SEND|%s|", filename);
    memcpy(packet + strlen(packet), buffer, len);

    int sock = connect_ke_server();
    write(sock, packet, strlen(packet) + len);

    char response[MAX_BUFFER];
    int bytes = read(sock, response, sizeof(response)-1);
        response[bytes] = '\0';

    printf("Respon server: %s\n", response);
    close(sock);
}

void download_file() {
    char filename[100];
    printf("Masukkan nama file JPEG di server: ");
    scanf("%s", filename);

    char packet[512];
    snprintf(packet, sizeof(packet), "DOWNLOAD|%s|", filename);

    int sock = connect_ke_server();
    write(sock, packet, strlen(packet));

    unsigned char response[MAX_BUFFER];
    int bytes = read(sock, response, sizeof(response));

    if (bytes <= 0 || strncmp((char*)response, "ERROR", 5) == 0) {
        printf("Gagal download: %s\n", response);
        close(sock);
        return;
    }

    char outpath[256];
    snprintf(outpath, sizeof(outpath), "%s/%s", OUTPUT_FOLDER, filename);
    FILE *out = fopen(outpath, "wb");
    fwrite(response, 1, bytes, out);
    fclose(out);

    printf("File berhasil di-download ke %s\n", outpath);
    close(sock);
}

void log_exit() {
    FILE *log = fopen("../server/server.log", "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char waktu[100];
    strftime(waktu, sizeof(waktu), "%Y-%m-%d %H:%M:%S", t);
    
    fprintf(log, "[Client][%s]: [EXIT] [Client keluar]\n", waktu);
    fclose(log);
}


int main() {
    mkdir(INPUT_FOLDER, 0777);
    mkdir(OUTPUT_FOLDER, 0777);

    while (1) {
        printf("\n============== IMAGE CLIENT =============\n");
        printf("1. Kirim file teks ke server\n");
        printf("2. Download file JPEG dari server\n");
        printf("3. Exit\n");
        printf("Pilih opsi: ");

        int opsi;
        scanf("%d", &opsi);

        printf("\n=========================================\n");

        switch (opsi) {
            case 1:
                kirim_file();
                break;
            case 2:
                download_file();
                break;
            case 3:
                printf("Keluar...\n");
                log_exit();
                exit(0);
            default:
                printf("Opsi tidak valid.\n");
        }
    }

    return 0;
}
