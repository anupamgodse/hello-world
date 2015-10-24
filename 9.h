typedef struct dirent dirent;
static struct option long_options[] = {
	{ "omitfirst", 0, 0, 'f' },
    	{ "recurse", 0, 0, 'r' },
    	{ "sameline", 0, 0, '1' },
    	{ "size", 0, 0, 'S' },
    	{ "noempty", 0, 0, 'n' },
    	{ "delete", 0, 0, 'd' },
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
void sortdir(myplan *mp, char *myname);
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
