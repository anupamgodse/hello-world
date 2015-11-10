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


typedef struct dirent dirent;

static struct option long_options[] = {
	{ "omitfirst", 0, 0, 'f' },
    	{ "recurse", 0, 0, 'r' },
    	{ "sameline", 0, 0, '1' },
    	{ "size", 0, 0, 'S' },
    	{ "noempty", 0, 0, 'n' },
    	{ "delete", 0, 0, 'd' },
    	{ "nopromt", 0, 0, 'N' },
    	{ "nohidden", 0, 0, 'A' },
    	{ "summarize", 0, 0, 'm' },
    	{ "symlinks", 0, 0, 's' },
    	{ "hardlinks", 0, 0, 'H' },
    	{ "recurse:", 0, 0, 'R' },
    	{ "quite", 0, 0, 'q' },
    	{ 0, 0, 0, 0 }
};

typedef struct node {
	char *mypath;
	off_t fsize;
	struct node *dup, *dupsign;
	struct node *nnext;
	int visited;
}node;

typedef struct mydir {
	node *nhead, *ntail;
	struct mydir *mnext;
	char *myname;
}mydir;

typedef struct myplan {
	mydir *head, *tail;
	int count;
}myplan;

typedef struct flink {
	off_t size;
	node *tail;
}flink;

void takefilesfrom(myplan *mp, char *myname);

void appendmydir(mydir *md, node *mn);

void appendmyplan(myplan *mp, mydir *md);

void printmyplan(myplan *);

off_t filesize(char *filename);

void linkbysize(myplan *mp);

void linkbyhashing(myplan *mp);

void finallink(myplan *mp);

int comparefiles(char *file1, char *file2);

char *getsign(char *fname);

void deletefiles(node *q, int n, int i);

void summarizefiles(myplan *mp);

ino_t getino(char *filename);

int getindex(int argc, char **argv);

void showprogress(float percent);

void displayhelp(void);

void destroy(myplan *mp);
