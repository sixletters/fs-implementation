// simple file system
#include "../include/sfs.h"
#include "../include/log.h"

#include <stdio.h>
#include <string.h>

/** Debug FS, read superblock and its information, read inode table and report infromation about node
 * 
 * @param disk pointer to disk
 * 
**/
void fs_debug(Disk *disk){
    Block block;

    // Read super block
    if (disk_read(disk, 0, block.data) == DISK_FAILURE) {
        return;
    }

    printf("SuperBlock:\n");
    printf("    %u blocks\n"         , block.super_block.blocks);
    printf("    %u inode blocks\n"   , block.super_block.inode_blocks);
    printf("    %u inodes\n"         , block.super_block.inodes);

    /* Read Inodes */
    // printf("Inodes:\n");
    // printf("    %u blocks\n"         , block.super_block.inodes);
    // printf("    %u inode blocks\n"   , block.super_block.inode_blocks);
    // printf("    %u inodes\n"         , block.super_block.inodes);
}

/** Format Disk by, writing to superblock (with appropriate magic number, number of blocks,
 *  number of inode blocks, and number of inodes) and clear all remaining blocks
 * 
 * SHOULD NOT CLEAR A mounted Disk!
 * 
 * @param disk pointer to disk
 * @return whether or not all disk operations were succesful
 * 
**/
bool fs_format(Disk *disk){
    return false;
};

/** Mount a specified FS to given disk by doing the following:
 * Read and check superblock vertify attributes), 
 * Record FS disk attribute and set Disk mount status,
 * copy superblock to FS meta attribute, 
 * initilize FS free blocks bitmap
 * 
 * SHOULD NOT MOUNT ON A DISK THAT HAS ALREADY BEEN MOUNTED
 * 
 * @param fs pointer to FileSystem structure
 * @param disk pointer to disk
 * @return whether or not mount operation was successful
 * 
**/
bool    fs_mount(FileSystem *fs, Disk *disk){
    return false;
};

/**
 * Unmount FileSystem from internal Disk by doing the following: 
 * 
 * Set Disk mounted status and FileSystem disk attribute,
 * Release free blocks bitmap.
 *
 * @param       fs      Pointer to FileSystem structure.
 **/
void    fs_unmount(FileSystem *fs){
};

/**
 * Allocate an Inode in the FileSystem Inode table by doing the following:
 *
 * Search Inode table for free inode.
 * Reserve free inode in Inode table.
 *
 * BE SURE TO UPDATE TO DISK
 *
 * @param       fs      Pointer to FileSystem structure.
 * @return      Inode number of allocated Inode.
 **/
ssize_t fs_create(FileSystem *fs){
    return -1;
}

/**
 * Remove Inode and associated data from FileSystem by doing the following:
 *
 * Load and check status of Inode.
 * Release any direct blocks.
 * Release any indirect blocks.
 * Mark Inode as free in Inode table.
 *
 * @param       fs              Pointer to FileSystem structure.
 * @param       inode_number    Inode to remove.
 * @return      Whether or not removing the specified Inode was successful.
 **/
bool    fs_remove(FileSystem *fs, size_t inode_number){
    return false;
};

/**
 * Return size of specified Inode.
 *
 * @param       fs              Pointer to FileSystem structure.
 * @param       inode_number    Inode to remove.
 * @return      Size of specified Inode (-1 if does not exist).
 **/
ssize_t fs_stat(FileSystem *fs, size_t inode_number){
    return -1;
};

/**
 * Read from the specified Inode into the data buffer exactly length bytes
 * beginning from the specified offset by doing the following:
 *
 * Load Inode information.
 * Continuously read blocks and copy data to buffer.
 *
 * Data is read from direct blocks first, and then from indirect blocks.
 *
 * @param       fs              Pointer to FileSystem structure.
 * @param       inode_number    Inode to read data from.
 * @param       data            Buffer to copy data to.
 * @param       length          Number of bytes to read.
 * @param       offset          Byte offset from which to begin reading.
 * @return      Number of bytes read (-1 on error).
 **/
ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset){
    return -1;
}

/**
 * Write to the specified Inode from the data buffer exactly length bytes
 * beginning from the specified offset by doing the following:
 *
 * Load Inode information.
 * Continuously copy data from buffer to blocks.
 *
 * Data is read from direct blocks first, and then from indirect blocks.
 *
 * @param       fs              Pointer to FileSystem structure.
 * @param       inode_number    Inode to write data to.
 * @param       data            Buffer with data to copy
 * @param       length          Number of bytes to write.
 * @param       offset          Byte offset from which to begin writing.
 * @return      Number of bytes read (-1 on error).
 **/
ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset){
    return -1;
}

