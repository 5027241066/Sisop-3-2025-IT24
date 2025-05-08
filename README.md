# Laporan Resmi Praktikum Sisop Modul 3


## Anggota Kelompok

| No | Nama                   | NRP         |
|----|------------------------|-------------|
| 1  | Aditya Reza Daffansyah | 5027241034  |
| 2  | Ahmad Yafi Ar Rizq     | 5027241066  |
| 3  | Zahra Khaalishah       | 5027241070  |



## Daftar Isi
### Soal 1
- [a. Downloading dan Unzipping File Input txt](#a-downloading-dan-unzipping-file-input-txt)
- [b. Membuat image server sebagai Daemon dan Menghubungkannya dengan image client melalui RPC](#b-membuat-image-server-sebagai-daemon-dan-menghubungkannya-dengan-image-client-melalui-rpc)
- [c. Membuat Code image client untuk Mendecrypt File menjadi Gambar](#c-membuat-code-image-client-untuk-mendecrypt-file-menjadi-gambar)
- [d. Membuat Menu image client sebagai Menu Kreatif yang Menerima Input Berkali Kali](#d-membuat-menu-image-client-sebagai-menu-kreatif-yang-menerima-input-berkali-kali)
- [e. Visibilitas Gambar yang Dihasilkan dari Proses Decrypt File txt](#e-visibilitas-gambar-yang-dihasilkan-dari-proses-decrypt-file-txt)
- [f. Error Handling Program](#f-error-handling-program)
- [g. Mencatat Log Activity Program pada server log](#g-mencatat-log-activity-program-pada-server-log)
  
### Soal 2
- [a. Mengunduh File Order dan Menyimpannya ke Shared Memory](#a-mengunduh-file-order-dan-menyimpannya-ke-shared-memory)
- [b. Pengiriman Bertipe Express](#b-pengiriman-bertipe-express)
- [c. Pengiriman Bertipe Reguler](#c-pengiriman-bertipe-reguler)
- [d. Mengecek Status Pesanan](#d-mengecek-status-pesanan)
- [e. Melihat Daftar Semua Pesanan](#e-melihat-daftar-semua-pesanan)
  

 
### Soal 3
- [a. Entering the Dungeon](#a-Entering-the-Dungeon)
- [b.Sightseeing](#b-Sightseeing)
- [c.Status Check](#c-Status-Check)
- [d.Weapon Shop](#d-Weapon-Shop)
- [e.Handy Inventory](#e-Handy-Iventory)
- [f.Enemy Encounter](#f-Enemy-Ecounter)
- [g.Other Battle Logic](#g-Other-Battle-Logic)
- [h.Error Handling](#h-Error-Handling)

 
### Soal 4
- [a. Shared Memory](#a-shared-memory)
- [b. Register Login](#b-register-login)
- [c. Hunter Info](#c-hunter-info)
- [d. Generate Dungeon](#d-generate-dungeon)
- [e. Dungeon Info](#e-dungeon-info)
- [f. Dungeon List (Hunter Menu)](#f-dungeon-list-hunter-menu)
- [g. Dungeon Raid & Level Up](#g-dungeon-raid--level-up)
- [h. Hunter's Battle (Revised)](#h-hunters-battle-revised)
- [i. Ban & Unban Hunter](#i-ban--unban-hunter)
- [j. Reset Hunter](#j-reset-hunter)
- [k. Notification](#k-notification)
- [l. Shutdown Server](#l-shutdown-server)

# Soal 1
Sekarang tahun 2045, seluruh dunia mengalami kekacauan dalam segala infrastruktur siber. Sebagai seorang mahasiswa Departemen Teknologi Informasi ITS, anda memiliki ide untuk kembali ke masa lalu (tahun 2025) untuk memanggil hacker asal Semarang bernama “rootkids” yang mampu melawan segala hacker lain. Tetapi anda tidak tahu bagaimana cara mencapainya.

Hanya sedikit yang diketahui tentang hacker bernama “rootkids”. Beberapa informasi yang anda temukan dari deep web hanyalah berupa beberapa file text yang berisi tulisan aneh, beserta beberapa petunjuk untuk mengubah text tersebut menjadi sebuah file jpeg.

Karena anda adalah seorang professional programmer, anda mengikuti petunjuk yang anda dapatkan dari deep web untuk membuat sistem RPC server-client untuk mengubah text file sehingga bisa dilihat dalam bentuk file jpeg. Situs deep web yang anda baca terlihat sebagai berikut. 

Working directory akhir : 


![tree server client](assets/tree%20server%20client.png)

## a. Downloading dan Unzipping File Input txt
Mendownload `secrets.zip` dan mengekstrak secara manual ke dalam directory `/client/secrets`

## b. Membuat image server sebagai Daemon dan Menghubungkannya dengan image client melalui RPC
- `image_server.c` akan berjalan sebagai `daemon` (berjalan terus menerus pada background).
- Menggunakan socket TCP `(AF_INET)`, untuk komunikasi melalui RPC dengan client.


```
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); 
    
    setsid();
    umask(0);
    chdir("/");

    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) close(x);
}

int server_fd = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
server_addr.sin_addr.s_addr = INADDR_ANY;

bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
listen(server_fd, 5);
```
Penjelasan :
- Server akan dijalankan secara daemon dengan `./server/image_server`.
- Client akan terhubung ke port `127.0.0.1:8080`.
- Melakukan komunikasi dengan format pesan :
    - Upload : `SEND|(filename)|(data)`
    - Download : `DOWNLOAD|(filename)`
    

## c. Membuat Code image client untuk Mendecrypt File menjadi Gambar
Proses decrypt dilakukan dengan mereverse text dengan format txt, lalu melakukan decode dari hex ke binary untuk dapat menjadi gambar dengan format .jpeg

```
void reverse(char *str) {
    for (int i = 0, j = strlen(str)-1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

int hex_ke_binary(const char *hex, unsigned char *output) {
    for (int i = 0; i < strlen(hex); i += 2) {
        sscanf(hex + i, "%2hhx", &output[i/2]);
    }
    return strlen(hex) / 2;
}
```
- Output dari hasil decrypt akan disimpan pada direktori `/server/database`.
- Format dari hasil decrypt berupa timestamp dengan format .jpeg

Output : 


![upload from client to server](assets/upload%20from%20client%20to%20server.png)

## d. Membuat Menu image client sebagai Menu Kreatif yang Menerima Input Berkali Kali
```
while (1) {
    printf("\n============== IMAGE CLIENT =============\n");
    printf("1. Kirim file teks ke server\n");
    printf("2. Download file JPEG dari server\n");
    printf("3. Exit\n");
    printf("Pilih opsi: ");
    scanf("%d", &opsi);

    switch (opsi) {
        case 1: kirim_file(); break;
        case 2: download_file(); break;
        case 3: exit(0);
        default: printf("Opsi tidak valid.\n");
    }
}
```
Terdapat 3 fitur dan sebuah `default` case :
- Menu untuk mengirim file teks ke server
- Menu untuk mendownload file yang telah di decrypt dari server
- Menu untuk keluar dari program

Output : 


![download form server to client](assets/download%20form%20server%20to%20client.png)

## e. Visibilitas Gambar yang Dihasilkan dari Proses Decrypt File txt
Sejauh ini, text belum terdecrypt dengan benar, sehingga belum dihasilkan visibilitas gambarnya. Namun, untuk masalah komunikasi antara `server` dan `client` sudah berjalan lancar, file berhasil di upload dengan format txt dan disimpan ke database setelah di decrypt, dan dapat di unduh kembali.

## f. Error Handling Program
Terdapat beberapa error handling dalam program ini :
- Error handling untuk masalah gagal connect ke server
- Error handling ketika ada nama file yang tidak sesuai dengan yang ada di client atau server
- Error handling untuk pesan RPC salah

```
if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Gagal connect ke server");
    exit(1);
}
```
```
if (!fin) {
    char *res = "ERROR|File tidak ditemukan";
    write(sock, res, strlen(res));
    tulis_log("Server", "ERROR", "File tidak ditemukan");
    close(sock);
    exit(1);
}
```

Output :


![error handling nama file](assets/error%20handling%20nama%20file.png)




![error handling w no server](assets/error%20handling%20w%20no%20server.png)

## g. Mencatat Log Activity Program pada server log
```
void tulis_log(const char *sumber, const char *aksi, const char *info) {
    FILE *log = fopen(LOG_FILE, "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char waktu[100];
    strftime(waktu, sizeof(waktu), "%Y-%m-%d %H:%M:%S", t);
    fprintf(log, "[%s][%s]: [%s] [%s]\n", sumber, waktu, aksi, info);
    fclose(log);
}
```
- Mencatat segala log aktivitas dari `client` maupun `server`.
- Mencatat beberapa aksi seperti `DECRYPT`, `SAVE`, `DOWNLOAD`, dan `UPLOAD`

Output :


![server client log](assets/server%20client%20log.png)


# Soal 2
Tahun 2025, di tengah era perdagangan serba cepat, berdirilah sebuah perusahaan ekspedisi baru bernama RushGo. RushGo ingin memberikan layanan ekspedisi terbaik dengan 2 pilihan, Express (super cepat) dan Reguler (standar). Namun, pesanan yang masuk sangat banyak! Mereka butuh sebuah sistem otomatisasi pengiriman, agar agen-agen mereka tidak kewalahan menangani pesanan yang terus berdatangan. Kamu ditantang untuk membangun Delivery Management System untuk RushGo (Author: Nayla / naylaarr)

Sistem ini terdiri dari dua bagian utama:
- `delivery_agent.c` untuk agen otomatis pengantar Express
- `dispatcher.c untuk` pengiriman dan monitoring pesanan oleh user

Working directory akhir :


![deliver full tree](assets/deliver%20full%20tree.png)

## a. Mengunduh File Order dan Menyimpannya ke Shared Memory
Mengunduh File Order dan Menyimpannya ke Shared Memory
Untuk memulai, Anda perlu mengelola semua orderan yang masuk dengan menggunakan shared memory.
- Unduh file delivery_order.csv
- Setelah file CSV diunduh, program Anda harus membaca seluruh data dari CSV dan menyimpannya ke dalam shared memory.

Mengunduh file `delivery_order.csv` secara manual, dan memindahkan ke direktori soal ini.

Struktur data shared memory :
```
typedef struct {
    char nama[50];
    char alamat[50];
    char jenis[10];  
    char status[15]; 
    char agen[20];  
} order;

typedef struct {
    order daftar_order[50];
    int jumlah_order;
} shared_data;
```
- Struct `order` digunakan untuk menangani informasi tentang pesanan seperti nama pelanggan, alamat pelanggan, jenis pengiriman, status pengiriman, dan nama agen pengiriman
- Struct `shared_data` digunakan untuk menangani daftar order dan jumlah order utuk menangani pesanan

Struktur untuk memuat file csv ke shared memory :
```
void muat_file_csv_ke_memori(const char *namafile) {
    FILE *fp = fopen(namafile, "r");
    while (fgets(baris, sizeof(baris), fp)) {
        sscanf(baris, "%49[^,],%49[^,],%9[^\n]",
               data->daftar_order[data->jumlah_order].nama,
               data->daftar_order[data->jumlah_order].alamat,
               data->daftar_order[data->jumlah_order].jenis);

        strcpy(data->daftar_order[data->jumlah_order].status, "PENDING");
        data->jumlah_order++;
    }
    fclose(fp);
}
```
- `delivery_order.csv` akan dibaca dan dimuat ke dalam shared memory
- Mengakses shared memory dengan `key = frok("delivery_agent.c"), 123`
- Memuat data order dengan atribut nama pelanggan, alamat pelanggan, dan jenis pengiriman
- Mengubah semua status pengiriman yang baru diinput sebagai `PENDING`, karena belum ada pesanan yang diantar

## b. Pengiriman Bertipe Express
RushGo memiliki tiga agen pengiriman utama: AGENT A, AGENT B, dan AGENT C.
Setiap agen dijalankan sebagai thread terpisah.
Agen-agen ini akan secara otomatis:
- Mencari order bertipe Express yang belum dikirim.
- Mengambil dan mengirimkannya tanpa intervensi user.
- Setelah sukses mengantar, program harus mencatat log di delivery.log

```
void *agen_jalan(void *arg) {
    char *agen = (char *)arg;
    while (1) {
        for (int i = 0; i < data->jumlah_order; i++) {
            if (strcmp(data->daftar_order[i].jenis, "Express") == 0 &&
                strcmp(data->daftar_order[i].status, "PENDING") == 0) {

                strcpy(data->daftar_order[i].status, "DELIVERED");
                strcpy(data->daftar_order[i].agen, agen);
                tulis_log(agen, data->daftar_order[i].nama, data >daftar_order[i].alamat);
            }
        }
        sleep(1); 
    }
}

int main() {
pthread_create(&agen_a, NULL, agen_jalan, "A");
pthread_create(&agen_b, NULL, agen_jalan, "B");
pthread_create(&agen_c, NULL, agen_jalan, "C");

return 0;
};
```

- Jalankan `./delivery_agent.c`
- Dilakukan pengecekan apakah ada order tipe express yang masih pending, jika ada maka order akan diantar dan statusnya diubah menjadi delivered.
- Tuliskan log dari proses `delivery_agent` dengan memanggil fungsi penulisan log
- Buat 3 thread berbeda untuk dapat menjalankan agent express A, B, C dalam thread yang berbeda

Log pengiriman express :


![auto delivery log](assets/auto%20delivery%20log.png)
  
## c. Pengiriman Bertipe Reguler
Berbeda dengan Express, untuk order bertipe Reguler, pengiriman dilakukan secara manual oleh user.
User dapat mengirim permintaan untuk mengantar order Reguler dengan memberikan perintah deliver dari dispatcher. 
Penggunaan:
`./dispatcher -deliver [Nama]`
- Pengiriman dilakukan oleh agent baru yang namanya adalah nama user.
- Setelah sukses mengantar, program harus mencatat log di delivery.log

```
if (strcmp(argv[1], "-deliver") == 0) {
    for (int i = 0; i < data->jumlah_order; i++) {
        if (strcmp(data->daftar_order[i].nama, argv[2]) == 0 &&
            strcmp(data->daftar_order[i].jenis, "Reguler") == 0) {

            strcpy(data->daftar_order[i].status, "DELIVERED");
            strcpy(data->daftar_order[i].agen, getenv("USER"));
            tulis_log(getenv("USER"), data->daftar_order[i].nama, data->daftar_order[i].alamat);
        }
    }
}
```
- Jalankan program `dispatcher -deliver [Nama]`.
- Dilakukan pengecekan apakah nama input sesuai dengan data pada `daftar_order` dan dilakukan pengecekan apakah order tersebut berasal dari order reguler.
- Jika ditemukan kecocokan, maka order akan dikirim (delivered) dan akan mengambil nilai agen mana yang mengirimkan order tersebut.
- Memanggil fungi tulis log aktivitas untuk mencatat aktivitas program sesuai dengan data yang ada.

Output : 


![antar manual](assets/antar%20manual.png)

## d. Mengecek Status Pesanan
Mengecek Status Pesanan
Dispatcher juga harus bisa mengecek status setiap pesanan.
Penggunaan:
`./dispatcher -status [Nama]`

```
else if (strcmp(argv[1], "-status") == 0) {
    for (int i = 0; i < data->jumlah_order; i++) {
        if (strcmp(data->daftar_order[i].nama, argv[2]) == 0) {
            printf("Status for %s: %s by Agent %s\n", 
                   data->daftar_order[i].nama,
                   data->daftar_order[i].status,
                   data->daftar_order[i].agen);
        }
    }
}
```
- Jalankan `./dispatcher -status [Nama]`
- Melakukan pengecekan apakah nama input sesuai dengan data order
- Jika sesuai, akan melakukan printout status order berupa nama, status pengiriman, dan nama agen

Status Express :


![status express](assets/status%20express.png)

Status Manual :


![status manual](assets/status%20manual.png)


## e. Melihat Daftar Semua Pesanan
Untuk memudahkan monitoring, program dispatcher bisa menjalankan perintah list untuk melihat semua order disertai nama dan statusnya.
Penggunaan:
`./dispatcher -list`

```
else if (strcmp(argv[1], "-list") == 0) {
    for (int i = 0; i < data->jumlah_order; i++) {
        printf("%d. %s (%s) | %s\n",
               i + 1,
               data->daftar_order[i].nama,
               data->daftar_order[i].jenis,
               data->daftar_order[i].status);
    }
}
```
- Jalankan perintah `./dispatcher -list`
- Melakukan perulangan untuk menampilkan data order
- Data yang ditampilkan berupa nama, jenis, dan status order

Output list:


![dispatcher list](assets/dispatcher%20list.png)

Output log express dan reguler :


![full delivery log](assets/full%20delivery%20log.png)



# Soal 3
# The Lost Dungeon
---
Program ini merupakan permainan berbasis teks dengan arsitektur client-server menggunakan socket TCP dan multithreading (`pthread`). Terdapat dua file utama:

- `dungeon.c` sebagai **server dungeon** yang memproses semua logic game dan menyimpan status player.
- `player.c` sebagai **client** yang terhubung ke server untuk mengakses fitur dungeon.

---

### a. Entering the Dungeon

Saat `player.c` dijalankan, program akan mengirim koneksi TCP ke server (`dungeon.c`) di `127.0.0.1:8080`.

**Command (dari client):**

```c
connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
```

**Output (di server):**

```
[SERVER] Client terhubung.
```

Server akan membuat thread baru dengan `pthread_create` untuk menangani masing-masing client.

---

### b. Sightseeing

Client menampilkan menu utama agar pemain bisa menjelajahi fitur dungeon.

**Command (di client):**

```text
1. Show Player Stats
2. Shop
3. View Inventory
4. Battle Mode
5. Exit
```

**Setiap opsi dikirim ke server sebagai angka string:**

```c
send(sock, pilihan, strlen(pilihan), 0);
```

---

### c. Status Check

Saat pemain memilih opsi `1`, client mengirim `"1"` ke server. Server akan mengirimkan status pemain yang mencakup:

- Gold
- Senjata aktif
- Damage dasar
- Passive (jika ada)
- Jumlah musuh dikalahkan

**Command (dikirim ke server):**

```
"1"
```

**Output (dari server):**

```
===== Player Status =====
Gold            : 120G
Active Weapon   : Bow
Base Damage     : 8
Passive         : Poison
Enemies Defeated: 3
=========================
```

Jika belum punya senjata:

```
Active Weapon   : (Belum ada)
Base Damage     : 5
Passive         : -
```

---

### d. Weapon Shop

Saat memilih opsi `2`, client mengirim `"2"` ke server. Server menampilkan daftar senjata yang bisa dibeli.

**Command:**

```
"2"
```

**Output (dari server):**

```
===== Weapon Shop =====
1. Sword  - 50G  - Damage: 10
2. Bow    - 60G  - Damage: 8  (Passive: Poison)
3. Axe    - 70G  - Damage: 12
4. Staff  - 90G  - Damage: 7  (Passive: Heal)
5. Dagger - 40G  - Damage: 6
Pilih senjata [1–5], atau 0 untuk batal:
```

**Lalu client kirim angka pilihan senjata (misal `"2"` untuk Bow)**

Jika berhasil:

```
Berhasil membeli Bow! Sisa gold: 60G
```

Jika gold tidak cukup:

```
Uang tidak cukup untuk membeli senjata.
```

---

### e. Handy Inventory

Saat client mengirim `"3"`, server akan menampilkan daftar senjata yang dimiliki.

**Command:**

```
"3"
```

**Output:**

```
===== Inventory =====
1. Bow (Damage: 8, Passive: Poison)
2. Dagger (Damage: 6)
Pilih nomor senjata untuk equip, atau 0 untuk batal:
```

**Lalu client mengirim `"1"` untuk memilih senjata pertama**

**Output jika berhasil equip:**

```
Senjata aktif sekarang: Bow
```

Jika memilih `0`:

```
Tidak jadi equip senjata.
```

---

### f. Enemy Encounter

Saat client mengirim `"4"`, server memulai battle mode.

**Command:**

```
"4"
```

**Output awal:**

```
Musuh muncul dengan 155 HP!
Ketik 'attack' untuk menyerang atau 'exit' untuk keluar battle mode.
```

**Command:**

```
"attack"
```

**Output jika berhasil menyerang:**

```
Kamu menyerang musuh: 12 damage
Passive aktif: Musuh terkena poison selama 2 turn.
Sisa HP musuh: 143
```

Jika critical hit:

```
Kamu menyerang musuh: 22 damage (Critical!)
```

Jika musuh kalah:

```
Seranganmu membunuh musuh!
Reward: 25G
Total musuh dikalahkan: 4
Musuh baru muncul dengan 137 HP.
```

**Command untuk keluar battle mode:**

```
"exit"
```

**Output:**

```
Keluar dari Battle Mode.
```

---

### g. Other Battle Logic

#### ✅ HP & Reward:

- HP musuh dibuat acak antara **50–200**:

```c
int enemy_hp = 50 + rand() % 151;
```

- Reward gold dibuat acak antara **10–30**:

```c
int gold_reward = 10 + rand() % 21;
```

#### ✅ Damage Equation:

- Base damage: senjata aktif (atau 5 jika belum punya)
- Modifier: `+0` sampai `+2` (acak)
- Critical hit 25%:

```c
int is_critical = rand() % 100 < 25;
```

#### ✅ Passive Effect:

- `Poison`: memberikan 5 damage tambahan selama 2 turn.
- `Heal`: menyembuhkan 10–20 HP (simulasi).

---

## 💾 Penyimpanan Data

Status pemain disimpan di file `player_data.txt`:

```
120         # gold
2           # jumlah inventory
4           # enemies defeated
Sword 10 -
Bow 8 Poison
1           # index weapon aktif
```

Saat program dijalankan kembali, file ini dibaca dan status dipulihkan dengan fungsi `load_player()`.

---

## ⚠️ Error Handling

#### Input menu tidak valid:

```
Pilihan tidak dikenal. Masukkan 1–5.
```

#### Inventory kosong saat ingin equip:

```
Inventory kosong.
```

#### Input selain `attack` atau `exit` dalam battle:

```
Input tidak valid! Gunakan 'attack' atau 'exit'.
```

---

---


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

![image](https://github.com/user-attachments/assets/773e437a-0de7-4966-b2c2-84f2ef036a25)


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
![image](https://github.com/user-attachments/assets/52a6f8ce-3fe9-41c2-a870-6cc8d8a7f5f3)

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
![image](https://github.com/user-attachments/assets/fee4dee4-2c3c-4b3e-a303-a8eaa4759d3d)

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
![image](https://github.com/user-attachments/assets/e6d17f8a-cb2a-4605-9725-a2a4a8e6aeff)

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
![image](https://github.com/user-attachments/assets/fe70d2ff-aeb2-4662-8442-d675e9e5bd17)

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
![image](https://github.com/user-attachments/assets/05e6e706-645e-4c3c-87b8-ace5f9671c37)


## g. Dungeon Raid & Level Up
```
void raid_dungeon(Hunter *player) {
    printf("\n=== Dungeons Available (Level %d) ===\n", player->level);
    for (int j = 0; j < MAX_DUNGEONS; ++j) {
        if (strlen(dungeons[j].name) > 0 && dungeons[j].min_level <= player->level) {
            printf("- %s | Min Lv: %d | ATK+%d | HP+%d | DEF+%d | EXP+%d | Key: %s\n",
                   dungeons[j].name, dungeons[j].min_level, dungeons[j].reward_atk,
                   dungeons[j].reward_hp, dungeons[j].reward_def, dungeons[j].reward_exp,
                   dungeons[j].key);
        }
    }
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
```
Code berikut berfungsi untuk melakukan raid pada dungeon. Akan ditampilkan dahulu list dungeon yang bisa diserang. Code `if (strcmp(dungeons[i].key, input_key) == 0 && dungeons[i].min_level <= player->level) {` berfungsi untuk mengukur kekuatan hunter. Kemudia code `    if (found == -1) {` memastikan dungeon hanya bisa dikalahkan jika hunter lebih kuat. 
![image](https://github.com/user-attachments/assets/71f15779-60a6-4468-a310-91339b1cab5d)


`    player->atk += dun->reward_atk;
    player->hp += dun->reward_hp;
    player->def += dun->reward_def;
    player->exp += dun->reward_exp;
`
Code berikut berfungsi untuk menambahkan reward kepada hunter jika berhasil memenangkan dungeon. 

`     if (player->exp >= 500) {
        player->level++;
        player->exp = 0;
        printf("Congratulations! You leveled up to Level %d!\n", player->level);
    }
`
Code berikut memeriksa EXP dari hunter, jika EXP sama atau lebih dari 500 setelah memenangkan dungeon maka hunter akan naik level. Ketika hunter naik level maka EXP akan di reset menjadi 0 kembali.
![image](https://github.com/user-attachments/assets/9ce4dac5-25e4-44e0-8633-94510786c135)


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
![image](https://github.com/user-attachments/assets/6f6b9e34-6070-4434-964b-6239f0a0f65f)


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
![image](https://github.com/user-attachments/assets/045b3916-2939-445b-9a20-7eb56dd9ff88)


## i. Ban & Unban Hunter
```
typedef struct {
    int id;
    char name[64];
    int atk, def, hp, exp, level;
    int banned; // 0 = tidak dibanned, 1 = dibanned
} Hunter;
```
Pada code berikut init awal adahal 0 untuk banned berarti hunter active, jika 1 maka hunter dibanned.

### Ban
```
void ban_hunter(int hunter_id) {
    sem_wait(&sem_hunter); // lock akses shared memory hunter

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].id == hunter_id) {
            hunters[i].banned = 1;
            printf("Hunter %s telah dibanned.\n", hunters[i].name);
            break;
        }
    }
    sem_post(&sem_hunter); // unlock akses hunter
}
```
Ketika dijalankan dan ditemukan hunter dengan ID yang sesuai `(hunters[i].id == hunter_id)`, maka field banned pada data hunter tersebut diset menjadi `1`, yang berarti hunter tersebut dibanned. Program memunculkan message bahwa hunter telah dibanned. `break` digunakan untuk menghentikan perulangan karena hunter sudah ditemukan.

![image](https://github.com/user-attachments/assets/da4951d4-95a3-43de-a164-e8856b1e8dfe)

![image](https://github.com/user-attachments/assets/cd82ae91-7d34-4f39-9db4-ecce0764c07b)


### Unban
```
void unban_hunter(int hunter_id) {
    sem_wait(&sem_hunter);

    for (int i = 0; i < MAX_HUNTERS; i++) {
        if (hunters[i].id == hunter_id) {
            hunters[i].banned = 0;
            printf("Hunter %s telah di-unban.\n", hunters[i].name);
            break;
        }
    }
    sem_post(&sem_hunter);
}
```
Ketika dijalankan dan ditemukan hunter dengan ID yang sesuai `(hunters[i].id == hunter_id)`, maka field banned pada data hunter tersebut diset menjadi `0` kembali, yang berarti hunter tersebut telah di unban. Program memunculkan message bahwa hunter telah unban. `break` digunakan untuk menghentikan perulangan karena hunter sudah ditemukan.

![image](https://github.com/user-attachments/assets/c4d73e61-4acb-4f59-a4a6-12d10ea72a6e)

![image](https://github.com/user-attachments/assets/db0f18ba-94ba-40ff-928c-1f82cfd3eb8f)


## j. Reset Hunter
```
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
```
Jika code tersebut dijalankan, kita akan memasukkan Hunter Key kemudian hunter tersebut akan di set seperti awal kembali yaitu level 1, exp 0, attack 10, hp 100, defend 5, dan tidak di ban. Jika berhasil akan muncul message “Hunter xxx has been reset and unbanned. They can now raid again.“. Jika Hunter Key tidak ditemukan maka akan muncul message “Hunter not found.”.

![image](https://github.com/user-attachments/assets/a4d812c6-f430-49f8-b5d9-a551642f6252)

![image](https://github.com/user-attachments/assets/d1f493a0-2055-463d-9598-fdaf4a8d8abe)

## k. Notification
```
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
```
Dari code berikut, code `    for (int i = 0; i < MAX_DUNGEONS; ++i) {
        if (strlen(dungeons[i].name) > 0) {` akan melakukan perulangan untuk mengecek semua dungeon yang tersedia. Jika nama dungeon tidak kosong, berarti dungeon tersebut aktif atau valid. Kemudian code ini  `            printf("Dungeon: %s | Min Lv: %d | Key: %s\n", 
                   dungeons[i].name, dungeons[i].min_level, dungeons[i].key);` akan menampilkan informasi dari dungeon yang ditampilkan. Agar notification berjalan setiap 3 detik maka digunakan code `sleep(3);`.

![image](https://github.com/user-attachments/assets/13ec1207-9763-48cc-887b-3aa2b0be8c63)


## l. Shutdown Server
```
 case 7:
                running = false;
                continue_running = false; 
                printf("Shutting down system...\n");
                break;

```
Dari `running` dan `continue_running` akan di set ke false agar program system berhenti berjalan, tidak seperti exit yang akan tetap menjalankan system di background. `break` berfungsi untuk menghentikan switch case.

![image](https://github.com/user-attachments/assets/40c2a947-ff11-4d79-b88d-5f5e3b3187ad)


