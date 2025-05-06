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
- [c. ]
- [d. ]
- [e. ]
- [f. ]
- [g. ]
- [h. ]
- [i. ]
- [j. ]
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




