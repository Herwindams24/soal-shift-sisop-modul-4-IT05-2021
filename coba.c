#define FUSE_USE_VERSION 28
#include <cs50.h>
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

// #define key 10
#define rotkey 13

static const char *dirpath = "/home/carlo/Downloads";

char lower[] = "abcdefghijklmnopqrstuvwxyz";
char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void encrypt_v1(char *str) {
	int str_length = strlen(str);
	int begin = 0;

	for(int i = strlen(str); i >= 0; i--) {
		if(str[i] == '/') {
			break;
		}
		else if(str[i] == '.') {
			 str_length = i;
		}
	}

	for(int i = 1; i < str_length; i++) {
		if(str[i] == '/') {
			begin = i;
		}
	}

	for(int i = begin; i < str_length; i++) {
		if(str[i] == '/') {
			continue;
		}
		if(!((str[i]>=0&&str[i]<65)||(str[i]>90&&str[i]<97)||(str[i]>122&&str[i]<=127)))
		{
			if(str[i]>='A'&&str[i]<='Z')
			str[i] = 'Z'+'A'-str[i];
			if(str[i]>='a'&&str[i]<='z')
			str[i] = 'z'+'a'-str[i];
		}
	}
}

void decrypt_v1(char *str) {
	int str_length = strlen(str);
	int begin = 0;

	for(int i = 1; i < str_length; i++) {
		if(str[i] == '/' || str[i+1] == '\0') {
			begin = i+1;
			break;
		}
	}

	for(int i = strlen(str); i >= 0; i--) {
		if(str[i] == '/') {
			break;
		}
		else if(str[i] == '.' && i == (strlen(str)-1)) {
			str_length = strlen(str);
			break;
		}
		else if(str[i] == '.' && i != (strlen(str)-1)) {
			str_length = i;
			break;
		}
	}

	for(int i = begin; i < str_length; i++) {
		if(str[i] == '/') {
			continue;
		}
		if(!((str[i]>=0&&str[i]<65)||(str[i]>90&&str[i]<97)||(str[i]>122&&str[i]<=127)))
		{
			if(str[i]>='A'&&str[i]<='Z')
			str[i] = 'Z'+'A'-str[i];
			if(str[i]>='a'&&str[i]<='z')
			str[i] = 'z'+'a'-str[i];
		}
	}
}

void encrypt_v2(char *str) {
	int str_length = strlen(str);
	int begin = 0, idx;
	char *ptr;

	for(int i = strlen(str); i >= 0; i--) {
		if(str[i] == '/') {
			break;
		}
		else if(str[i] == '.') {
			str_length = i;
		}
	}

	for(int i = 1; i < str_length; i++) {
		if(str[i] == '/') {
			begin = i;
		}
	}

	for(int i = begin; i < str_length; i++) {
		if(str[i] == '/') {
			continue;
		}
		if(str[i] >= 'a' && str[i] <= 'z'){
			ptr = strchr(lower, str[i]);
			if(ptr) {
				idx = ptr - lower;
				str[i] = lower[(idx + rotkey) % 26];
			}
		}
		else if(str[i] >= 'A' && str[i] <= 'Z'){
			ptr = strchr(upper, str[i]);
			if(ptr) {
				idx = ptr - upper;
				str[i] = upper[(idx + rotkey) % 26];
			}
		}
	}
}

void decrypt_v2(char *str) {
	int str_length = strlen(str);
	int begin = 0, idx;
	char *ptr;

	for(int i = 1; i < str_length; i++) {
		if(str[i] == '/' || str[i+1] == '\0') {
			begin = i+1;
			break;
		}
	}

	for(int i = strlen(str); i >= 0; i--) {
		if(str[i] == '/') {
			break;
		}
		else if(str[i] == '.' && i == (strlen(str)-1)) {
			str_length = strlen(str);
			break;
		}
		else if(str[i] == '.' && i != (strlen(str)-1)) {
			str_length = i;
			break;
		}
	}

	for(int i = begin; i < str_length; i++) {
		if(str[i] == '/') {
			continue;
		}
		if(str[i] >= 'a' && str[i] <= 'z'){
			ptr = strchr(lower, str[i]);
			if(ptr) {
				idx = ptr - lower - rotkey;
				if(idx < 0) {
					idx += 26;
				}
				str[i] = lower[idx];
			}
		}
		else if(str[i] >= 'A' && str[i] <= 'Z'){
			ptr = strchr(upper, str[i]);
			if(ptr) {
				idx = ptr - upper - rotkey;
				if(idx < 0) {
					idx += 26;
				}
				str[i] = upper[idx];
			}
		}
	}
}

char *LOG = "/home/carlo/SinSeiFS.log";

void log_warning(char* desc, char* path) {
	FILE *file_log = fopen(LOG, "a");

	time_t rawtime = time(NULL);
	struct tm tm = *localtime(&rawtime);

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

	/*
    fprintf(file_log, "WARNING::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n", 
		tahun, bulan, hari, jam, menit, detik, desc, path);
    */
    fprintf(file_log, "WARNING::%02d%02d%04d-%02d:%02d:%02d::%s::%s\n", 
		hari, bulan, tahun, jam, menit, detik, desc, path);

	fclose(file_log);
}

void log_info(char* desc, char* path) {
	FILE *file_log = fopen(LOG, "a");

	time_t rawtime = time(NULL);
	struct tm tm = *localtime(&rawtime);

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

	fprintf(file_log, "INFO::%02d%02d%04d-%02d:%02d:%02d::%s::%s\n", 
		hari, bulan, tahun, jam, menit, detik, desc, path);
	fclose(file_log);
}

/*
//Khusus Rename

void log_rename(char from[100], char to[100])
{
    FILE *file_log = fopen(LOG, "a");

	time_t rawtime = time(NULL);
	struct tm tm = *localtime(&rawtime);

    char text[200];

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

    sprintf(text, "INFO::%02d%02d%04d-%02d:%02d:%02d::RENAME::%s::%s\n", 
        hari, bulan, tahun, jam, menit, detik, from, to);
    for (int i = 0; text[i] != '\0'; i++)
} 

*/

static int xmp_getattr(const char *path, struct stat *stbuf) {
    	int res;
    	char fpath[1000], temp[1000];
	
	strcpy(temp, path);
	// /Documents/AtoZ_rhs/FOTO_PENTING/kelincilucu.jpg
	// membandingkan 7 karakter pertama
	if(strncmp(path, "/AtoZ_", 6) == 0) {
		decrypt_v1(temp);
	}
    if(strncmp(path, "/RX_", 4) == 0) {
        decrypt_v1(temp);
		decrypt_v2(temp);
	}

	sprintf(fpath, "%s%s", dirpath, temp);
	// printf("getattr fpath : %s\n", fpath);
	res = lstat(fpath, stbuf);

    	if(res == -1) return -errno;
	log_info("LS", fpath);

   	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    	char fpath[1000], temppath[1000];
	
    	if(strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		strcpy(temppath, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temppath);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temppath);
			decrypt_v2(temppath);
		}

		sprintf(fpath, "%s%s", dirpath, temppath);
	}
	// printf("readdir fpath : %s\n", fpath);
	
    	int res = 0;

   	DIR *dp;
    	struct dirent *de;

    	(void) offset;
    	(void) fi;

    	dp = opendir(fpath);

    	if(dp == NULL) return -errno;

   	while ((de = readdir(dp)) != NULL) {
        	struct stat st;
	        memset(&st, 0, sizeof(st));
        	st.st_ino = de->d_ino;
        	st.st_mode = de->d_type << 12;
		//titik sama dua titik direktori
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
			continue;
		}

		char temp[1000];
		
		strcpy(temp, de->d_name);	// ngambil nama file doang
		// printf("ini temp = %s\n", temp);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			encrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            encrypt_v1(temp);
			encrypt_v2(temp);
		}
		res = (filler(buf, temp, &st, 0));

        	if (res != 0) break;
    	}
	closedir(dp);
	log_info("CD", fpath);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    	char fpath[1000];
    	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}
		sprintf(fpath, "%s%s", dirpath, temp);
	}
	printf("read fpath : %s\n", fpath);
    	int res = 0;
    	int fd = 0;

    	(void)fi;

    	fd = open(fpath, O_RDONLY);

    	if (fd == -1) return -errno;

    	res = pread(fd, buf, size, offset);

    	if (res == -1) res = -errno;

    	close(fd);
    	return res;
}

static int xmp_truncate(const char *path, off_t size) {
	char fpath[1000];
	
	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}
		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	res = truncate(fpath, size);
	if (res == -1) return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	char fpath[1000];
	
	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

    	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);

    	// logs(0, "WRITE", fpath, "");
	log_info("WRITE", fpath);
	return res;
}

static int xmp_unlink(const char *path) {
	char fpath[1000];

	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}
		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	res = unlink(fpath);
	if (res == -1) return -errno;

    	// logs(1, "UNLINK", fpath, "");
	log_warning("UNLINK", fpath);

	return 0;
}

static int xmp_rename(const char *from, const char *to) {
	char src[1000], dst[1000];
	// printf("ini from : %s\n", from);
	// printf("ini to : %s\n", to);
	if (strcmp(from, "/") == 0) {
        	from = dirpath;
        	sprintf(src, "%s", from);
    	} else {
		char tempa[1000];		//rename folder encv ke biasa
		strcpy(tempa, from);
		//ada atau gak ada folder AtoZ_
		if(strncmp(from, "/AtoZ_", 6) == 0) {
			decrypt_v1(tempa);
		}
        if(strncmp(from, "/RX_", 4) == 0) {
            decrypt_v1(tempa);
			decrypt_v2(tempa);
		}

		sprintf(src, "%s%s", dirpath, tempa);
	}

	if (strcmp(to, "/") == 0) {
        	to = dirpath;
        	sprintf(dst, "%s", to);
    	} else {
		char tempb[1000];		//rename dari biasa ke encv
		strcpy(tempb, to);

		if(strncmp(to, "/AtoZ_", 6) == 0) {
			decrypt_v1(tempb);		//dekrip yg didocomuents
		}
        if(strncmp(to, "/RX_", 4) == 0) {
            decrypt_v1(tempb);
			decrypt_v2(tempb);
		}

		sprintf(dst, "%s%s", dirpath, tempb);
	}
	
	int res;

	res = rename(src, dst);
	if (res == -1) return -errno;

	// logs(0, "RENAME", src, dst);
	log_info("RENAME", src);

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode) {
	char fpath[1000];
	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	res = mkdir(fpath, 0750);
	if (res == -1) return -errno;

	// logs(0, "MKDIR", fpath, "");
	log_info("MKDIR", fpath);

	return 0;
}

static int xmp_rmdir(const char *path) {
	char fpath[1000];
	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);
		// /home/whitezhadow/Documents/AtoZ_rhs/FOTO_PENTING/kelincilucu.jpg
		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	res = rmdir(fpath);
	if (res == -1) return -errno;

	// logs(1, "RMDIR", fpath, "");
	log_warning("RMDIR", fpath);

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
	char fpath[1000];

	if (strcmp(path, "/") == 0) {
	        path = dirpath;
	        sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}
		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	res = open(fpath, fi->flags);
	if (res == -1) return -errno;

	close(res);
	
   	// logs(0, "OPEN", fpath, "");
	log_info("OPEN", fpath);

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
	char fpath[1000];

	if (strcmp(path, "/") == 0) {
        	path = dirpath;
        	sprintf(fpath, "%s", path);
    	} else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/AtoZ_", 6) == 0) {
			decrypt_v1(temp);
		}
        if(strncmp(path, "/RX_", 4) == 0) {
            decrypt_v1(temp);
			decrypt_v2(temp);
		}
		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) {
			res = close(res);
		}
	} else if (S_ISFIFO(mode)) {
		res = mkfifo(fpath, mode);
	} else {
		res = mknod(fpath, mode, rdev);
	}
	
	if (res == -1) return -errno;

	// logs(0, "CREATE", fpath, "");
	log_info("CREATE", fpath);

	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr 	= xmp_getattr,
    .readdir 	= xmp_readdir,
    .read 	= xmp_read,
	.truncate   = xmp_truncate,
	.write      = xmp_write,
	.unlink     = xmp_unlink,
	.rename     = xmp_rename,
	.mkdir	    = xmp_mkdir,
	.rmdir	    = xmp_rmdir,
	.open       = xmp_open,
	.mknod 	    = xmp_mknod,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
