#include <stdio.h>
#include <stdlib.h>
//stat fstat;
int main(int argc, char *argv[]) {
	DIR *dp;
	dirent *dir;
	myplan mp;
	initmyplan(&mp);
	d = opendir(argv[1]);
	if (d == NULL) {
		printf("Open Failed\n");
		return 0;
	}
	//iff exists open the directory;
	//create mydir;
	mydir md;
	//at this state we dont know whether we have any files or directories present in this directory. Hence nhead = nprev = NULL)
	initmydir(&md);
	//This is root director.
	md.mprev = md.mnext = NULL;
	//make head(and tail) point to this directory;(myplan)
	mp.head = mp.tail = &md;
	stat fstat;
	while(dir = readdir(dp)) {
		stat(dir->d_name, &fstat);
		//identify dirent as file or directory;
		if(!(S_ISDIR(fstat.st_mode))) {
			//iff a dirent is a file(implies the file is present in given directory) 
			//create node and link file dirent to this node.
			node mn;
			initnode(mn);
			mn.cdir = dir;
			appendmydir(&md, &mn);
		}
		else if(S_ISDIR(fstat.st_mode){
			//at this state we dont know whether we have any files or directories present in this directory. Hence nhead = nprev = mnext = NULL(mprev = NULL)
			//create new dir.
			mydir ndir;
			initmydir(&ndir);
			//link this directory to mypaln.
			appendmyplan(&mp, &ndir);
			//go inside the directory, and repeat the above procedure.
			//call sortdir, this function will be recursive and take all the files in data structure under specific directory.
			sortdir(&mp, dir->d_name);
		}
	}
	now we have all directorires under myplan and all the files under specific mydir;
}	
void sortdir(mydir *a, name) {
	go to specific path;
	open the req directory;
	create mydir;
	append this directory to my plan;
	while(no file or directory(dirent) is present) {
		identify dirent as file or directory;
		iff a dirent is a file(implies the file is present in given directory) {
			create a node append it to current mydir; 
		}
		elseif(dirent = directory) {
				call sortdir(*a, directory name(withpath));  //this function will be recursive and take all the files in data structure under specific directory.
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
		md->nhead = mn;
		mn->nprev = NULL;
	}
	else {
		md->ntail->nnext = mn;
		mn->nprev = md->tail;
	}
	md->ntail = mn;
}
void appendmyplan(myplan *mp, mydir *md) {
	md->mnext = NULL;
	md->mprev = mp->tail;
	mp->tail->mnext = md;
}
//Now i will be having all the files. Now i will search the specified file by its name in myplan. once i got the first file i will compare it using filename, filesize, md5 signature and byte by byte comparison. When i will get a duplicate file I will take its pointer in a new file pointer array. Like this I will have all the duplicate files and then i can do all desired oprations in them. 
	
