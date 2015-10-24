#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "2.h"
int main(int argc, char **argv) {
	DIR *dp;
	dirent *dir;
	myplan *mp = (myplan *)malloc(sizeof(myplan));
	initmyplan(mp);
	struct stat fstat;
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
	md->myname = (char *)malloc(strlen(argv[1]) + 1);
	strcpy(md->myname, argv[1]);
	mp->head = mp->tail = md;
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			if(dir->d_type == DT_REG) {
				printf("%s is a file in %s\n", dir->d_name, md->myname);
				node *mn = (node *)malloc(sizeof(node));
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn->nname, dir->d_name);
				appendmydir(md, mn);
			}
			else if(dir->d_type == DT_DIR){
				printf("%s is a folder in %s\n", dir->d_name, md->myname);
				sortdir(mp, dir->d_name);
			}
		}
	}
	printmyplan(mp);
	printf("%s\t%s\n", mp->head->myname, mp->tail->myname);
	return 0;
}
void sortdir(myplan *mp, char *myname) {
	dirent *dir;
	DIR *dp;
	struct stat fstat;
	chdir(mp->tail->myname);
	dp = opendir(myname);
	if (!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	mydir *ndir = (mydir *)malloc(sizeof(mydir));
	initmydir(ndir);
	ndir->myname = (char *)malloc(strlen(myname) + 1);
	strcpy(ndir->myname, myname);
	appendmyplan(mp, ndir);
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			stat(dir->d_name, &fstat);
			if(dir->d_type == DT_REG) {
				printf("%s is a file in %s\n", dir->d_name, ndir->myname);
				node *mn = (node *)malloc(sizeof(node));
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn->nname, dir->d_name);
				appendmydir(ndir, mn);
			}
			else if(dir->d_type == DT_DIR){
				printf("%s is a folder in %s\n", dir->d_name, ndir->myname);
				sortdir(mp, dir->d_name);
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
		printf("append %s to %s\n", mn->nname, md->myname);
		md->nhead = mn;
	}
	else {
		printf("append %s to %s\n", mn->nname, md->myname);
		md->ntail->nnext = mn;
	}
	mn->nprev = md->ntail;
	md->ntail = mn;
}
void appendmyplan(myplan *mp, mydir *md) {
	md->mnext = NULL;
	md->mprev = mp->tail;
	mp->tail->mnext = md;
	printf("append %s to myplan\n", md->myname);
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
		printf("\n");
		p = p->mnext;
	}
}
void printmydir(mydir *md) {
	node *p = md->nhead;
	while(p){
		printf("%s\t", p->nname);
		p = p->nnext; 
	}
}
