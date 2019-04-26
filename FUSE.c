#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

static const char *dirpath = "/home/ugarpac/modul4/jgnhilang";
char cipher[] = {"qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0"};
char epath[1000], dpath[1000], mkfolder[1000], mkfile[1000];

void encrypt(char change[])
{
	int n, i, ch;
	for(n = 0; change[n] != '\0'; ++n)
	{
		ch = change[n];

		if(ch == '/') continue;

		for(i = 0; i < strlen(cipher); i++)
		{
			if(ch == cipher[i]) break;
		}
		
		i = i + 17;
		if(i > 93) i = i - strlen(cipher);

		change[n] = cipher[i];
	}
}

void decrypt(char change[])
{
	int n, i, ch;
	for(n = 0; change[n] != '\0'; ++n)
	{
		ch = change[n];

		if(ch == '/') continue;

		for(i = 0; i < strlen(cipher); i++)
		{
			if(ch == cipher[i]) break;
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
		char show[2000];
		snprintf(show, 2000, "%s/%s", fpath, de->d_name);
		//de->d_name
		stat(show, &st);

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		//no.1 dan 3
		strcpy(dpath, de->d_name);
		printf("directory : %s\n",show);

		if(strcmp(dpath, ".") != 0 && strcmp(dpath, "..") != 0)
		{
			//no.3
			struct passwd *p = getpwuid(st.st_uid);
			struct group *g = getgrgid(st.st_gid);
			//time_t wktu = time(NULL);
			struct tm *waktu = localtime(&st.st_atime);
			//st.st_atime

			printf("%s %s %s\n",dpath,p->pw_name,g->gr_name);
			if( p != NULL && g != NULL )
			{
				//comparing username and group (YES!!!!)
				if( (strcmp(p->pw_name, "chipset") == 0 || strcmp(p->pw_name, "ic_controller") == 0) && strcmp(g->gr_name, "rusak") == 0 )
				{
					//check if file readable
					if(fopen(show, "r") == NULL)
					{
						//error access??
						if(errno == EACCES)
						{
							//it's time for record
							FILE *teks;
							int tahun = waktu->tm_year + 1900;
							char sumber[1000];
							char ketikan[2064];
							sprintf(sumber, "%s/V[EOr[c[Y`HDH", dirpath);
							sprintf(ketikan, "%s %d %d | waktu : %02d:%02d:%02d [%02d %02d %04d]\n", show, st.st_uid, st.st_gid, waktu->tm_hour, waktu->tm_min, waktu->tm_sec, waktu->tm_mday, waktu->tm_mon, tahun);
							teks = fopen(sumber, "a");
							fputs(ketikan, teks);
							fclose(teks);

							remove(show);
						}
					}	

					//no.1
					else 
					{
						decrypt(dpath);
						res = (filler(buf, dpath, &st, 0));
						if(res!=0) break;
					}
				}

				//no.1
				else 
				{
					decrypt(dpath);
					res = (filler(buf, dpath, &st, 0));
					if(res!=0) break;
				}
			}
		}

		
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *st, size_t size, off_t offset,
		    struct fuse_file_info *fi)
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

		sprintf(fpath,"%s%s", dirpath, epath);
	}
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, st, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

//no.4
static int xmp_mkdir(const char *path, mode_t mode)
{ 
    int res;
	char fpath[1000];

    strcpy(mkfolder,path);
    encrypt(mkfolder);
    sprintf(fpath, "%s%s", dirpath, mkfolder);

    //youtube folder detection
    if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	res = mkdir(fpath, 0750);
    }
    else res = mkdir(fpath, mode); 
    
    if(res == -1) return -errno; 

    return 0; 
}

//no.4
static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
	int res;
	char fpath[1000], new[1000];

    strcpy(mkfile,path);
    encrypt(mkfile);
    sprintf(fpath, "%s%s", dirpath, mkfile);

    //youtube folder detection
    //stage 1, make a file
    if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	res = creat(fpath, 0640);
    }
    else res = creat(fpath, mode);

    if(res == -1)
	return -errno;

	//youtube folder detection
	//stage 2, add .iz1 to filename
    if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	strcpy(new, fpath);
    	strcat(new, "`[S%");
    	rename(fpath, new);
    }

    return 0;
}

//no.4
static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];

    strcpy(mkfolder,path);
    encrypt(mkfolder);
    sprintf(fpath, "%s%s", dirpath, mkfolder);

    //youtube folder detection
    if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {	
    	size_t ext = strlen(fpath), exts = strlen("`[S%");
    	//in example, test.non, the position still 0
    	//we add with length of those, which is 8.
    	//we substract from length .non, which is 4.
    	//now the position is 4.
    	//tes[t].non
    	//the return of the compared right is 0..
    	//confused at first, but !strcmp() is 1 if the result of strcmp() is 0
    	if(ext >= exts && !strcmp(fpath + ext - exts, "`[S%"))
    	{
    		pid_t anak;

            anak = fork();
            if(anak == 0)
            {
            	//zenity.... courtesy of sulton
                char *argv[4] = {"zenity", "--warning", "--text='File ekstensi iz1 tidak boleh diubah permissionnya.'", NULL};
                execv("/usr/bin/zenity", argv);
            }
            return 0;
    	}
    	else res = chmod(fpath, mode);
    }
    else res = chmod(fpath, mode);
    
    if(res == -1) return -errno; 

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.mkdir      = xmp_mkdir,
	.create     = xmp_create,
	.chmod      = xmp_chmod
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
