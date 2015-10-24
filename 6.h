typedef struct dirent dirent;
typedef struct node {
	char *mypath;
	off_t fsize;
	struct node *dup, *dupsign;
	struct node *nnext, *nprev;
	int visited;
}node;
typedef struct mydir {
	node *nhead, *ntail;
	struct mydir *mnext, *mprev;
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
void sortdir(myplan *, char *);
void initmyplan(myplan *);
void initmydir(mydir *);
void initnode(node *);
void appendmydir(mydir *, node *);
void appendmyplan(myplan *, mydir *);
void printmyplan(myplan *);
off_t filesize(char *filename);
int getlevel(char *name);
void linkbysize(myplan *mp);
void linkbyhashing(myplan *mp);
void finallink(myplan *mp);
int comparefiles(char *file1, char *file2);
char *getsign(char *fname);
