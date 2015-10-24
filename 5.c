#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "5.h"
int main(int argc, char **argv) {
	DIR *dp;
	dirent *dir;
	myplan *mp = (myplan *)malloc(sizeof(myplan));
	if(mp == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	initmyplan(mp);
	if(argc == 1) {
		perror("No directory specified");
		exit(0);
	}
	dp = opendir(argv[1]);
	if(!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	mydir *md = (mydir *)malloc(sizeof(mydir));
	if(md == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	initmydir(md);
	md->mprev = md->mnext = NULL;
	md->myname = (char *)malloc(strlen(argv[1]) + 2);
	if(md->myname == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	strcpy(md->myname, argv[1]);
	strcat(md->myname, "/");
	mp->head = mp->tail = md;
	while((dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			if(dir->d_type == DT_REG) {
				//printf("%s is a file in %s\n", dir->d_name, md->myname);
				node *mn = (node *)malloc(sizeof(node));
				if(mn == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				if(mn->nname == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				mn->mypath = (char *)malloc(strlen(dir->d_name) + strlen(md->myname) + 1);
				if(mn->mypath == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				strcpy(mn->mypath, md->myname);
				strcat(mn->mypath, dir->d_name);
				strcpy(mn->nname, dir->d_name);
				(mp->count)++;
				appendmydir(md, mn);
			}
			else if(dir->d_type == DT_DIR){
				//printf("%s is a folder in %s\n", dir->d_name, md->myname);
				char *myname = (char *)malloc(strlen(md->myname) + strlen(dir->d_name) + 2);
				if(myname == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				strcpy(myname, md->myname);
				strcat(myname, dir->d_name);
				sortdir(mp, myname);
			}
		}
	}
	linkbysize(mp);
	linkbyhashing(mp);
	mydir *p = mp->head;
	node *q;
	while(p) {
		q = p->nhead;
		while(q) {
			q->dup = NULL;
			q = q->nnext;
		}
		p = p->mnext;
	}
	finallink(mp);
	printmyplan(mp);
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
	if(ndir->myname == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	strcpy(ndir->myname, myname);
	strcat(ndir->myname, "/");
	free(myname);
	appendmyplan(mp, ndir);
	while((dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			if(dir->d_type == DT_REG) {
				//printf("%s is a file in %s\n", dir->d_name, ndir->myname);
				node *mn = (node *)malloc(sizeof(node));
				if(mn == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				initnode(mn);
				mn->nname = (char *)malloc(strlen(dir->d_name) + 1);
				if(mn->nname == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				mn->mypath = (char *)malloc(strlen(dir->d_name) + strlen(ndir->myname) + 1);
				if(mn->mypath == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				strcpy(mn->mypath, ndir->myname);
				strcat(mn->mypath, dir->d_name);
				strcpy(mn->nname, dir->d_name);
				(mp->count)++;
				appendmydir(ndir, mn);
			}
			else if(dir->d_type == DT_DIR) {
				//printf("%s is a folder in %s\n", dir->d_name, ndir->myname);
				char *myname = (char *)malloc(strlen(ndir->myname) + strlen(dir->d_name) + 2);
				if(myname == NULL) {
					printf("Out of Memory :\n");
					exit(1);
				}
				strcpy(myname, ndir->myname);
				strcat(myname, dir->d_name);
				sortdir(mp, myname);
			}
		}
	}
}
void initmyplan(myplan *mp) {
	mp->head = mp->tail = NULL;
	mp->count = 0;
	return;	
}
void initmydir(mydir *md) {
	md->nhead = md->ntail = NULL;
	return;
}
void initnode(node *mn) {
	mn->nnext = mn->nprev = mn->dup = mn->dupsign = NULL;
	mn->visited = 0;
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
/*void printmyplan(myplan *mp) {
	mydir *p = mp->head;
	node *q;
	int state;
	while(p) {
		//printf("%s\n", p->myname);
		q = p->nhead;
		state = 0;
		while(q) {
			//printf("%p\t%s\t%p\t",q, q->mypath, q->dupsign);
			if(q->dup && state != 1) {
				printf("%s\n%s\n", q->dup->mypath, q->mypath);
				state = 1;
			}
			else if(q->dup)
				printf("%s\n", q->mypath);
			//else 
			//	printf("\n");
			q = q->nnext; 
		}
		printf("\n\n\n");
		p = p->mnext;
	}
}*/
void printmyplan(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n;
	while(p) {
		q = p->nhead;
		while(q) {
			if(q->visited == 0) {
				printf("%s\n", q->mypath);
				n = q->dup;
				while(n) {
					printf("%s\n", n->mypath);
					n = n->dup;
				}
				printf("\n\n");
			}
			q = q->nnext; 
		}
		p = p->mnext;
	}
}
void linkbysize(myplan *mp) {
	mydir *p = mp->head;
	node *q;
	int i = 0, level, result, k;
	flink arr[mp->count];
	while(p) {
		q = p->nhead;
		chdir(p->myname);
		while(q) {
			q->fsize = filesize(q->nname);
			for(k = 0; k < i; k++) {
				if(arr[k].size == q->fsize) {
					result = k;
					break;	
				}
			}
			if(k == i)
				result = -1;
			if(result == -1) {
				arr[i].tail = q;
				arr[i].size = q->fsize;
				i++;
			}
			else {
				arr[result].tail->dup = q;
				arr[result].tail = q;
			}
			//printf("%ld\n", q->fsize);
			q = q->nnext;
		}
		level = getlevel(p->myname);
		while((level--) > 0) 
			chdir("..");
		p = p->mnext;	
	} 
	
}
void linkbyhashing(myplan *mp) {
	char *s[mp->count], *s1, *s2;
	mydir *p = mp->head;
	node *q, *n, *r;
	int i = 0, k, state;
	while(p) {
		q = p->nhead;	
		while(q) {
			state = 0;
			n = r = q;
			s1 = getsign(q->mypath);
			for(k = 0; k < i; k++) {  
				if(s[k] == s1) {
					q = q->nnext;
					state = 1;
					break;
				}
			}
			if(state == 1)
				continue;
			if(k == i && q->dup)
				s[i++] = s1;
			while(n->dup) {
				s2 = getsign(n->dup->mypath);
				if(strcmp(s1, s2) == 0) { 
					r->dupsign = n->dup;
					r = n->dup;
				}
				n = n->dup;
			}
		q = q->nnext;
		}
	p = p->mnext; 
	}
}
/*void finallink(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n;
	int result;
	while(p) {
		q = p->nhead;	
		while(q) {
			n = q->dupsign;
			if(q->dup) {
				q = q->nnext;
				continue;
			}
			while(n) {
				if(n->dup) {
					n = n->dupsign;
					continue;
				}
				if(n->fsize < 16)
					result = 0;
				else 
					result = comparefiles(q->mypath, n->mypath);
				if(result == 0)  
					n->dup = q;
				n = n->dupsign;
			}
			q = q->nnext;
		}
		p = p->mnext; 
	}
}*/
void finallink(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n, *r;
	int result;
	while(p) {
		q = p->nhead;	
		while(q) {
			if(q->visited == 1) {
				q = q->nnext;
				continue;
			}
			r = q;
			n = q->dupsign;
			if(!n)
				q->visited = 1;
			while(n) {
				if(n->visited == 1) {
					n = n->dupsign;
					continue;
				}
				if(n->fsize < 16)
					result = 0;
				else
					result = comparefiles(r->mypath, n->mypath);
				if(result == 0) {
					n->visited = 1; 
					r->dup = n;
					r = n;
				}
				n = n->dupsign;
			}
		q = q->nnext;
		}
	p = p->mnext; 
	}
}
char *getsign(char *fname) {
	FILE *fp;
	int i;
	unsigned char c;
	char *sign =(char *)malloc(32 * 3 + 1), temp[4];
	if(sign == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	sign[0] = '\0';
	off_t fsize, x;
	fsize = filesize(fname);
	fp = fopen(fname, "rb");
	if(!fp) {
		perror("Open Failed\n");
		exit(errno);
	}
	if(fsize % 32 == 0)
		x = fsize / 32;
	else 
		x = fsize / 32 + 1;
	if(fsize < 16) {
		while(fread(&c, 1, 1, fp)) {
			snprintf(temp, 4, "%d", c);
			strcat(sign, temp);
		}
		return sign;
	}
	for(i = 0; i < 16; i++) {
		fseek(fp, x * i, SEEK_SET);
		fread(&c, 1, 1, fp);
		snprintf(temp, 4, "%d", c);
		strcat(sign, temp);
		fseek(fp, (fsize - 2) - (x * i), SEEK_SET);
		fread(&c, 1, 1, fp);
		snprintf(temp, 4, "%d", c);
		strcat(sign, temp);	 
	}
	return sign;
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
int comparefiles(char *file1, char *file2) {
	FILE *f1, *f2;
	unsigned char c1, c2;
	int state = 0;
	f1 = fopen(file1, "rb");
	if(f1 == NULL) {
		perror("Open Failed\n");
		exit(errno);
	}
	f2 = fopen(file2, "rb");
	if(f2 == NULL) {
		perror("Open Failed\n");
		exit(errno);
	}
	while(fread(&c1, sizeof(c1), 1, f1) && fread(&c2, sizeof(c1), 1, f2)) {
		if(c1 != c2) {
			state = 1;
			break;
		}
	}
	fclose(f1);
	fclose(f2);
	if(state == 1)
		return 1;
	return 0;
}
