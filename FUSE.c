#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/ugarpac/modul4/jgnhilang";
char cipher[] = {"qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0"};
char epath[1000], dpath[1000];

void encrypt(char change[])
{
	int n, i, ch, counter;
	for(n = 0; change[n] != '\0'; ++n)
	{
		ch = change[n];
		counter = -1;

		if(ch == '/') continue;

		for(i = 0; i < strlen(cipher); i++)
		{
			if(ch == cipher[i])
			{
				counter = i;
				break;
			}
		}
		
		i = i + 17;
		if(i > 93) i = i - strlen(cipher);

		change[n] = cipher[i];
	}
}

void decrypt(char change[])
{
	int n, i, ch, counter;
	for(n = 0; change[n] != '\0'; ++n)
	{
		ch = change[n];
		counter = -1;

		if(ch == '/') continue;

		for(i = 0; i < strlen(cipher); i++)
		{
			if(ch == cipher[i])
			{
				counter = i;
				break;
			}
		}
		
		i = i - 17;
		if(i < 0) i = i + strlen(cipher);

		change[n] = cipher[i];
	}
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];

	//no.1
	strcpy(epath, path);
	if(strcmp(epath, ".") != 0 && strcmp(epath, "..") != 0)encrypt(epath);

	sprintf(fpath,"%s%s", dirpath, epath);
	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else
	{
		//no.1
		strcpy(epath, path);
		if(strcmp(epath, ".") != 0 && strcmp(epath, "..") != 0)encrypt(epath);
		sprintf(fpath, "%s%s", dirpath, epath);
	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL)
	{
		struct stat st;
		memset(&st, 0, sizeof(st));

		//show
		char show[1000];
		snprintf(show, 1000, "%s/%s", dirpath, de->d_name);
		stat(show, &st);

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		//no.1
		strcpy(dpath, de->d_name);
		if(strcmp(dpath, ".") != 0 && strcmp(dpath, "..") != 0)decrypt(dpath);

		res = (filler(buf, dpath, &st, 0));
		if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}