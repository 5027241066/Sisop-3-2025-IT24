//hunter.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define MAX_HUNTERS 100
#define MAX_DUNGEONS 20

const char *SHM_HUNTER = "/hunter_shm";
const char *SHM_DUNGEON = "/dungeon_shm";
const char *SEM_NAME = "/hunter_sem";

typedef struct {
    char name[50];
    int level, exp, atk, hp, def;
    int banned;
    char key[50];
} Hunter;

typedef struct {
    char name[100];
    int min_level;
    int reward_atk, reward_hp, reward_def, reward_exp;
    char key[50];
} Dungeon;

Hunter *hunters;
Dungeon *dungeons;
sem_t *sem;

volatile bool notif_running = true;

void show_dungeons_by_level(Hunter *player) {
    printf("\n=== Dungeons Available (Level %d) ===\n", player->level);
    for (int i = 0; i < MAX_DUNGEONS; ++i) {
        if (strlen(dungeons[i].name) > 0 && dungeons[i].min_level <= player->level) {
            printf("- %s | Min Lv: %d | ATK+%d | HP+%d | DEF+%d | EXP+%d | Key: %s\n",
                   dungeons[i].name, dungeons[i].min_level, dungeons[i].reward_atk,
                   dungeons[i].reward_hp, dungeons[i].reward_def, dungeons[i].reward_exp,
                   dungeons[i].key);
        }
    }
}

void raid_dungeon(Hunter *player) {
    char input_key[50];
    int found = -1;

    printf("\nEnter Dungeon Key to raid: ");
    scanf("%s", input_key);

    for (int i = 0; i < MAX_DUNGEONS; ++i) {
        if (strcmp(dungeons[i].key, input_key) == 0 && dungeons[i].min_level <= player->level) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Dungeon not found or level too low.\n");
        return;
    }

    Dungeon *dun = &dungeons[found];
    printf("\nYou raided '%s' successfully!\n", dun->name);
    printf("Rewards: +%d ATK | +%d HP | +%d DEF | +%d EXP\n",
           dun->reward_atk, dun->reward_hp, dun->reward_def, dun->reward_exp);

    player->atk += dun->reward_atk;
    player->hp += dun->reward_hp;
    player->def += dun->reward_def;
    player->exp += dun->reward_exp;

    if (player->exp >= 500) {
        player->level++;
        player->exp = 0;
        printf("Congratulations! You leveled up to Level %d!\n", player->level);
    }

    memset(dun, 0, sizeof(Dungeon));
}

void show_dungeon_notifications() {
    while (notif_running) {
        printf("\n=== Dungeon Notifications ===\n");
        for (int i = 0; i < MAX_DUNGEONS; ++i) {
            if (strlen(dungeons[i].name) > 0) {
                printf("Dungeon: %s | Min Lv: %d | Key: %s\n", 
                       dungeons[i].name, dungeons[i].min_level, dungeons[i].key);
            }
        }
        sleep(3);
    }
}

void hunter_battle(Hunter *player) {
    printf("\n=== Hunter's Battle ===\n");
    printf("Opponents Lists:\n");

    int target_indices[MAX_HUNTERS];
    int count = 0;

    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strlen(hunters[i].name) > 0 && strcmp(hunters[i].key, player->key) != 0 && !hunters[i].banned) {
            printf("%d. %s (Level %d | ATK: %d, HP: %d, DEF: %d, Total: %d) - Key: %s\n", count + 1,
                   hunters[i].name, hunters[i].level, hunters[i].atk, hunters[i].hp, hunters[i].def,
                   hunters[i].atk + hunters[i].hp + hunters[i].def, hunters[i].key);
            target_indices[count++] = i;
        }
    }

    if (count == 0) {
        printf("No opponents available.\n");
        return;
    }

    char target_key[50];
    printf("Enter Enemy Hunter Key: ");
    scanf("%s", target_key);

    sem_wait(sem);
    int target_index = -1;
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strcmp(hunters[i].key, target_key) == 0 && strcmp(hunters[i].key, player->key) != 0 && !hunters[i].banned) {
            target_index = i;
            break;
        }
    }

    if (target_index == -1) {
        printf("Opponents not valid.\n");
        sem_post(sem);
        return;
    }

    Hunter *opponent = &hunters[target_index];
    int player_power = player->atk + player->hp + player->def;
    int opponent_power = opponent->atk + opponent->hp + opponent->def;

    printf("Fighting against %s...\n", opponent->name);
    printf("%s (Power: %d) vs %s (Power: %d)\n", player->name, player_power, opponent->name, opponent_power);

    if (player_power >= opponent_power) {
        printf("You win!\n");
        player->atk += opponent->atk;
        player->hp += opponent->hp;
        player->def += opponent->def;
        memset(opponent, 0, sizeof(Hunter));
    } else {
        printf("You lose!\n");
        opponent->atk += player->atk;
        opponent->hp += player->hp;
        opponent->def += player->def;
        memset(player, 0, sizeof(Hunter));
        sem_post(sem);
        printf("Eliminated.\n");
        exit(0);
    }

    sem_post(sem);
}


void user_menu(Hunter *player) {
    int choice;
    do {
        printf("\n=== %s MENU ===\n", player->name);
        printf("1. Dungeon list\n");
        printf("2. Dungeon Raid\n");
        printf("3. Notifications\n");
        printf("4. Hunter's Battle\n");  
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                show_dungeons_by_level(player);
                break;
            case 2:
                raid_dungeon(player);
                break;
            case 3:
                printf("Notifications activated!\n");
                notif_running = true;
                show_dungeon_notifications(); 
                break;
            case 4:
                hunter_battle(player);  
                break;
            case 0:
                printf("Logging out...\n");
                notif_running = false;
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }
    } while (choice != 0);
}


void register_hunter() {
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strlen(hunters[i].name) == 0) {
            printf("Enter Hunter Name: ");
            scanf("%s", hunters[i].name);
            hunters[i].level = 1;
            hunters[i].exp = 0;
            hunters[i].atk = 10;
            hunters[i].hp = 100;
            hunters[i].def = 5;
            hunters[i].banned = 0;
            snprintf(hunters[i].key, 50, "HNT%03d", rand() % 1000);
            printf("Registered! Key: %s\n", hunters[i].key);
            sem_post(sem);
            return;
        }
    }
    printf("Hunter list full!\n");
    sem_post(sem);
}

void login_hunter() {
    char key[50];
    printf("Enter Hunter Key: ");
    scanf("%s", key);
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strcmp(hunters[i].key, key) == 0) {
            if (hunters[i].banned) {
                printf("You are banned.\n");
                sem_post(sem);
                return;
            }
            printf("Welcome, %s! Level %d\n", hunters[i].name, hunters[i].level);
            Hunter *player = &hunters[i];
            sem_post(sem);
            user_menu(player);
            return;
        }
    }
    sem_post(sem);
    printf("Invalid key.\n");
}

int main() {
    srand(time(NULL));

    int shm_fd1 = shm_open(SHM_HUNTER, O_RDWR, 0666);
    int shm_fd2 = shm_open(SHM_DUNGEON, O_RDWR, 0666);
    if (shm_fd1 < 0 || shm_fd2 < 0) {
        perror("Run system first.");
        exit(1);
    }

    hunters = mmap(NULL, sizeof(Hunter) * MAX_HUNTERS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    dungeons = mmap(NULL, sizeof(Dungeon) * MAX_DUNGEONS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
    sem = sem_open(SEM_NAME, 0);

    int choice;
    do {
        printf("\n1. Register\n2. Login\n0. Exit\nChoice: ");
        scanf("%d", &choice);
        if (choice == 1) register_hunter();
        else if (choice == 2) login_hunter();
    } while (choice != 0);

    munmap(hunters, sizeof(Hunter) * MAX_HUNTERS);
    munmap(dungeons, sizeof(Dungeon) * MAX_DUNGEONS);
    close(shm_fd1);
    close(shm_fd2);
    sem_close(sem);
    return 0;
}
