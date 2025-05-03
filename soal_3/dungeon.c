#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_WEAPONS 10

typedef struct {
    char name[32];
    int damage;
    char passive[64];
} Weapon;

typedef struct {
    int gold;
    Weapon inventory[MAX_WEAPONS];
    int inventory_count;
    Weapon *active_weapon;
    int enemies_defeated;
} PlayerStatus;

void load_player(PlayerStatus *player) {
    FILE *file = fopen("player_data.txt", "r");
    if (!file) return;

    fscanf(file, "%d\n", &player->gold);
    fscanf(file, "%d\n", &player->inventory_count);
    fscanf(file, "%d\n", &player->enemies_defeated);

    for (int i = 0; i < player->inventory_count; i++) {
        fscanf(file, "%s %d %s\n",
            player->inventory[i].name,
            &player->inventory[i].damage,
            player->inventory[i].passive);
    }

    int active_index;
    fscanf(file, "%d\n", &active_index);
    if (active_index >= 0 && active_index < player->inventory_count) {
        player->active_weapon = &player->inventory[active_index];
    }

    fclose(file);
}

void save_player(PlayerStatus *player) {
    FILE *file = fopen("player_data.txt", "w");
    if (!file) return;

    fprintf(file, "%d\n", player->gold);
    fprintf(file, "%d\n", player->inventory_count);
    fprintf(file, "%d\n", player->enemies_defeated);

    for (int i = 0; i < player->inventory_count; i++) {
        fprintf(file, "%s %d %s\n",
            player->inventory[i].name,
            player->inventory[i].damage,
            strlen(player->inventory[i].passive) > 0 ? player->inventory[i].passive : "-");
    }

    int active_index = -1;
    for (int i = 0; i < player->inventory_count; i++) {
        if (&player->inventory[i] == player->active_weapon) {
            active_index = i;
            break;
        }
    }

    fprintf(file, "%d\n", active_index);
    fclose(file);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[1024];
    char response[2048];

    PlayerStatus player;
    player.gold = 100;
    player.inventory_count = 0;
    player.active_weapon = NULL;
    player.enemies_defeated = 0;

    load_player(&player);
    printf("[SERVER] Client terhubung.\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(client_socket, buffer, sizeof(buffer));
        if (bytes <= 0) {
            printf("[SERVER] Client terputus.\n");
            break;
        }

        printf("[SERVER] Pesan: %s\n", buffer);
        memset(response, 0, sizeof(response));

        if (strcmp(buffer, "1") == 0) {
            if (player.active_weapon) {
                sprintf(response,
                    "===== Player Status =====\n"
                    "Gold            : %dG\n"
                    "Active Weapon   : %s\n"
                    "Base Damage     : %d\n"
                    "Passive         : %s\n"
                    "Enemies Defeated: %d\n"
                    "=========================",
                    player.gold, player.active_weapon->name,
                    player.active_weapon->damage,
                    strlen(player.active_weapon->passive) > 0 ? player.active_weapon->passive : "-",
                    player.enemies_defeated);
            } else {
                sprintf(response,
                    "===== Player Status =====\n"
                    "Gold            : %dG\n"
                    "Active Weapon   : (Belum ada)\n"
                    "Base Damage     : 5\n"
                    "Passive         : -\n"
                    "Enemies Defeated: %d\n"
                    "=========================",
                    player.gold, player.enemies_defeated);
            }
        }

        else if (strcmp(buffer, "2") == 0) {
            strcpy(response,
                "===== Weapon Shop =====\n"
                "1. Sword  - 50G  - Damage: 10\n"
                "2. Bow    - 60G  - Damage: 8  (Passive: Poison)\n"
                "3. Axe    - 70G  - Damage: 12\n"
                "4. Staff  - 90G  - Damage: 7  (Passive: Heal)\n"
                "5. Dagger - 40G  - Damage: 6\n"
                "Pilih senjata [1–5], atau 0 untuk batal:");
            send(client_socket, response, strlen(response), 0);

            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            int choice = atoi(buffer);

            Weapon new_weapon;
            switch (choice) {
                case 1: strcpy(new_weapon.name, "Sword"); new_weapon.damage = 10; strcpy(new_weapon.passive, ""); break;
                case 2: strcpy(new_weapon.name, "Bow"); new_weapon.damage = 8; strcpy(new_weapon.passive, "Poison"); break;
                case 3: strcpy(new_weapon.name, "Axe"); new_weapon.damage = 12; strcpy(new_weapon.passive, ""); break;
                case 4: strcpy(new_weapon.name, "Staff"); new_weapon.damage = 7; strcpy(new_weapon.passive, "Heal"); break;
                case 5: strcpy(new_weapon.name, "Dagger"); new_weapon.damage = 6; strcpy(new_weapon.passive, ""); break;
                default:
                    strcpy(response, "Pembelian dibatalkan atau pilihan tidak valid.");
                    send(client_socket, response, strlen(response), 0);
                    continue;
            }

            int price = 50 + (choice - 1) * 10;
            if (player.gold >= price) {
                player.inventory[player.inventory_count++] = new_weapon;
                player.gold -= price;
                sprintf(response, "Berhasil membeli %s! Sisa gold: %dG", new_weapon.name, player.gold);
            } else {
                strcpy(response, "Uang tidak cukup untuk membeli senjata.");
            }
        }

        else if (strcmp(buffer, "3") == 0) {
            if (player.inventory_count == 0) {
                strcpy(response, "Inventory kosong.");
                send(client_socket, response, strlen(response), 0);
                continue;
            }

            strcpy(response, "===== Inventory =====\n");
            for (int i = 0; i < player.inventory_count; i++) {
                char item[128];
                if (strlen(player.inventory[i].passive) > 0) {
                    sprintf(item, "%d. %s (Damage: %d, Passive: %s)\n", i+1,
                        player.inventory[i].name, player.inventory[i].damage, player.inventory[i].passive);
                } else {
                    sprintf(item, "%d. %s (Damage: %d)\n", i+1,
                        player.inventory[i].name, player.inventory[i].damage);
                }
                strcat(response, item);
            }
            strcat(response, "Pilih nomor senjata untuk equip, atau 0 untuk batal:");
            send(client_socket, response, strlen(response), 0);

            memset(buffer, 0, sizeof(buffer));
            read(client_socket, buffer, sizeof(buffer));
            int select = atoi(buffer);

            if (select >= 1 && select <= player.inventory_count) {
                player.active_weapon = &player.inventory[select - 1];
                sprintf(response, "Senjata aktif sekarang: %s", player.active_weapon->name);
            } else {
                strcpy(response, "Tidak jadi equip senjata.");
            }
        }
          
        else if (strcmp(buffer, "4") == 0) {
            int enemy_hp = 50 + rand() % 151;
            int poison_turns = 0;
            int heal_amount = 0;

            while (1) {
                sprintf(response,
                    "Musuh muncul dengan %d HP!\nKetik 'attack' untuk menyerang atau 'exit' untuk keluar battle mode.",
                    enemy_hp);
                send(client_socket, response, strlen(response), 0);

                memset(buffer, 0, sizeof(buffer));
                read(client_socket, buffer, sizeof(buffer));

                if (strcmp(buffer, "exit") == 0) {
                    strcpy(response, "Keluar dari Battle Mode.");
                    break;
                } else if (strcmp(buffer, "attack") != 0) {
                    strcpy(response, "Input tidak valid! Gunakan 'attack' atau 'exit'.");
                    send(client_socket, response, strlen(response), 0);
                    continue;
                }

                int base_damage = player.active_weapon ? player.active_weapon->damage : 5;
                int modifier = rand() % 3;
                int damage = base_damage + modifier;

                int is_critical = rand() % 100 < 25;
                if (is_critical) damage *= 2;

                int poison_damage = 0;
                if (poison_turns > 0) {
                    poison_damage = 5;
                    poison_turns--;
                }

                char passive_log[128] = "";
                if (player.active_weapon && strlen(player.active_weapon->passive) > 0) {
                    if (strcmp(player.active_weapon->passive, "Poison") == 0) {
                        poison_turns = 2;
                        strcpy(passive_log, "\nPassive aktif: Musuh terkena poison selama 2 turn.");
                    } else if (strcmp(player.active_weapon->passive, "Heal") == 0) {
                        heal_amount = 10 + rand() % 11;
                        sprintf(passive_log, "\nPassive aktif: Kamu sembuh sebesar %d HP (simulasi).", heal_amount);
                    }
                }

                int total_damage = damage + poison_damage;
                enemy_hp -= total_damage;

                if (enemy_hp <= 0) {
                    int gold_reward = 10 + rand() % 21;
                    player.gold += gold_reward;
                    player.enemies_defeated++;
                    enemy_hp = 50 + rand() % 151;

                    sprintf(response,
                        "Seranganmu membunuh musuh!\n"
                        "Damage total: %d (%s%s)\n"
                        "Reward: %dG\n"
                        "Total musuh dikalahkan: %d\n\n"
                        "Musuh baru muncul dengan %d HP.",
                        total_damage,
                        is_critical ? "Critical" : "Normal",
                        poison_damage > 0 ? " + Poison" : "",
                        gold_reward, player.enemies_defeated, enemy_hp);
                } else {
                    sprintf(response,
                        "Kamu menyerang musuh: %d damage %s\n"
                        "Sisa HP musuh: %d%s%s",
                        total_damage,
                        is_critical ? "(Critical!)" : "",
                        enemy_hp,
                        poison_damage > 0 ? "\nPoison memberikan 5 damage tambahan." : "",
                        passive_log);
                }

                send(client_socket, response, strlen(response), 0);
            }
        }
        else {
            strcpy(response, "Pilihan tidak dikenal. Masukkan 1–5.");
        }

        send(client_socket, response, strlen(response), 0);
    }

    save_player(&player);
    close(client_socket);
    free(arg);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    srand(time(NULL));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("[SERVER] Menunggu koneksi di port %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        int *client_sock = malloc(sizeof(int));
        *client_sock = new_socket;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}

