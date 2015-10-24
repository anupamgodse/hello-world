#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "2.h"
int main(int argc, char **argv) {
	DIR *dp;
	dirent *dir;
	myplan mp;
	initmyplan(&mp);
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
	//iff exists open the directory;
	//create mydir;
	mydir md;
	//at this state we dont know whether we have any files or directories present in this directory. Hence nhead = nprev = NULL)
	initmydir(&md);
	//This is root director.
	md.mprev = md.mnext = NULL;
	//preserve its name
	md.myname = (char *)malloc(strlen(argv[1]) + 1);
	strcpy(md.myname, argv[1]);
	//make head(and tail) point to this directory;(myplan)
	mp.head = mp.tail = &md;
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			stat(dir->d_name, &fstat);
			//identify dirent as file or directory;
			if(!(S_ISDIR(fstat.st_mode))) {
				//iff a dirent is a file(implies the file is present in given directory) 
				//create node and link file dirent to this node.
				printf("%s is a file\n", dir->d_name);
				node mn;
				initnode(&mn);
				mn.nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn.nname, dir->d_name);
				appendmydir(&md, &mn);
			}
			else if(S_ISDIR(fstat.st_mode)){
				//at this state we dont know whether we have any files or directories present in this directory. Hence nhead = nprev = mnext = NULL(mprev = NULL)
				//go inside the directory, and repeat the above procedure.
				//call sortdir, this function will be recursive and take all the files in data structure under specific directory.
				printf("%s is a folder\n", dir->d_name);
				sortdir(&mp, dir->d_name);
			}
		}
	}
//	now we have all directorires under myplan and all the files under specific mydir;
	//printmyplan(&mp);
	//printmydir(mp.head);
	printf("%s\t%s\n", mp.head->myname, mp.tail->myname);
	
	return 0;
}
void sortdir(myplan *mp, char *myname) {
	static int i;
	i++;
	printf("%d", i);
	dirent *dir;
	DIR *dp;
	struct stat fstat;
	chdir(mp->tail->myname);
	//printf("%s\n", mp->tail->myname);
	//open the req directory;
	printf("%s\n", myname);
	dp = opendir(myname);
	if (!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	//create new dir.
	mydir ndir;
	initmydir(&ndir);
	ndir.myname = (char *)malloc(strlen(myname) + 1);
	strcpy(ndir.myname, myname);
	printf("%s\n", ndir.myname);
	//link this directory to mypaln.
	appendmyplan(mp, &ndir);
	while(dir = readdir(dp)) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			//printf("dir = %s\n", dir->d_name);
			stat(dir->d_name, &fstat);
			//identify dirent as file or directory;
			if(!(S_ISDIR(fstat.st_mode))) {
				//iff a dirent is a file(implies the file is present in given directory) 
				//create node and link file dirent to this node.
				node mn;
				initnode(&mn);
				mn.nname = (char *)malloc(strlen(dir->d_name) + 1);
				strcpy(mn.nname, dir->d_name);
				appendmydir(&ndir, &mn);
			}
			else if(S_ISDIR(fstat.st_mode)){
				//at this state we dont know whether we have any files or directories present in this directory. Hence nhead = nprev = mnext = NULL(mprev = NULL)
				//create new dir.
				mydir ndir;
				initmydir(&ndir);
				strcpy(ndir.myname, dir->d_name);
				//link this directory to mypaln.
				appendmyplan(mp, &ndir);
				//go inside the directory, and repeat the above procedure.
				//call sortdir, this function will be recursive and take all the files in data structure under specific directory.
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
		//mn->nprev = NULL;
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
	printf("tail%s\n", mp->tail->myname);
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
/*Now i will be having all the files. Now i will search the specified file by its name in myplan. once i got the first file I will compare
 *it using filename, filesize, md5 signature and byte by byte comparison. When i will get a duplicate file I will take its pointer in a  
 * new file pointer array. Like this I will have all the duplicate files and then i can do all desired oprations in them.
 */ 
