# Laporan Resmi Praktikum Sisop Modul 3


## Anggota Kelompok

| No | Nama                   | NRP         |
|----|------------------------|-------------|
| 1  | Aditya Reza Daffansyah | 5027241034  |
| 2  | Ahmad Yafi Ar Rizq     | 5027241066  |
| 3  | Zahra Khaalishah       | 5027241070  |



## Daftar Isi
### Soal 1
- [a. ]
- [b. ]
- [c. ]
- [d. ]
- [e. ]
- [f. ]
- [g. ]
  
### Soal 2
- [a. ]
- [b. ]
- [c. ]
- [d. ]
- [e. ]
 
### Soal 3
- [a. ]
- [b. ]
- [c. ]
- [d. ]
- [e. ]
- [f. ]
- [g. ]
- [h. ]
 
### Soal 4
- [a. Shared Memory](#a-shared-memory)
- [b. Register Login](#b-register-login)
- [c. Hunter Info](#c-hunter-info)
- [d. Generate Dungeon](#d-generate-dungeon)
- [e. Dungeon Info](#e-dungeon-info)
- [f. Dungeon List (Hunter Menu)](#f-dungeon-list-(hunter-menu))
- [g. Dungeon Raid & Level Up](#g-dungeon-raid-&-level-up)
- [h. Hunter's Battle (Revised)](#h-hunter's-battle-(revised))
- [k. ]
- [l. ]

# Soal 1

# Soal 2

# Soal 3

# Soal 4
## a. Shared Memory
### system.c membuat shared memory
```
  int shm_fd1 = shm_open(SHM_HUNTER, O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd1, sizeof(Hunter) * MAX_HUNTERS);
  hunters = mmap(NULL, sizeof(Hunter) * MAX_HUNTERS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);

  int shm_fd2 = shm_open(SHM_DUNGEON, O_CREAT | O_RDWR, 0666);
  ftruncate(shm_fd2, sizeof(Dungeon) * MAX_DUNGEONS);
  dungeons = mmap (NULL, sizeof(Dungeon) * MAX_DUNGEONS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
```
  Code ini terdapat pada file system.c yang berfungsi untuk membuat dan mengatur shared memory. `shm_open()` digunakan dengan `flag O_CREAT | O_RDWR` untuk membuat shared memory baru jika belum ada. `ftruncate()` mengatur ukuran memori untuk masing-masing struktur. `mmap()` digunakan agar pointer hunters dan dungeons dapat mengakses shared memory tersebut.

### hunter.c membuka shared memory
```
// hunter.c - Membuka shared memory yang sudah dibuat system.c
int shm_fd1 = shm_open(SHM_HUNTER, O_RDWR, 0666);
int shm_fd2 = shm_open(SHM_DUNGEON, O_RDWR, 0666);
if (shm_fd1 < 0 || shm_fd2 < 0) {
    perror("Run system first.");
    exit(1);
}
```
Code ini menggunakan shared memory yang telah dijalankan pada system.c. Jika shared memory belum dibuat, maka `shm_open()` akan gagal dan memunculkan message "Run system first.".

## b. Register Login
### Register
```
#define MAX_HUNTERS 100
const char *SHM_HUNTER = "/hunter_shm";
const char *SEM_NAME = "/hunter_sem";

typedef struct {
    char name[50];
    int level, exp, atk, hp, def;
    int banned;
    char key[50];
} Hunter;

Hunter *hunters;
sem_t *sem;
```
Pada code berikut mendefinisikan atribut hunter seperti name, level, exp, atk, hp, def, banned, dan key. Shared memory untuk hunter di deklarasikan dengan `/hunter_shm` . 

```
hunters = mmap(NULL, sizeof(Hunter) * MAX_HUNTERS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
dungeons = mmap(NULL, sizeof(Dungeon) * MAX_DUNGEONS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);
sem = sem_open(SEM_NAME, 0);
}
```
Shared memory di-map ke memori proses menggunakan `mmap` untuk mengisi data hunter. 

```
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
```
Program untuk register akan meminta user menuliskan namanya, kemudian mengatur stats awal sesuai soal: level = 1, exp = 0, atk = 10, hp = 100, def = 5 dan mengatur banned = 0 (hunter aktif). Kemudian membuat unique id agar tidak sama dengan user lain dengan format "HNTxxx" dengan nomor acak 0-999. Jika slot hunter penuh maka akan di print "Hunter list full!". Hunter maksimal adalah 100.

### Login
```
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
```
Untuk fungsi login ini, program akan meminta unique id user untuk digunakan login kemudian mencari hunterkey yang cocok di array `hunters` . Jika hunter di ban maka akan muncul pesan "You are banned." kemudian jika hunterkey salah maka akan muncul pesan "Invalid key". Jika user benar maka kita akan masuk dan dilanjutkan ke `user_menu`.

## c. Hunter Info
```
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
```
Fungsi ini menampilkan semua informasi hunter yang tersimpan di shared memory. Menggunakan `sem_wait` dan `sem_post` untuk memastikan akses aman ke shared memory. Informasi yang ditampilkan berisi name, level, exp, atk, hp, def, key, dan status hunter (ACTIVE/BANNED). Status hunter ditentukan oleh field `banned`.

## d. Generate Dungeon
```
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
```
Dungeon dibuat secara random dengan nama dari array names (sheets). Level dengan range 1 sampai 5. Reward ATK 100–150, HP 50–100, DEF 25–50, dan EXP 150–300. Disimpan ke shared memory `dungeons[]`. Setiap dungeon memiliki unique key `DNGxxx`, dan bisa diakses hunter dari proses lain melalui shared memory.

## e. Dungeon Info
```
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
```
Fungsi `show_dungeons()` menampilkan seluruh informasi dungeon yang ada di shared memory. Informasi yang ditampilkan adalah nama dungeon, level minimum dungeon, reward dungeon (ATK, HP, DEF, dan EXP), Unique Key Dungeon `DNGxxx`. Dungeon hanya ditampilkan jika memiliki nama (valid data), yang diperiksa melalui `strlen(dungeons[i].name) > 0`.

## f. Dungeon List (Hunter Menu)
```
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
```
Fungsi `show_dungeons_by_level()` akan menampilkan daftar dungeon yang dapat diakses oleh hunter berdasarkan level. Hunter hanya dapat melihat dungeon yang memiliki `min_level <= level hunter`. Filter dungeon dilakukan dengan `dungeons[i].min_level <= player->level`  dan hanya dungeon dengan data valid `(strlen(dungeons[i].name) > 0)` yang ditampilkan.

## g. Dungeon Raid & Level Up
```
void enter_dungeon(Hunter *hunter, Dungeon *dungeon) {
    int hunter_power = hunter->ATK + hunter->DEF + hunter->HP;
    if (hunter_power >= dungeon->difficulty) {
        printf("Hunter berhasil menaklukkan dungeon!\n");

        hunter->ATK += dungeon->reward_ATK;
        hunter->DEF += dungeon->reward_DEF;
        hunter->HP  += dungeon->reward_HP;
        hunter->EXP += dungeon->reward_EXP;

        if (hunter->EXP >= 500) {
            hunter->level += 1;
            hunter->EXP = 0;
            printf("Hunter naik level!\n");
        }
        sem_wait(&sem_dungeon);
        dungeon->is_active = 0;
        sem_post(&sem_dungeon);
    } else {
        printf("Hunter gagal menaklukkan dungeon. Terlalu lemah.\n");
    }
}
```
Code berikut berfungsi untuk melakukan raid pada dungeon. Code `hunter_power = ATK + DEF + HP` berfungsi untuk mengukur kekuatan hunter. Kemudia code `if (hunter_power >= dungeon->difficulty)` memastikan dungeon hanya bisa dikalahkan jika hunter lebih kuat. 
`        hunter->ATK += dungeon->reward_ATK;
        hunter->DEF += dungeon->reward_DEF;
        hunter->HP  += dungeon->reward_HP;
        hunter->EXP += dungeon->reward_EXP;
`
Code berikut berfungsi untuk menambahkan reward kepada hunter jika berhasil memenangkan dungeon. 

` if (hunter->EXP >= 500) {
            hunter->level += 1;
            hunter->EXP = 0;
            printf("Hunter naik level!\n");
        }
`
Code berikut memeriksa EXP dari hunter, jika EXP sama atau lebih dari 500 setelah memenangkan dungeon maka hunter akan naik level. Ketika hunter naik level maka EXP akan di reset menjadi 0 kembali.

## h. Hunter’s Battle (Revised)

```
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
```
Code berikut  berfungsi untuk menampilkan lawan tersedia, menampilkan semua hunter kecuali diri sendiri dan hunter yang di banned.

```
    if (count == 0) {
        printf("No opponents available.\n");
        return;
    }

    char target_key[50];
    printf("Enter Enemt Hunter Key: ");
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
```

Jika tidak ada hunter lain maka akan mendapatkan pesan “No opponents available.”. Kemudian jika terdapat hunter lain maka kita akan diperintahkan untuk memasukkan Hunter Key lawan yang dipiliih. Kemudian jika salah memasukkan Hunter Key maka akan ditampilkan “Opponents not valid”.
```
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
```
Setalah memilih musuh akan muncul message “Fighting against xxx”. Jika kita lebih kuat dalam segala aspek dari musuh maka kita akan menang dan muncul message “You Win!” dan user hunter lawan akan dihapus dari shared memory. Jika kita kalah dari lawan maka akan muncul message “You Lose!” dan “Eliminated!” kemudian user kita akan dihapus dari shared memory. 

