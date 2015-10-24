#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "md5.h"
#include "4.h"
int main(int argc, char **argv) {
	DIR *dp;
	dirent *dir;
	myplan *mp = (myplan *)malloc(sizeof(myplan));
	initmyplan(mp);
	if(argc == 1) {
		perror("No directory specified");
		exit(errno);
	}
	dp = opendir(argv[1]);
	if(!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	mydir *md = (mydir *)malloc(sizeof(mydir));
	initmydir(md);
	md->mprev = md->mnext = NULL;
	md->myname = (char *)malloc(strlen(argv[1]) + 2);
	strcpy(md->myname, argv[1]);
	strcat(md->myname, "/");
	mp->head = mp->tail = md;
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			if(dir->d_type == DT_REG) {
				//printf("%s is a file in %s\n", dir->d_name, md->myname);
				node *mn = (node *)malloc(sizeof(node));
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn->nname, dir->d_name);
				appendmydir(md, mn);
			}
			else if(dir->d_type == DT_DIR){
				//printf("%s is a folder in %s\n", dir->d_name, md->myname);
				char *myname = (char *)malloc(strlen(md->myname) + strlen(dir->d_name) + 2);
				strcpy(myname, md->myname);
				strcat(myname, dir->d_name);
				sortdir(mp, myname);
			}
		}
	}
	printmyplan(mp);
	mydir *p = mp->head;
	while(p) {
		hashfiles(p);
		p = p->mnext;
	}
	//checkdup(mp);
	return 0;
}
void sortdir(myplan *mp, char *myname) {
	dirent *dir;
	DIR *dp;
	dp = opendir(myname);
	if (!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	mydir *ndir = (mydir *)malloc(sizeof(mydir));
	initmydir(ndir);
	ndir->myname = (char *)malloc(strlen(myname) + 2);
	strcpy(ndir->myname, myname);
	strcat(ndir->myname, "/");
	free(myname);
	appendmyplan(mp, ndir);
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			if(dir->d_type == DT_REG) {
				//printf("%s is a file in %s\n", dir->d_name, ndir->myname);
				node *mn = (node *)malloc(sizeof(node));
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn->nname, dir->d_name);
				appendmydir(ndir, mn);
			}
			else if(dir->d_type == DT_DIR) {
				//printf("%s is a folder in %s\n", dir->d_name, ndir->myname);
				char *myname = (char *)malloc(strlen(ndir->myname) + strlen(dir->d_name) + 2);
				strcpy(myname, ndir->myname);
				strcat(myname, dir->d_name);
				sortdir(mp, myname);
			}
		}
	}
}
void initmyplan(myplan *mp) {
	mp->head = mp->tail = NULL;
	return;	
}
void initmydir(mydir *md) {
	md->nhead = md->ntail = NULL;
	return;
}
void initnode(node *mn) {
	mn->nnext = mn->nprev = NULL;
}
void appendmydir(mydir *md, node *mn) {
	mn->nnext = NULL;
	if(!(md->nhead)) {
		//printf("append %s to %s\n", mn->nname, md->myname);
		md->nhead = mn;
	}
	else {
		//printf("append %s to %s\n", mn->nname, md->myname);
		md->ntail->nnext = mn;
	}
	mn->nprev = md->ntail;
	md->ntail = mn;
}
void appendmyplan(myplan *mp, mydir *md) {
	md->mnext = NULL;
	md->mprev = mp->tail;
	mp->tail->mnext = md;
	//printf("append %s to myplan\n", md->myname);
	mp->tail = md;
}
void printmyplan(myplan *mp) {
	mydir *p = mp->head;
	node *q;
	while(p) {
		printf("%s\t", p->myname);
		q = p->nhead;
		while(q) {
			printf("%s\t", q->nname);
			q = q->nnext; 
		}
		printf("\n\n\n");
		p = p->mnext;
	}
}
/*void printmydir(mydir *md) {
	node *p = md->nhead;
	while(p){
		printf("%s\t", p->nname);
		p = p->nnext; 
	}
}*/
void hashfiles(mydir *md) {
	int level;
	char cwd[1024];
	node *p = md->nhead;
	chdir(md->myname);
	while(p) {
		p->fsize = filesize(p->nname);
		printf("%ld\n", p->fsize);
		strcpy(p->signature, getsignature(p->nname));
		printf("%s\n", p->signature);
		p = p->nnext;
	}
	level = getlevel(md->myname);
	while((level--) > 0) 
		chdir("..");
}
char *getsignature(char *filename) {
	off_t fsize, x;
	unsigned char signature[33], temp;
	unsigned char c; 
	FILE *file;
	fsize = filesize(filename);
	file = fopen(filename, "rb");
	if (file == NULL) {
		perror("error opening file\n");
		exit(errno);
	}
	if(fsize % 32 == 0)
		x = fsize / 32;
	else
		x = (fsize / 32) + 1;
	while (x  < fsize) {
		if (fread(&c, 1, 1, file) != 1) {
			perror("error reading from file\n");
			exit(errno);
		}
		snprintf(temp, 4, "%d", c);
		
	}
	fclose(file);
	return signature;
}
off_t filesize(char *filename) {
  	struct stat s;
 	if (stat(filename, &s) != 0) 
 	 	return -1;
 	return s.st_size;
}
int getlevel(char *name) {
	int n, i, level = 0;
	n = strlen(name);
	for(i = 0; i < n; i++) 
		if(name[i] == '/')
			level++;
	return level;
}
