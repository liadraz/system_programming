
/*******************************************************************************
					Ext2 File System Project
********************************************************************************
*
*	DESCRIPTION		API - Program which parcing an ext2 file
*					system directory and reads some files.
*	AUTHOR 			Liad Raz
*	DATE			     03 Apr 2022
*
*******************************************************************************/

#ifndef __EXT2_H__
#define __EXT2_H__

#include "article_files/ext2_fs.h"

/*
     Create exts2 file system device on RAM
     --------------------------------------

     sudo modprobe brd
     sudo mkfs.ext2 -b 1024 /dev/ram0
     sudo mount -o sync /dev/ram0 /project_ext2
*/


/*******************************************************************************
* DESCRIPTION	Super Block
*
*******************************************************************************/
struct ext2_super_block GetSuperBlock(void);


void ReadGroupDescriptors(void);

#endif    /* __EXT2_H__ */