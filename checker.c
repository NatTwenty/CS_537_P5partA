#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>

//Block 0 = unused
//Block 1 = super
//Block 2 = start of Inodes

#define ROOTINO 1 //root of Inode
#define BSIZE 512 //block size

struct superblock{
	uint size;	//number of blocks in fs
	uint nblocks;
	uint ninodes;
};

#define NDIRECT (12)
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

struct dinode{
	short type;
	short major;	//device #
	short minor;
	short nlink;	//number of links TO THIS inode
	uint size;
	uint addrs[NDIRECT+1];	//Datablock addresses
};

int
main(int argc, char *argv[])
{ 	
	int fd = open(argv[1], O_RDONLY);
	assert(fd > -1);

	int rc;
	struct stat sbuf;
	rc = fstat(fd, &sbuf);
	assert(rc == 0);

	void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(img_ptr != MAP_FAILED);

	struct superblock *sb;
	sb = (struct superblock *)(img_ptr + BSIZE);
	printf("%d %d %d\n", sb->size, sb->nblocks, sb->ninodes);//TODO debug purposes only

	int range = (int)((sb->size) * BSIZE);//should be the max value of valid addresses
	printf("%d\n", range);

	//inodes
	int i;
	struct dinode *dip = (struct dinode*)(img_ptr + (2*BSIZE));

	for(i = 0; i< sb->ninodes; i++){
		short type = dip->type;
		//CHECK:valid inodes
		//stat.h => type defs > T_DIR (directory) = 1
		//			T_FILE (file) = 2
		//			T_DEV (special device) = 3
		//note: 0 => unallocated
		if(type == 0 || type == 1 || type == 2 || type == 3){
			printf("%d type: %d\n", i, type);//TODO debug purposes only
			if(type != 0){
				int dev = (dip->size)/BSIZE;
				int mod = (dip->size)%BSIZE;
				printf("%d.%d\n", dev, mod);//can use this to get the total number of blocks in use

				printf("number of links = %d\nsize = %d\naddresses = ", dip->nlink, dip->size);
				
				int j;
				for(j = 0; j < (NDIRECT+1); j++){
					printf("%d  ", dip->addrs[j]);
				}
				printf("\n\n");
			}
			dip++;
		}else{
			fprintf(stderr,"bad inode\n");
			return 1;
		}

		//CHECK:root directory
		if(i == ROOTINO && type != 1){
			fprintf(stderr,"root directory does not exist\n");
			return 1;
		}
		
	}




	//bitmap

	//other


	return 0;
}
