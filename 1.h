typedef struct dirent dirent;
typedef struct node {
	dirent *cdir;
	struct node *nnext, *nprev;
}node;
typedef struct mydir {
	node *nhead, *ntail;
	struct mydir *mnext, *mprev;
}mydir;
typedef myplan {
	mydir *head, *tail;
}myplan;
typedef struct stat stat;
void sortdir(mydir *, name);
void initmyplan(myplan *);
void initmydir(mydir *);
void appendmydir(mydir *, dirent *);
