#define MAX 1024
typedef struct dirent dirent;
typedef struct node {
	char *nname;
	char signature[33];
	off_t fsize;
	struct node *nnext, *nprev;
}node;
typedef struct mydir {
	node *nhead, *ntail;
	struct mydir *mnext, *mprev;
	char *myname;
}mydir;
typedef struct myplan {
	mydir *head, *tail;
}myplan;
void sortdir(myplan *, char *);
void initmyplan(myplan *);
void initmydir(mydir *);
void initnode(node *);
void appendmydir(mydir *, node *);
void appendmyplan(myplan *, mydir *);
void printmyplan(myplan *);
void printmydir(mydir *);
char *getsignature(char *filename);
void hashfiles(mydir *md);
off_t filesize(char *filename);
int getlevel(char *name);
