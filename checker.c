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
	uint size;
	uint nblocks;
	uint ninodes;
};

#define NDIRECT (12)


struct dinode{
	short type;
	short major;	//device #
	short minor;
	short nlink;
	uint size;
	uint addrs[NDIRECT+1];	//Datablock addresses
};

int
main(int argc, char *argv[])
{
	int fd = open("fs.img", O_RDONLY);
	assert(fd > -1);

	int rc;
	struct stat sbuf;
	rc = fstat(fd, &sbuf);
	assert(rc == 0);

	void *img_ptr = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	assert(img_ptr != MAP_FAILED);

	struct superblock *sb;
	sb = (struct superblock *)(img_ptr + BSIZE);
	printf("%d %d %d\n", sb->size, sb->nblocks, sb->ninodes);


	//inodes
	int i;
	struct dinode *dip = (struct dinode*)(img_ptr + (2*BSIZE));
	for(i = 0; i< sb->ninodes; i++){
		printf("%d type: %d\n", i, dip->type);
		dip++;
	}

	//bitmap

	//other


	return 0;
}
