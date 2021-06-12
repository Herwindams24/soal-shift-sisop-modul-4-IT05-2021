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
## Pendahuluan

Sebelum memulai pengerjaan soal, kelompok kami membuat FUSE filesystem yang dapat berjalan seperti file system pada umumnya dengan mengimplementasikan beberapa system-call.

```c
static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .truncate = xmp_truncate,
    .write = xmp_write,
    .unlink = xmp_unlink,
    .rename = xmp_rename,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .open = xmp_open,
    .mknod = xmp_mknod,
};
```

- `xmp_getattr()`: untuk mendapatkan stat dari path yang diinputkan.
- `xmp_readdir()`: untuk membaca folder dari path.
- `xmp_read()`: untuk membaca file dari path.
- `xmp_truncate()`: untuk melakukan truncate (membesarkan atau mengecilkan size) dari path.
- `xmp_write()`: untuk menulis kedalam path.
- `xmp_unlink()`: untuk menghapus sebuah file pada path.
- `xmp_rename()`: untuk me-rename dari path awal menjadi path tujuan.
- `xmp_mkdir()`: untuk membuat direktori pada path.
- `xmp_rmdir()`: untuk menghapus directory pada path.
- `xmp_open()`: untuk meng-open (membuka) path.
- `xmp_mknod()`: untuk membuat filesytem node pada path.

### System-call getattr

System-call akan menerima parameter path dan stbuf. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_. Selanjutnya dilakukan `lstat(loc, stbuf)` dengan `loc` dan `stbuf` sebagai parameternya. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `LS`

```c
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char loc[1000], temp[1000];
    char ke[1000];
    sprintf(ke, "%s", path);
    strcpy(temp, path);
    // /Documents/AtoZ_rhs/FOTO_PENTING/kelincilucu.jpg
    // membandingkan 7 karakter pertama
    if (strncmp(path, "/AtoZ_", 6) == 0)
    {
        decrypt_v1(temp);
    }
    if (strncmp(path, "/RX_", 4) == 0)
    {
        decrypt_v1(temp);
        decrypt_v2(temp);
    }

    sprintf(loc, "%s%s", dirpath, temp);
    // printf("getattr loc : %s\n", loc);
    res = lstat(loc, stbuf);

    if (res == -1)
        return -errno;
    log_info("LS", ke);

    return 0;
}
```

### System-call readdir

System-call akan menerima beberapa parameter. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan di dalam variabel `loc`. Selanjutnya dilakukan `opendir()` dengan `loc` sebagai parameternya. System-call akan melakukan `encrypt_v1()` untuk path yang berawalan AtoZ_ dan `encrypt_v1()` + `encrypt_v2()` untuk path yang berawalan RX_, lalu hasil enskripsinya akan dilakukan `filler()`. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `CD`

```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char loc[1000], temppath[1000];
    char ke[1000];
    sprintf(ke, "%s", path);
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        strcpy(temppath, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temppath);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temppath);
            decrypt_v2(temppath);
        }

        sprintf(loc, "%s%s", dirpath, temppath);
    }

    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(loc);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        //titik sama dua titik direktori
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            continue;
        }

        char temp[1000];

        strcpy(temp, de->d_name); // ngambil nama file doang
        // printf("ini temp = %s\n", temp);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            encrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            encrypt_v1(temp);
            encrypt_v2(temp);
        }
        res = (filler(buf, temp, &st, 0));

        if (res != 0)
            break;
    }
    closedir(dp);
    log_info("CD", ke);
    return 0;
}
```

### System-call read

System-call akan menerima beberapa parameter. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `open(loc, O_RDONLY)` dan `pread(fd, buf, size, offset)` yang disimpan dalam variabel `res`, lalu dikembalikan.

```c
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char loc[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }
        sprintf(loc, "%s%s", dirpath, temp);
    }
    printf("read loc : %s\n", loc);
    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(loc, O_RDONLY);

    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}
```

### System-call truncate

System-call akan menerima parameter path dan off_t size. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `truncate(loc, size)` dengan `loc` dan `size` sebagai parameternya,  hasilnya disimpan dalam variabel `res`, lalu dikembalikan.

```c
static int xmp_truncate(const char *path, off_t size)
{
    char loc[1000];

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }
        sprintf(loc, "%s%s", dirpath, temp);
    }

    int res;
    res = truncate(loc, size);
    if (res == -1)
        return -errno;

    return 0;
}
```

### System-call write

System-call akan menerima beberapa parameter. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `open(loc, O_WRONLY)` dan `pwrite(fd, buf, size, offset)`, lalu hasilnya disimpan dalam variabel `res`. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `Write`

```c
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }

        sprintf(loc, "%s%s", dirpath, temp);
    }

    int fd;
    int res;

    (void)fi;
    fd = open(loc, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    // logs(0, "WRITE", loc, "");
    log_info("WRITE", ke);
    return res;
}
```

### System-call unlink

System-call akan menerima parameter path dan stbuf. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_. Selanjutnya dilakukan `unlink(loc)` dengan `loc` sebagai parameternya. Terakhir, system-call akan memanggil `buatLog()` untuk membuat Log dengan proses `UNLINK`

```c
static int xmp_unlink(const char *path)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }
        sprintf(loc, "%s%s", dirpath, temp);
    }

    char temp[] = "unlink";
    buatLog(temp, ke);

    int res;
    res = unlink(loc);
    if (res == -1)
        return -errno;

    // logs(1, "UNLINK", loc, "");
    //log_warning("UNLINK", loc);

    return 0;
}
```

### System-call rename

System-call akan menerima parameter `from` dan `to`. Pada kedua parameter yang diterima, system-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan di dalam variabel `src` dan `dst`. Selanjutnya dilakukan `rename(src, dst)` dengan `src` dan `dst` sebagai parameternya. Terakhir, system-call akan memanggil `buatLogrename()` untuk membuat Log dengan proses `RENAME`

```c
static int xmp_rename(const char *from, const char *to)
{
    char src[1000], dst[1000];

    char dari[1000], ke[1000];
    sprintf(dari, "%s", from);
    sprintf(ke, "%s", to);

    // printf("ini from : %s\n", from);
    // printf("ini to : %s\n", to);
    if (strcmp(from, "/") == 0)
    {
        from = dirpath;
        sprintf(src, "%s", from);
    }
    else
    {
        char tempa[1000]; //rename folder encv ke biasa
        strcpy(tempa, from);
        //ada atau gak ada folder AtoZ_
        if (strncmp(from, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(tempa);
        }
        if (strncmp(from, "/RX_", 4) == 0)
        {
            decrypt_v1(tempa);
            decrypt_v2(tempa);
        }

        sprintf(src, "%s%s", dirpath, tempa);
        //sprintf(src, "%s", tempa);
    }

    if (strcmp(to, "/") == 0)
    {
        to = dirpath;
        sprintf(dst, "%s", to);
    }
    else
    {
        char tempb[1000]; //rename dari biasa ke encv
        strcpy(tempb, to);

        if (strncmp(to, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(tempb); //dekrip yg didocomuents
        }
        if (strncmp(to, "/RX_", 4) == 0)
        {
            decrypt_v1(tempb);
            decrypt_v2(tempb);
        }

        sprintf(dst, "%s%s", dirpath, tempb);
        //sprintf(dst, "%s", tempb);
    }

    int res;

    res = rename(src, dst);
    if (res == -1)
        return -errno;
    buatLogrename(dari, ke);
    // logs(0, "RENAME", src, dst);
    //log_info("RENAME", src);
    //log_rename("RENAME", src);

    return 0;
}
```

### System-call mkdir

System-call akan menerima parameter path dan mode_t mode. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `mkdir(loc, 0750)` dengan `loc`sebagai parameternya. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `MKDIR`

```c
static int xmp_mkdir(const char *path, mode_t mode)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }

        sprintf(loc, "%s%s", dirpath, temp);
    }

    int res;
    res = mkdir(loc, 0750);
    if (res == -1)
        return -errno;

    // logs(0, "MKDIR", loc, "");
    log_info("MKDIR", ke);
    //char temp[] = "mkdir";
    //buatLog(temp, ke);
    return 0;
}
```

### System-call rmdir

System-call akan menerima parameter path. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `rmdir(loc)` dengan `loc` sebagai parameternya. Terakhir, system-call akan memanggil `buatLog()` untuk membuat Log dengan proses `RMDIR`

```c
static int xmp_rmdir(const char *path)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);
        // /home/whitezhadow/Documents/AtoZ_rhs/FOTO_PENTING/kelincilucu.jpg
        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }

        sprintf(loc, "%s%s", dirpath, temp);
    }

    int res;
    res = rmdir(loc);
    if (res == -1)
        return -errno;

    // logs(1, "RMDIR", loc, "");
    //log_warning("RMDIR", loc);
    char temp[] = "rmdir";
    buatLog(temp, ke);

    return 0;
}
```

### System-call open

System-call akan menerima beberapa parameter. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan `open(loc, fi->flags)` dengan `loc` dan `fi->flags` sebagai parameternya. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `OPEN`

```c
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }
        sprintf(loc, "%s%s", dirpath, temp);
    }

    int res;
    res = open(loc, fi->flags);
    if (res == -1)
        return -errno;

    close(res);

    // logs(0, "OPEN", loc, "");
    log_info("OPEN", ke);

    return 0;
}
```

### System-call mknod

System-call akan menerima beberapa parameter. System-call akan melakukan `decrypt_v1()` untuk path yang berawalan AtoZ_ dan `decrypt_v1()` + `decrypt_v2()` untuk path yang berawalan RX_, lalu hasilnya disimpan dalam variabel `loc`. Selanjutnya dilakukan pengecekan untuk mode yang digunakan apakah `S_ISREG` atau `S_ISFIFO`. Terakhir, system-call akan memanggil `log_info()` untuk membuat Log dengan proses `CREATE`


```c
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    char loc[1000];
    char ke[1000];
    sprintf(ke, "%s", path);
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(loc, "%s", path);
    }
    else
    {
        char temp[1000];
        strcpy(temp, path);

        if (strncmp(path, "/AtoZ_", 6) == 0)
        {
            decrypt_v1(temp);
        }
        if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temp);
            decrypt_v2(temp);
        }
        sprintf(loc, "%s%s", dirpath, temp);
    }

    int res;
    if (S_ISREG(mode))
    {
        res = open(loc, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
        {
            res = close(res);
        }
    }
    else if (S_ISFIFO(mode))
    {
        res = mkfifo(loc, mode);
    }
    else
    {
        res = mknod(loc, mode, rdev);
    }

    if (res == -1)
        return -errno;

    log_info("CREATE", ke);

    return 0;
}
```

---
## Soal 1

### Deskripsi Soal
- Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/user/Downloads/[Nama Direktori] → /home/user/Downloads/AtoZ_[Nama Direktori]
- Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

**Contoh Implementasi:**
```
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
```

### Pembahasan Enkripsi AtoZ

Proses enkripsi untuk AtoZ dilakukan dengan cara yaitu pertama, full path dari direktori yang akan di enkripsi akan di cek per char dari belakang, apabila char yang sedang dicek merupakan '/' akan di break. Untuk menghandle apabila file memiliki ekstensi, apabila terdapat '.' maka str_length akan diubah menjadi `i`, sehingga yang ter enkrip hanya nama file dan tanpa ekstensi.

```c
void encrypt_v1(char *str)
{
    int str_len = strlen(str);
    int mulai = 0;

    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == '/')
        {
            break;
        }
        else if (str[i] == '.')
        {
            str_len = i;
        }
    }
```

Untuk mengambil file atau direktori paling belakang pada path, dilakukan loop dimana setiap bertemu '/', variable `mulai` akan di set menjadi indeks dimana '/' tersebut berada.

```c
    for (int i = mulai; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            continue;
        }
    ...
    }
```

Kemudian dilakukan pemetaan tiap-tiap karakter sebagai hasil dari enkripsi dengan loop.

```c
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) || (str[i] > 122 && str[i] <= 127)))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = 'Z' + 'A' - str[i];
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = 'z' + 'a' - str[i];
        }
```

### Pembahasan Dekripsi AtoZ

Proses dekripsi untuk AtoZ dilakukan dengan cara yaitu pertama, full path akan dicek per char dari depan, apabila ditemukan '/' atau end of array \0, maka variable begin di set menjadi `i+1` lalu di break.

```c
void decrypt_v1(char *str)
{
    int str_len = strlen(str);
    int mulai = 0;

    for (int i = 1; i < str_len; i++)
    {
        if (str[i] == '/' || str[i + 1] == '\0')
        {
            mulai = i + 1;
            break;
        }
    }
```

Kemudian dilakukan loop untuk mengidentifikasi apakah file memiliki ekstensi atau tidak.

```c
for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == '/')
        {
            break;
        }
        else if (str[i] == '.' && i == (strlen(str) - 1))
        {
            str_len = strlen(str);
            break;
        }
        else if (str[i] == '.' && i != (strlen(str) - 1))
        {
            str_len = i;
            break;
        }
    }
```

Kemudian dilakukan pemetaan tiap-tiap karakter sebagai hasil dari enkripsi dengan loop.

```c
for (int i = mulai; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            continue;
        }
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) || (str[i] > 122 && str[i] <= 127)))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = 'Z' + 'A' - str[i];
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = 'z' + 'a' - str[i];
        }
    }
```

---
## Soal 2

### Deskripsi Soal
- Jika sebuah direktori dibuat dengan awalan “RX_”, maka direktori tersebut akan menjadi direktori ter-encode sesuai kasus nomor 1 dengan tambahan encode metode ROT13.
- Jika sebuah direktori di-rename dengan awalan “RX_”, maka direktori tersebut akan menjadi direktori ter-encode beserta isinya dengan algoritma Atbash +  Vigenere.
- Apabila direktori yang terenkripsi dihilangkan awalan “RX_”nya maka akan menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat pada file log
- File-file pada direktori asli akan terpecah menjadi file-file kecil sebesar 1024 bytes.

### Pembahasan Enkripsi RX_
Sebagai awal, terdapat deklarasi rotkey 13 yang akan mendefinisikan shift yang dilakukan sebanyak 13 kali, kemudian terdapat deklarasi character lower dan upper untuk memuat huruf-huruf yang memungkinkan enkripsi menjadi case sensitive.

```c
#define rotkey 13

static const char *dirpath = "/home/kali/Downloads";

char lower[] = "abcdefghijklmnopqrstuvwxyz";
char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
```

Proses enkripsi untuk RX dilakukan dengan cara yaitu pertama, full path dari direktori yang akan di enkripsi akan di cek per char dari belakang, apabila char yang sedang dicek merupakan '/' akan di break. Untuk menghandle apabila file memiliki ekstensi, apabila terdapat '.' maka str_length akan diubah menjadi `i`, sehingga yang ter enkrip hanya nama file dan tanpa ekstensi.

```c
    for (int i = mulai; i < str_len; i++)
    {
        if (str[i] == '/')
        {
            continue;
        }
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            ptr = strchr(lower, str[i]);
            if (ptr)
            {
                idx = ptr - lower;
                str[i] = lower[(idx + rotkey) % 26];
            }
        }
        else if (str[i] >= 'A' && str[i] <= 'Z')
        {
            ptr = strchr(upper, str[i]);
            if (ptr)
            {
                idx = ptr - upper;
                str[i] = upper[(idx + rotkey) % 26];
            }
        }
    }
```
Kemudian dilakukan if statement untuk menentukan apakah input text (nama file/folder awal) berupa huruf besar atau huruf kecil. Kemudian akan dilakukan loop untuk menggeser huruf sebanyak 13 langkah sesuai abjad.

### Pembahasan Dekripsi RX_
Proses dekripsi untuk RX dilakukan dengan cara yang kurang lebih sama, full path dari direktori yang akan di enkripsi akan di cek per char dari belakang, apabila char yang sedang dicek merupakan '/' akan di break. Untuk menghandle apabila file memiliki ekstensi, apabila terdapat '.' maka str_length akan diubah menjadi `i`, sehingga yang ter dekrip hanya nama file dan tanpa ekstensi.

```c
void decrypt_v2(char *str)
{
    int str_len = strlen(str);
    int mulai = 0, idx;
    char *ptr;

    for (int i = 1; i < str_len; i++)
    {
        if (str[i] == '/' || str[i + 1] == '\0')
        {
            mulai = i + 1;
            break;
        }
    }

    for (int i = strlen(str); i >= 0; i--)
    {
        if (str[i] == '/')
        {
            break;
        }
        else if (str[i] == '.' && i == (strlen(str) - 1))
        {
            str_len = strlen(str);
            break;
        }
        else if (str[i] == '.' && i != (strlen(str) - 1))
        {
            str_len = i;
            break;
        }
    }

```
Kemudian terdapat if statement untuk menentukan apakah input file (nama file/folder yang sebelumnya telah terenkripsi) merupakan huruf besar atau huruf kecil sehingga dapat menyesuaikan dalam proses dekripsi. Kemudian dilakukan penggeseran secara reverse untuk mengembalikan ke abjad awal sebanyak 13 shift.

Proses eksekusi program dilakukan dengan menjalankan terlebih dahulu algoritma atbash baru kemudian menjalankan algoritma Shift 13 baik pada enkripsi maupun pada dekripsi. Contoh implementasi pemanggilan fungsi dilakukan pada code brikut

```c
 if (strncmp(path, "/RX_", 4) == 0)
        {
            encrypt_v1(temp);
            encrypt_v2(temp);
        }
```
Sementara dalam proses dekripsi sebagai berikut

```c
if (strncmp(path, "/RX_", 4) == 0)
        {
            decrypt_v1(temppath);
            decrypt_v2(temppath);
        }

```

---
## Soal 3

Belum berhasil diselesaikan

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

## Screenshot
Inisiasi fuse dan isi foloder fuse
<img alt="inisiasi fuse" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/folder%20fuse.jpg">
Isi folder download
<img alt="folder download" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/folder%20download.jpg">
Rename folder target manjadi awalan AtoZ_
<img alt="Rename AtoZ_" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/Rename%20Atoz.jpg">
Folder terenkripsi dengan algoritma atbash
<img alt="Enkripsi AtoZ_" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/Enkripsi%20Atoz.jpg">
Rename folder target manjadi awalan RX_
<img alt="Rename RX_" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/Rename%20RX.jpg">
Folder terenkripsi dengan algoritma atbash + ROT13
<img alt="Enkripsi RX_" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/Enkripsi%20RX.jpg">
Folder terdekriopsi seperti semula setelah rename awalan dihilangkan
<img alt="Dekripsi" src="https://github.com/Herwindams24/soal-shift-sisop-modul-4-IT05-2021/blob/main/screenshots/Dekripsi.jpg">
VirtualBox
<img width="900" alt="VirtualBoxVM_nzL5z4jfKm" src="https://user-images.githubusercontent.com/57980125/121776811-195f5b00-cbb9-11eb-9816-3d1df88a25fc.png">
Log membuat folder baru
<img width="500" alt="mkdir" src="https://user-images.githubusercontent.com/57980125/121776710-9b9b4f80-cbb8-11eb-8997-4276a9a2263d.png">
Log membaca isi file
<img width="500" alt="open" src="https://user-images.githubusercontent.com/57980125/121776716-a3f38a80-cbb8-11eb-9935-352b018f409c.png">
Log membuat file baru
<img width="500" alt="write" src="https://user-images.githubusercontent.com/57980125/121776720-a9e96b80-cbb8-11eb-9674-507f7e0bbfb1.png">
Log melakukan penamaan ulang
<img width="500" alt="rename" src="https://user-images.githubusercontent.com/57980125/121776722-ac4bc580-cbb8-11eb-87b0-1dc0005d0ba9.png">
Log melakukan penghapusan file
<img width="500" alt="unlink" src="https://user-images.githubusercontent.com/57980125/121776726-aeae1f80-cbb8-11eb-8d34-8c06a882bfc5.png">
Log melakukan penghapusan folder
<img width="500" alt="rmdir" src="https://user-images.githubusercontent.com/57980125/121776727-b241a680-cbb8-11eb-8688-478bb477ef13.png">

## Kendala
- Soal 1 : Belum berhasil membuat log khusus untuk enkripsi-dekripsi AtoZ
- Soal 2 : Belum berhasil membuat fungsi vigenere ketika dilakukan rename serta membagi file ke dalam file-file kecil berukuran 1024mb
- Soal 3 : Belum dapat memahamai soal
- Soal 4 : -
