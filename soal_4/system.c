//system.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

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
bool running = true;
bool continue_running = true;

void handle_sigint(int sig) {
    printf("\nSIGINT received, but the system continues running.\n");
}

void show_hunters() {
    printf("\n=== Hunter Info ===\n");
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strlen(hunters[i].name) > 0) {
            printf("Name: %s | Level: %d | EXP: %d | ATK: %d | HP: %d | DEF: %d | Key: %s | Status: %s\n",
                   hunters[i].name, hunters[i].level, hunters[i].exp, hunters[i].atk,
                   hunters[i].hp, hunters[i].def, hunters[i].key,
                   hunters[i].banned ? "BANNED" : "ACTIVE");
        }
    }
    sem_post(sem);
}

void show_dungeons() {
    printf("\n=== Dungeon Info ===\n");
    for (int i = 0; i < MAX_DUNGEONS; ++i) {
        if (strlen(dungeons[i].name) > 0) {
            printf("Name: %s | Min Level: %d | ATK: +%d | HP: +%d | DEF: +%d | EXP: +%d | Key: %s\n",
                   dungeons[i].name, dungeons[i].min_level, dungeons[i].reward_atk,
                   dungeons[i].reward_hp, dungeons[i].reward_def, dungeons[i].reward_exp,
                   dungeons[i].key);
        }
    }
}

void generate_dungeon() {
    const char *names[] = {
        "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon", "Hunters Guild Dungeon", "Busan A-Rank Dungeon", "Insects Dungeon", "Goblins Dungeon", "D-Rank Dungeon", "Gwanak Mountain Dungeon", "Hapjeong Subway Station Dungeon"
    };
    
    srand(time(NULL));
    for (int i = 0; i < MAX_DUNGEONS; ++i) {
        if (strlen(dungeons[i].name) == 0) {
            int idx = rand() % 11;
            bool unique = false;
            while (!unique) {
                unique = true;
                for (int j = 0; j < MAX_DUNGEONS; ++j) {
                    if (strcmp(dungeons[j].name, names[idx]) == 0) {
                        unique = false;
                        idx = rand() % 11;
                        break;
                    }
                }
            }

            strcpy(dungeons[i].name, names[idx]);
            dungeons[i].min_level = 1 + rand() % 5;
            dungeons[i].reward_atk = 100 + rand() % 51;
            dungeons[i].reward_hp = 50 + rand() % 51;
            dungeons[i].reward_def = 25 + rand() % 26;
            dungeons[i].reward_exp = 150 + rand() % 151;
            snprintf(dungeons[i].key, 50, "DNG%03d", rand() % 1000);
            printf("Dungeon '%s' generated!\n", dungeons[i].name);
            break;
        }
    }
}


void ban_hunter() {
    char key[50];
    printf("Enter Hunter Key to Ban: ");
    scanf("%s", key);
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strcmp(hunters[i].key, key) == 0) {
            hunters[i].banned = 1;
            printf("Hunter %s has been banned.\n", hunters[i].name);
            sem_post(sem);
            return;
        }
    }
    sem_post(sem);
    printf("Hunter not found.\n");
}

void unban_hunter() {
    char key[50];
    printf("Enter Hunter Key to Unban: ");
    scanf("%s", key);
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strcmp(hunters[i].key, key) == 0) {
            hunters[i].banned = 0;
            printf("Hunter %s is now unbanned and can raid again.\n", hunters[i].name);
            sem_post(sem);
            return;
        }
    }
    sem_post(sem);
    printf("Hunter not found.\n");
}

void reset_hunter() {
    char key[50];
    printf("Enter Hunter Key to Reset: ");
    scanf("%s", key);
    sem_wait(sem);
    for (int i = 0; i < MAX_HUNTERS; ++i) {
        if (strcmp(hunters[i].key, key) == 0) {
            hunters[i].level = 1;
            hunters[i].exp = 0;
            hunters[i].atk = 10;
            hunters[i].hp = 100;
            hunters[i].def = 5;
            hunters[i].banned = 0;

            printf("Hunter %s has been reset and unbanned. They can now raid again.\n", hunters[i].name);
            sem_post(sem);
            return;
        }
    }
    sem_post(sem);
    printf("Hunter not found.\n");
}

void admin_menu() {
    while (running) {
        printf("\n=== SYSTEM MENU ===\n");
        printf("1. Hunter Info\n");
        printf("2. Dungeon Info\n");
        printf("3. Generate Dungeon\n");
        printf("4. Ban Hunter\n");
        printf("5. Reset Hunter\n");
        printf("6. Unban Hunter\n");
        printf("7. Shutdown System\n");
        printf("0. Exit\n");
        printf("Choice: ");
        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1: show_hunters(); break;
            case 2: show_dungeons(); break;
            case 3: generate_dungeon(); break;
            case 4: ban_hunter(); break;
            case 5: reset_hunter(); break;
            case 6: unban_hunter(); break;
            case 7:
                running = false;
                continue_running = false; 
                printf("Shutting down system...\n");
                break;
            case 0: 
                running = false; 
                break;
            default: printf("Invalid choice.\n"); break;
        }
    }
}

int main() {
    signal(SIGINT, handle_sigint);

    int shm_fd1 = shm_open(SHM_HUNTER, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd1, sizeof(Hunter) * MAX_HUNTERS);
    hunters = mmap(NULL, sizeof(Hunter) * MAX_HUNTERS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

    int shm_fd2 = shm_open(SHM_DUNGEON, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd2, sizeof(Dungeon) * MAX_DUNGEONS);
    dungeons = mmap(NULL, sizeof(Dungeon) * MAX_DUNGEONS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    printf("System started. Choose 0 to exit.\n");
    admin_menu();

    if (!continue_running) {
        shm_unlink(SHM_HUNTER);
        shm_unlink(SHM_DUNGEON);
        sem_close(sem);
    }

    return 0;
}
