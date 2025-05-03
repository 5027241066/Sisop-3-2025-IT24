
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define MAX_BUFFER 8192
#define DATABASE_FOLDER "/home/deefen/sisopmodul3/no1/server/database"
#define LOG_FILE "/home/deefen/sisopmodul3/no1/server/server.log"

void tulis_log(const char *sumber, const char *aksi, const char *info) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char waktu[100];
    strftime(waktu, sizeof(waktu), "%Y-%m-%d %H:%M:%S", t);
    
    fprintf(log, "[%s][%s]: [%s] [%s]\n", sumber, waktu, aksi, info);
    fclose(log);
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

int hex_ke_binary(const char *hex, unsigned char *output) {
    int panjang = strlen(hex);
    if (panjang % 2 != 0) {
        return -1;
    }

    for (int i = 0; i < panjang; i += 2) {
        sscanf(hex + i, "%2hhx", &output[i/2]);
    }
    return panjang / 2;
}

void proses_client(int sock) {
    char buffer[MAX_BUFFER];
    int bytes = read(sock, buffer, sizeof(buffer)-1);
    if (bytes <= 0) {
        close(sock);
        exit(1);
    }

    buffer[bytes] = '\0';
    char *cmd = strtok(buffer, "|");
    char *nama_file = strtok(NULL, "|");
    char *isi = strtok(NULL, "|");

    if (!cmd || !nama_file) {
        char *res = "ERROR|Format salah";
        write(sock, res, strlen(res));
        tulis_log("Server", "ERROR", "Format salah dari client");
        close(sock);
        exit(1);
    }

    if (strcmp(cmd, "SEND") == 0) {
        tulis_log("Client", "DECRYPT", nama_file);
    
        time_t now = time(NULL);
        char nama_jpeg[256];
        snprintf(nama_jpeg, sizeof(nama_jpeg), "%s/%ld.jpeg", DATABASE_FOLDER, now);
    
        FILE *fout = fopen(nama_jpeg, "wb");
        if (!fout) {
            char *res = "ERROR|Gagal simpan file";
            write(sock, res, strlen(res));
            tulis_log("Server", "ERROR", "Simpan file gagal");
            close(sock);
            exit(1);
        }
    
        fwrite(isi, 1, bytes - (isi - buffer), fout);
        fclose(fout);
    
        tulis_log("Server", "SAVE", strrchr(nama_jpeg, '/') + 1);
        char respon[128];
        snprintf(respon, sizeof(respon), "OK|%ld.jpeg", now);
        write(sock, respon, strlen(respon));
    }

    else if (strcmp(cmd, "DOWNLOAD") == 0) {
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", DATABASE_FOLDER, nama_file);
        FILE *fin = fopen(path, "rb");
        if (!fin) {
            char *res = "ERROR|File tidak ditemukan";
            write(sock, res, strlen(res));
            tulis_log("Server", "ERROR", "File tidak ditemukan");
            close(sock);
            exit(1);
        }

        unsigned char filedata[MAX_BUFFER];
        int len = fread(filedata, 1, MAX_BUFFER, fin);
        fclose(fin);

        tulis_log("Client", "DOWNLOAD", nama_file);
        tulis_log("Server", "UPLOAD", nama_file);

        write(sock, filedata, len);
    }

    else {
        char *res = "ERROR|Command tidak dikenal";
        write(sock, res, strlen(res));
        tulis_log("Server", "ERROR", "Command tidak dikenal");
    }

    close(sock);
    exit(0);
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    for (int x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
        close(x);
    }
}

int main() {
    daemonize();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    mkdir(DATABASE_FOLDER, 0777);

    while (1) {
        int client_sock = accept(server_fd, NULL, NULL);
        if (client_sock >= 0) {
            if (fork() == 0) {
                close(server_fd);
                proses_client(client_sock);
            }
            close(client_sock);
        }
    }

    return 0;
}
