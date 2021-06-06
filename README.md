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
* [Kendala](#kendala)

---
## Soal 1


---
## Soal 2


---
## Soal 3


---
## Soal 4

``c
char *LOG = "/home/kali/SinSeiFS.log";

// Fungsi untuk membuat log
void buatLog(char process[100], char loc[100])
{
    char text[200];
    FILE *fp = fopen(LOG, "a");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (strcmp(process, "unlink") == 0)
    {
        sprintf(text, "WARNING::%02d%02d%04d-%02d:%02d:%02d::UNLINK::%s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, loc);
    }

    else if (strcmp(process, "mkdir") == 0)
    {
        sprintf(text, "INFO::%02d%02d%04d-%02d:%02d:%02d::MKDIR::%s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, loc);
    }
    else if (strcmp(process, "rmdir") == 0)
    {
        sprintf(text, "WARNING::%02d%02d%04d-%02d:%02d:%02d::RMDIR::%s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, loc);
    }
    for (int i = 0; text[i] != '\0'; i++)
    {
        fputc(text[i], fp);
    }
    fclose(fp);
}

// Fungsi untuk membuat log khusus proses rename
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

``

--
## Kendala
- 
-
- 
  
