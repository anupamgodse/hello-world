/*****************************************************************************
    	projectfdupes "projectfdupes" is a program for identifying duplicate 
    	files residing within specified directories.  
    	
    	Copyright (C) 2015  ANUPAM GODSE anupamgodse121@gmail.com

    	projectfdupes is free software: you can redistribute it and/or modify
    	it under the terms of the GNU General Public License as published by
    	the Free Software Foundation, either version 3 of the License, or
    	(at your option) any later version.

    	projectfdupes is distributed in the hope that it will be useful,
    	but WITHOUT ANY WARRANTY; without even the implied warranty of
    	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    	GNU General Public License for more details.

    	You should have received a copy of the GNU General Public License
    	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "projectfdupes.h"

int omitfirst = 0, recurse = 0, recurseafter = 0, sameline = 0, showsize = 0, exempty = 0, delete = 0, nopromt = 0, nohidden = 0, 
summarize = 0, symlinks = 0, hardlinks = 0, hideprogress = 0;

int main(int argc, char **argv) {
	int opt, i, rafter;
	char **args;
	
	args = (char **) malloc(sizeof(char*) * argc);
  	if (args == NULL) {
    		printf("out of memory!\n");
    		exit(1);
  	}
	/*creating a copy of given argument list*/
  	for (i = 0; i < argc; i++) {
    		args[i] = (char *) malloc(strlen(argv[i]) + 1);
	    	if (args[i] == NULL) {
			free(args);
			printf("out of memory!\n");
			exit(1);
	    	}
    	
    		strcpy(args[i], argv[i]);
  	}
	
	/*set the given options*/
	while ((opt = getopt_long(argc, argv, "frR1SndNAmsHhq", long_options, NULL)) != EOF) {
   		 switch (opt) {
    			case 'f':
      			omitfirst = 1;
      			break;
		    	case 'r':
				recurse = 1;
				break;
			case 'R' :
				recurseafter = 1;
				break;				
			case '1':
				sameline = 1;
				break;
			case 'S':
				showsize = 1;
				break;
			case 'n':
				exempty = 1;
				break;
			case 'd':
				delete = 1;
				break;
			case 'N':
				nopromt = 1;
				break;
			case 'A' :
				nohidden = 1;
				break;
			case 'm' :
				summarize = 1;
				break;
			case 's' :
				symlinks = 1;
				break;
			case 'H' :
				hardlinks = 1;
				break;
			case 'q' :
				hideprogress = 1;
				break;			
			case 'h':
				displayhelp();
				exit(1);
			default:
				printf("Enter proper options : See help for more information\n");
				exit(1);
		}
  	}
  	
  	if (optind >= argc) {
   	 	printf("no directories specified\n");
    		exit(1);
  	}
  	
  	if(summarize && delete) {
  		printf("options --summarize and --delete are not compatible\n");
  		exit(1);
  	}
	
	if(recurse && recurseafter) {
  		printf("options --recurse and --recurse: are not compatible\n");
  		exit(1);
  	}
	
	myplan *mp = (myplan *)malloc(sizeof(myplan));
	if(mp == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	mp->head = mp->tail = NULL;
	mp->count = 0;
	
	char **myname = (char **)malloc(sizeof(char *) * (argc - optind));
	if(myname == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	
	/*taking all names of directories specified*/
	
	for(i = optind; i < argc; i++) {
		myname[i - optind] = (char *)malloc(strlen(argv[i]) + 1);
		if(myname[i - optind] == NULL) {
			printf("Out of Memory :\n");
			exit(1);
		}
		strcpy(myname[i - optind], argv[i]);
	}	
	
	/*if -R is option given then seperating the directories for which subdirectories are to be searched*/
	if (recurseafter == 1) {
   	 	rafter = getindex(argc, args);
    		
    		if (rafter == argc) {
			printf("Missing directory(s) name(s) after --recurse:\n");
			exit(1);
		}

	    	for (i = 0; i < rafter; i++) 
			takefilesfrom(mp, myname[i]);
		

	    	recurse = 1;

	    	for (i = rafter; i < argc - optind; i++) 
			takefilesfrom(mp, myname[i]);
	
	}
  	else {
		for(i = 0; i < argc - optind; i++)
			takefilesfrom(mp, myname[i]);
	}
	
	if(!hideprogress)
		printf("Progress[%d/%d] %d%%", 0, mp->count, (int)((float)0 / mp->count * 100));
	
	/*the function linkbysize links the files with same sizes*/ 
	linkbysize(mp);
	
	/*the function linkbyhashing further links files with same sizes also having same hashing only*/
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
	
	/*the function finallink finally links all the exact duplicates*/
	finallink(mp);
	
	if(!hideprogress)
		printf("\rProgress[%d/%d] %d%%", mp->count, mp->count, (int)((float)(mp->count) / mp->count * 100));
	
		printf("\r                                                 \r");
	
	if(summarize == 1)
		summarizefiles(mp);
	else
		printmyplan(mp);
	
	/*destroy frees all the memories allocated*/
	destroy(mp);
	
	return 0;
}

void takefilesfrom(myplan *mp, char *myname) {
	dirent *dir;
	DIR *dp;
	char *new;
	struct stat s, ls;
	
	dp = opendir(myname);
	if (!dp) {
		perror("Open Failed\n");
		exit(errno);
	}
	
	mydir *ndir = (mydir *)malloc(sizeof(mydir));
	if(ndir == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	
	ndir->nhead = ndir->ntail = NULL;
	ndir->myname = (char *)malloc(strlen(myname) + 2);
	if(ndir->myname == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	
	strcpy(ndir->myname, myname);
	strcat(ndir->myname, "/");
	free(myname);
	
	appendmyplan(mp, ndir);
	
	/*check for each dirent in the directory if it is file then append it to list under current directory else if it is a directory 
	 *go inside that directory and repeat the process recurssively
	 */ 
	while((dir = readdir(dp))) {
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
			/*check for hidden files*/		
			if((dir->d_name[0] == '.') && (nohidden))
				continue;
			
			new = (char *)malloc(strlen(dir->d_name) + strlen(ndir->myname) + 1);
			if(new == NULL) {
					printf("Out of Memory :\n");
					exit(1);
			}
			
			strcpy(new, ndir->myname);
			strcat(new, dir->d_name);
			
			if (stat(new, &s) == -1) {
				free(new);
				continue;
      		}

      		if (lstat(new, &ls) == -1) {
				free(new);
				continue;
			}
			/*if directory then take files from that directory*/
			if((S_ISDIR(s.st_mode)) && (recurse && (symlinks || !S_ISLNK(ls.st_mode)))) {
					myname = new;
					takefilesfrom(mp, myname);
			}
			/*if regular file then append it to current directory list*/
			else if((S_ISREG(ls.st_mode) || (S_ISLNK(ls.st_mode) && symlinks)) && !(S_ISDIR(s.st_mode))) {
				node *mn = (node *)malloc(sizeof(node));
				if(!mn) {
					printf("Out of Memory :\n");
					exit(1);
				}
				
				mn->dup = mn->dupsign = NULL;
				mn->visited = 0;
				mn->mypath = new;
				(mp->count)++;
				
				appendmydir(ndir, mn);
			}
			else 
				free(new);
		}
	}
	closedir(dp);
}

void appendmydir(mydir *md, node *mn) {
	mn->nnext = NULL;
	if(!(md->nhead)) 
		md->nhead = mn;
	else 
		md->ntail->nnext = mn;
	md->ntail = mn;
}

void appendmyplan(myplan *mp, mydir *md) {
	md->mnext = NULL;
	if(!(mp->head))
		mp->head = md;
	else
		mp->tail->mnext = md;
	mp->tail = md;
}

void linkbysize(myplan *mp) {
	mydir *p = mp->head;
	node *q;
	int i = 0, k;
	flink arr[mp->count];
	
	while(p) {
		q = p->nhead;
		while(q) {
			q->fsize = filesize(q->mypath);
			
			for(k = 0; k < i; k++) {
				if(arr[k].size == q->fsize) 
					break;	
			}
			
			if((!(exempty)) | (q->fsize)) {
				if(k == i) {
					arr[i].tail = q;
					arr[i].size = q->fsize;
					i++;
				}
				else {
					arr[k].tail->dup = q;
					arr[k].tail = q;
				}
			}
			q = q->nnext;
		}
		p = p->mnext;	
	} 
	
}

void linkbyhashing(myplan *mp) {
	char *s[mp->count], *s1, *s2;
	mydir *p = mp->head;
	node *q, *n, *r;
	int i = 0, k;
	
	while(p) {
		q = p->nhead;	
		while(q) {
			n = r = q;
			/*the function getsign gets hash of a file*/
			s1 = getsign(q->mypath);
			
			for(k = 0; k < i; k++) {  
				if(s[k] == s1) {
					q = q->nnext;
					break;
				}
			}
			
			if(k < i)
				continue;
			else if(q->dup)
				s[i++] = s1;
			
			while(n->dup) {
				s2 = getsign(n->dup->mypath);
				
				if(strcmp(s1, s2) == 0) { 
					r->dupsign = n->dup;
					r = n->dup;
				}
				
				n = n->dup;
				free(s2);
			}
			q = q->nnext;
		}
		p = p->mnext; 
	}
	for(k = 0; k < i; k++)
		free(s[k]);
}

void finallink(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n, *r;
	int result, ni = 0, i;
	ino_t hino[mp->count], i1, i2;
	struct stat s;
	int count = 0;
	
	while(p) {
		q = p->nhead;	
		while(q) {
			if(q->visited == 1) {
				q = q->nnext;
				continue;
			}
			count++;
			r = q;
			n = q->dupsign;
			
			if(!n)
				q->visited = 1;
			
			lstat(q->mypath, &s);
			
			if(!hardlinks && !(S_ISLNK(s.st_mode))) {	
				i1 = getino(q->mypath);
				hino[ni++] = i1;
			}
			
			while(n) { 
				if(n->visited == 1) {
					n = n->dupsign;
					continue;
				}
				lstat(n->mypath, &s);
				count++;
				if(!hardlinks && (!(S_ISLNK(s.st_mode)))) {
					i2 = getino(n->mypath);
				
					for(i = 0; i < ni; i++) {
						if(i2 == hino[i]) 
							break;
					}
			
					if(i < ni) {
						n->visited = 1;
						n = n->dupsign;
						continue;
					}
					else
						hino[ni++] = i2;
				}
				
				if(n->fsize < 16) 
					result = 0;
				else {
					if(!hideprogress)
						printf("\rProgress[%d/%d] %d%%", count, mp->count, (int)((float)count / mp->count * 100));
					/*the function comparefiles compares two files given byte by byte and returns 0 on success*/
					result = comparefiles(r->mypath, n->mypath);
				}
				/*if result is 0 then link the files as they are duplictes*/
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

void summarizefiles(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n;
	int ns = 0, i = 0, x;
	off_t size = 0;
	
	while(p) {
		q = p->nhead;
		while(q) {
			x = 0;
			
			if(q->visited == 0 && q->dup) {
				ns++;
				n = q->dup;
				
				while(n) {
					i++;
					x++;
					n = n->dup;
				}
				
				size += (q->fsize) *  x;	
			}
			q = q->nnext;
		}
		p = p->mnext;
	}
	
	if (ns == 0)
    		printf("No duplicates found.\n\n");
  	else
  	{
    		if (size < 1024.0)
     			printf("%d duplicate files (in %d sets), occupying %.0f bytes.\n\n", i, ns, (double)size);
    		else if (size <= (1000.0 * 1000.0))
      		printf("%d duplicate files (in %d sets), occupying %.1f kylobytes\n\n", i, ns, size / 1000.0);
    		else
      		printf("%d duplicate files (in %d sets), occupying %.1f megabytes\n\n", i, ns, size / (1000.0 * 1000.0));
  	}
}

void printmyplan(myplan *mp) {
	mydir *p = mp->head;
	node *q, *n;
	int i, ns = 0;
	
	while(p) {
		q = p->nhead;
		while(q) {
			i = 1;
			
			if(q->visited == 0 && q->dup) {
				ns++;
				
				if((!nopromt)) {
					if(showsize)
						printf("%ld bytes each : \n", q->fsize);
					
					if((!omitfirst) && (!delete)) 
						printf("%s", q->mypath);
					else if(delete){
						printf("[%d]", i++);
						printf("%s", q->mypath);
					}	
					
					if(sameline)
						printf("\t");
					else
						printf("\n");	
					
					n = q->dup;
					
					while(n) {
						if(delete == 1)
							printf("[%d]", i++);
						printf("%s", n->mypath);
						
						if(sameline)
							printf("\t");
						else
							printf("\n");
						
						n = n->dup;
					}
				}
				else {
					n = q;
					
					while(n) {
						i++;
						n = n->dup;
					}
				}	
				
				if(delete == 1) 
					deletefiles(q, ns, i - 1);
				
				printf("\n");
			}
			q = q->nnext; 
		}
		p = p->mnext;
	}
}

void deletefiles(node *q, int n, int i) {
	node *node = q;
	int x, k, state = 0, go = 1;
	
	char *choices = (char *)malloc(256), *token;
	if (!choices) { 
	    printf("Out of memory\n");
	    exit(1);
	}
	
	char *preserve = (char *)malloc(sizeof(int) * i);
	if (!preserve) { 
	    printf("Out of memory\n");
	    exit(1);
	}
	
	for (x = 0; x < i; x++) 
		preserve[x] = 0;
	
	if(!nopromt) {
		printf("\nSet %d , preserve files [1 - %d, all] : ", n, i);
		fgets(choices, 256, stdin);
		int l = strlen(choices) - 1;
		while(choices[l] != '\n') {
			choices = (char *)realloc(choices, strlen(choices) + 256 + 1);
			if (!choices) { 
			    printf("Out of memory\n");
			    exit(1);
		  	}
		  	
		  	fgets(choices + l + 1, 256, stdin);
		  	l = strlen(choices) - 1;
		}
	
		token = strtok(choices, " ,\t\n");
		if(token == NULL) {
			go = 0;
			printf("Enter proper choice:");
			deletefiles(q, n, i);
		}
	
		while(token) {
			if(strcmp(token, "all") == 0) {
				state = 1;
				for (x = 0; x < i; x++) 
					preserve[x] = 1;
			}
			
			k = atoi(token);
			
			if (k > 0 && k <= i) { 
				state = 1;
				preserve[k - 1] = 1;
			}
			else if(strcmp("all", token) != 0){
				go = 0;
				printf("Enter proper choice:");
				deletefiles(q, n, i);
			}
		  	token = strtok(NULL, " ,\t\n");
		}
		
		if(state == 0)
			preserve[0] = 1;
	}
	else {
		preserve[0] = 1;
		for(x = 1; x < i; x++)
			preserve[i] = 0;
	}	
	
	k = 0;
	
	while(node && (go != 0)) {
		if(preserve[k++])
			printf("\t[+] %s\n", node->mypath);
		else {
			if(remove(node->mypath) == -1) {
				printf("\t[-] %s -----", node->mypath);
				printf(" Unable to delete %s\n", node->mypath);
			}
			else
				printf("\t[-] %s\n", node->mypath);
		}
		node = node->dup;
	}
	free(preserve);
	free(choices);
}

char *getsign(char *fname) {
	FILE *fp;
	int i;
	unsigned char c;
	off_t fsize, x;
	
	char *sign =(char *)malloc(32 * 3 + 1), temp[4];
	if(sign == NULL) {
		printf("Out of Memory :\n");
		exit(1);
	}
	sign[0] = '\0';
	
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
		fclose(fp);
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
	
	fclose(fp);
	return sign;
}

int comparefiles(char *file1, char *file2) {
	FILE *f1, *f2;
	unsigned char c1, c2;
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
			fclose(f1);
			fclose(f2);
			return 1;
		}	
	}
	fclose(f1);
	fclose(f2);
	return 0;
}

int getindex(int argc, char **argv) {
	int i, rafter = 0;
	
	for(i = 1; i < argc; i++) {
		if((strcmp(argv[i], "-R") == 0) || (strcmp(argv[i], "-recurse:") == 0)) 
			break;
		else if(((argv[i])[0]) != '-') 
			rafter++;
	}	
 	return rafter;
}

off_t filesize(char *filename) {
  	struct stat s;
 	
 	if (stat(filename, &s) != 0) 
 	 	return -1;
 	
 	return s.st_size;
}

ino_t getino(char *filename) {
  struct stat s;
   
  if (stat(filename, &s) != 0) 
  	return 0;

  return s.st_ino;   
}

void destroy(myplan *mp) {
	mydir *md = mp->head, *nmd;
	node *q, *nq;
	while(md) {
		q = md->nhead;
		while(q) {
			free(q->mypath);
			nq = q->nnext;
			free(q);
			q = nq;
		}
		free(md->myname);
		nmd = md->mnext;
		free(md);
		md = nmd;
	}	
}

void displayhelp() {
	printf(" -f --omitfirst	\tomit the first file in each set of matches(while using\n"); 
	printf("	            \tdelete it will show all files)\n\n");
  	printf(" -r --recurse  	\tfor every directory given follow subdirectories\n");
  	printf("                \tencountered within\n\n");
  	printf(" -R --recurse:  \tfor each directory given before this option does not\n"); 
  	printf("	            \tfollow subdirectories encountered within but those\n");
  	printf("               	\tafter this option follow subdirectories encountered within\n\n");
  	printf(" -1 --sameline  \tlist each set of matches on a single line\n\n");
  	printf(" -S --size    	\tshow size for each set of matches \n\n");
  	printf(" -n --noempty   \texclude zero-length files from consideration\n\n");
  	printf(" -d --delete    \tprompt user for files to preserve and delete\n\n"); 
  	printf(" -N --noprompt  \ttogether with --delete, preserve the first file in\n");
  	printf("                \teach set of duplicates and delete the rest without\n");
  	printf("                \tprompting the user\n\n");
  	printf(" -A --nohidden  \texclude hidden files from consideration\n\n");
  	printf(" -m --summarize \tsummarize duplicate files information\n\n");
  	printf(" -s --symlinks  \tfollow symlinks\n\n");
  	printf(" -H --hardlinks \tnormally, when two or more files point to the same\n");
	printf("                \tdisk area they are treated as non-duplicates; this\n"); 
  	printf("                \toption will change this behavior\n\n");
  	printf(" -q --quiet     \thide progress indicator\n\n");
  	printf(" -h --help      \tdisplay this help message\n\n\n\n");
  	printf(" When same directory is specified more than once each file will\n");
	printf(" considered only once unless user tells explicitly to consider\n"); 
	printf(" hardlinks(they are files with same path(same files)!!) because if\n");
	printf(" directories specified are same, of course each set of duplicate\n");
	printf(" will list same files as many times as the same directory is spcified.\n");
	printf(" All files within that directory will be listed as their own\n");
	printf(" duplicates, this can lead to data loss, user must preserve a file\n"); 
	printf(" without its ""duplicate"" (the file itself!).\n\n");
	printf(" When using -d or --delete, care should be taken to insure against\n");
	printf(" accidental data loss. While no information will be immediately\n"); 
	printf(" lost, using this option together with -s or --symlink can lead \n");
	printf(" to confusing information being presented to the user when prompted\n");
	printf(" for files to preserve. Specifically, a user could accidentally\n");
	printf(" preserve a symlink while deleting the file it points to.\n");
}
