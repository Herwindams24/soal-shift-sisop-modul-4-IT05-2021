# Shift 4 SISOP 2021 - IT05
Penyelesaian Soal Shift 4 Sistem Operasi 2021\
Kelompok IT05
  * Herwinda Marwaa Salsabila (05311840000009)
  * Stefanus Lionel Carlo Nugroho (05311940000027)
  * I Gde Ardha Semaranatha Gunasatwika (05311940000034)

---

## Table of Contents

* [Soal 1](#soal-1)
* [Soal 2](#soal-2)
* [Soal 3](#soal-3)
* [Soal 4](#soal-4)
* [Kendala](#kendala)

---
## Soal 1


---
## Soal 2


---
## Soal 3


---
## Soal 4

### Deskripsi Soal
- Log system yang akan terbentuk bernama `SinSeiFS.log` pada direktori *home* pengguna 
- Log system menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
- Log system terbagi menjadi dua level, yaitu INFO dan WARNING.
- Log level WARNING, digunakan untuk mencatat syscall `rmdir` dan `unlink`.
- Sisanya, akan dicatat pada level INFO.
Format untuk logging yaitu:
```
[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]
```

- Level : Level logging,
- dd : 2 digit tanggal, 
- mm : 2 digit bulan, 
- yyyy : 4 digit tahun, 
- HH : 2 digit jam (format 24 Jam),
- MM : 2 digit menit, 
- SS : 2 digit detik, 
- CMD : System Call yang terpanggil, 
- DESC : informasi dan parameter tambahan.

**Contoh Implementasi:**
```
INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt
```

### Pembahasan

Dalam pembuatan file histori `SinSeiFS.log`, penulis membuat tiga fungsi, yaitu:
- `void buatLog()` ->  Untuk proses `rmdir` dan `unlink`.
- `void buatLogrename()` -> Untuk proses `rename`.
- `void log_info()` -> Untuk selain proses yang telah disebutkan di atas.

Pertama-tama, penulis mendefinisikan path dari file log yang akan diproses oleh ketiga fungsi di atas. 
Di sini penulis, menyimpan file pada direktori `home` penulis seperti yang diminta pada soal.
```c
char *LOG = "/home/kali/SinSeiFS.log";
```
- Selanjutnya, penulis membuat fungsi `buatLog` dengan 2 parameter yaitu 
`process[]` yang berfungsi untuk menampung nilai process yang sedang dijalankan dan `loc[]` yang menampung nilai lokasi dari proses yang sedang berjalan. 
- Path file yang telah di declare dalam variable LOG akan di open dan dilakukan append.
- Kemudian dilakukan pengambilan local time dengan `struct tm`.

```c

// Fungsi untuk membuat log
void buatLog(char process[100], char loc[100])
{
    char text[200];
    FILE *fp = fopen(LOG, "a");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
```

- Dilanjutkan dengan pembuatan tiga `If-statement`, dengan 3 kondisi yang berbeda.
  - Kondisi pertama: Jika menyiratkan process = unlink, maka jalankan blok code di dalamnya (cetak Level `WARNING` dan System Call `UNLINK`).
  - Kondisi ketiga:  Jika menyiratkan process = rmdir, maka jalankan blok code di dalamnya (cetak Level `WARNING` dan System Call `RMDIR`).
- Local time akan disimpan pada file `SinSeiFS.log` sesuai format.

```c
    if (strcmp(process, "unlink") == 0)
    {
        sprintf(text, "WARNING::%02d%02d%04d-%02d:%02d:%02d::UNLINK::%s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, loc);
    }
    else if (strcmp(process, "rmdir") == 0)
    {
        sprintf(text, "WARNING::%02d%02d%04d-%02d:%02d:%02d::RMDIR::%s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, loc);
    }
```

- Dilakukan perulangan for, untuk memasukkan string ke dalam file karakter tunggal pada satu waktu.
- Setelah log selesai, maka `fclose()` tereksekusi.

```c
    for (int i = 0; text[i] != '\0'; i++)
    {
        fputc(text[i], fp);
    }
    fclose(fp);
```

- Proses pada fungsi 'buatLogrename'  hampir sama dengan proses yang telah dijelaskan sebelumnya, yang membedakan adalah adanya penambahan variabel `from` dan `to`.
- Variabel ini berfungsi sebagai penampung nilai yang akan dicetak dari file yang telah di rename.
- Variabel 'from' berfungsi untuk menampung nama asli file (nama sebelum di-*rename*).
- Variabe 'to' berfungsi untuk menampung nama file setelah di-*rename*.

```c
void buatLogrename(char from[100], char to[100])
{
    char text[200];
    FILE *fp = fopen(LOG, "a");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(text, "INFO::%02d%02d%04d-%02d:%02d:%02d::RENAME::%s::%s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
            from, to);
    for (int i = 0; text[i] != '\0'; i++)
    {
        fputc(text[i], fp);
    }
    fclose(fp);
}
```

- Proses pada fungsi 'log_info' juga hampir mirip dengan 2 fungsi yang telah dijelaskan sebelumnya, yang membedakan adalah  hanya strukturnya.
- Sebenaenya fungsi `buatLog` hanya dikhususkan untuk LEVEL `WARNING`.
- Fungsi ini dibuat untuk  beberapa systemcall yang tidak dapat dipanggil dengan fungsi `buatLog`.
- Fungsi ini hanya diperuntukan LEVEL `INFO`.

```c
void log_info(char *desc, char *path)
{
    FILE *file_log = fopen(LOG, "a");

    time_t rawtime = time(NULL);
    struct tm tm = *localtime(&rawtime);

    int tahun = tm.tm_year + 1900;
    int bulan = tm.tm_mon + 1;
    int hari = tm.tm_mday;
    int jam = tm.tm_hour;
    int menit = tm.tm_min;
    int detik = tm.tm_sec;

    fprintf(file_log, "INFO::%02d%02d%04d-%02d:%02d:%02d::%s::%s\n",
            hari, bulan, tahun, jam, menit, detik, desc, path);
    fclose(file_log);
}

```

Ketiga fungsi diatas akan dipanggil pada masing-masing fungsi fuse sesuai dengan kebutuhan yang diperlukan, semisal :
- Untuk `buatLog()`
```c
buatLog(temp, ke);
```
- Untuk `buatLogrename()`
```c
buatLogrename(dari, ke);
```
- Untuk `log_info()`
```c
log_info("OPEN", ke);
```

**Screenshot**



## Kendala
