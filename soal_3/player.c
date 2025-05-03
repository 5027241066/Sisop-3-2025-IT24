#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void tampilkanMenu() {
    printf("\n===== Dungeon Main Menu =====\n");
    printf("1. Show Player Stats\n");
    printf("2. Shop\n");
    printf("3. View Inventory\n");
    printf("4. Battle Mode\n");
    printf("5. Exit\n");
    printf("Pilih menu: ");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    char pilihan[10];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Gagal terhubung ke server.\n");
        return -1;
    }

    printf("Terhubung ke dungeon server!\n");

    while (1) {
        tampilkanMenu();
        fgets(pilihan, sizeof(pilihan), stdin);
        pilihan[strcspn(pilihan, "\n")] = 0;

        if (strcmp(pilihan, "5") == 0) {
            printf("Keluar dari dungeon.\n");
            break;
        }

        send(sock, pilihan, strlen(pilihan), 0);

        char response[1024] = {0};
        read(sock, response, sizeof(response));
        printf("\n[Server Response]\n%s\n", response);
    }

    close(sock);
    return 0;
}
