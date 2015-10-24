typedef struct dirent dirent;
typedef struct node {
	char *nname;
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
