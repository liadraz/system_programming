
/*******************************************************************************
					Ext2 File System Project
********************************************************************************
*
*	DESCRIPTION		Implementation of a program which parcing an ext2 file
*					system directory and reads some files.
*	AUTHOR 			Liad Raz
*	DATE			03 Apr 2022
*
*******************************************************************************/

#include <stdio.h>		/* printf */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "article_files/ext2_fs.h"

#define BASE_OFFSET 1024		/* locates beginning of the super block */
#define FD_DEVICE "/dev/ram0"	/* the ram device */

static unsigned int block_size = 0;			/* block size (to be calculated) */


/*

Structure of an Ext2 Filesystem

	Boot-Block ||| Block Group 0 ||| ... ||| Block Group N
					|
					|
					V
Super Block || Group Descriptors || Data Block Bitmap || Inode Bitmap || Inode Table || Data Blocks

*/

/*******************************************************************************
	Super-Block
*******************************************************************************/
struct ext2_super_block GetSuperBlock(void)
{

	struct ext2_super_block super;
	int fd;

	/* open floppy device */

	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) 
	{
		perror(FD_DEVICE);
		exit(1);  /* error while opening the floppy device */
	}

	/* read super-block */

	lseek(fd, BASE_OFFSET, SEEK_SET);
	read(fd, &super, sizeof(super));
	close(fd);

	if (super.s_magic != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(1);
	}
		
	block_size = 1024 << super.s_log_block_size;

	printf("Reading super-block from device " FD_DEVICE ":\n"
	       "Inodes count            : %u\n"
	       "Blocks count            : %u\n"
	       "Reserved blocks count   : %u\n"
	       "Free blocks count       : %u\n"
	       "Free inodes count       : %u\n"
	       "First data block        : %u\n"
	       "Block size              : %u\n"
	       "Blocks per group        : %u\n"
	       "Inodes per group        : %u\n"
	       "Creator OS              : %u\n"
	       "First non-reserved inode: %u\n"
	       "Size of inode structure : %hu\n"
		   "Size of log_block_size  : %hu\n"
		   "\n"
	       ,
	       super.s_inodes_count,  
	       super.s_blocks_count,
	       super.s_r_blocks_count,     /* reserved blocks count */
	       super.s_free_blocks_count,
	       super.s_free_inodes_count,
	       super.s_first_data_block,
	       block_size,
	       super.s_blocks_per_group,
	       super.s_inodes_per_group,
	       super.s_creator_os,
	       super.s_first_ino,          /* first non-reserved inode */
	       super.s_inode_size,
	       super.s_log_block_size);
}



/*******************************************************************************
	Group-block descriptors from a Ext2 ram device.
*******************************************************************************/
void ReadGroupDescriptors(void)
{
	struct ext2_super_block super = GetSuperBlock();
	struct ext2_group_desc group;
	int fd;

	/* open floppy device */
	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) 
	{
		perror(FD_DEVICE);
		exit(1);  /* error while opening the floppy device */
	}

	block_size = 1024 << super.s_log_block_size;

	/* read group descriptor */
	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
	read(fd, &group, sizeof(group));
	close(fd);

	printf("Reading first group-descriptor from device " FD_DEVICE ":\n"
	       "Blocks bitmap block: %u\n"
	       "Inodes bitmap block: %u\n"
	       "Inodes table block : %u\n"
	       "Free blocks count  : %u\n"
	       "Free inodes count  : %u\n"
	       "Directories count  : %u\n"
		   "\n"
	       ,
	       group.bg_block_bitmap,
	       group.bg_inode_bitmap,
	       group.bg_inode_table,
	       group.bg_free_blocks_count,
	       group.bg_free_inodes_count,
	       group.bg_used_dirs_count);    /* directories count */
}

/*
	The group descriptor tells us the location of the block/inode bitmaps and of the inode table (described later) through the bg_block_bitmap, bg_inode_bitmap and bg_inode_table fields. 
*/