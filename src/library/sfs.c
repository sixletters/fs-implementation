// simple file system
#include "../include/sfs.h"
#include "../include/log.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


const int INODE_SIZE = sizeof(Inode);

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
    if(disk == NULL || disk->mounted) {
        error("disk has already been mounted or disk is a null pointer");
        return false;
    }
    // create the block and set the appropriate values
    Block super_block;
    if(!get_superblock_from_disk(disk, &super_block)){
        return false;
    }
    if(!verify_superblock(&super_block, disk)){
        return false;
    }
    char data[BLOCK_SIZE];
    // char empty_data[BLOCK_SIZE];
    if(!superblock_to_bytes(&super_block, data)){
        error("unable to convert super block to bytes");
        return false;
    }
    if(disk_write(disk, 0, data) == DISK_FAILURE) {
        error("error writing super block to disk");
        return false;
    }
    // for(size_t i = 1; i < disk->blocks; i++){
    //     if(disk_write(disk, i, empty_data) == DISK_FAILURE) {
    //         error("error writing empty data to disk at blocl numer %zu", i);
    //         return false;
    //     }
    // }
    return true;
};

/** Mount a specified FS to given disk by doing the following:
 * Read and check superblock vertify attributes, 
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
    if(fs == NULL || disk == NULL || disk->mounted){
        error("fs or disk is a null pointer");
        return false;
    }
    // format the disk with appropriate information
    if(!fs_format(disk)) {
        return false;
    }
    Block super_block;
    if(!get_superblock_from_disk(disk, &super_block)){
        return false;
    }
    if(!fs_initialize_meta(fs, &super_block, disk)){
        return false;
    }
    // intialize free _blocks and also set all to true except inode and super block
    if(!fs_initialize_free_block_bitmap(fs)){
        return false;
    }
    return true;
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

// this utilisy function transforms a superblock struct into a char array/ stream of bytes
// todo: add a byte stream api to easily add and remove values from bytestream
bool superblock_to_bytes(Block* super_block, char* data) {
    if(super_block == NULL || data == NULL) return false;
    // all values in super block are uint32_t integers
    size_t curr_index = 0;
    memcpy(data+curr_index, &(super_block->super_block.magic_number), 4);
    curr_index += 4;
    
    memcpy(data+curr_index, &(super_block->super_block.total_blocks), 4);
    curr_index += 4;

    memcpy(data+curr_index, &(super_block->super_block.total_inodes), 4);
    curr_index += 4;

    memcpy(data+curr_index, &(super_block->super_block.blocks), 4);
    curr_index += 4;

    memcpy(data+curr_index, &(super_block->super_block.inode_blocks), 4);
    curr_index += 4;

    memcpy(data+curr_index, &(super_block->super_block.inodes), 4);
    curr_index += 4;
    return true;
} 

// this utilisy function transforms a superblock struct from a char array/ stream of bytes
bool superblock_from_bytes(Block* super_block, char* data) {
    if(super_block == NULL || data == NULL) return false;

    size_t curr_index = 0;
    memcpy(&(super_block->super_block.magic_number), data+curr_index, 4);
    curr_index += 4;
    
    memcpy(&(super_block->super_block.total_blocks), data+curr_index, 4);
    curr_index += 4;

    memcpy(&(super_block->super_block.total_inodes), data+curr_index, 4);
    curr_index += 4;

    memcpy(&(super_block->super_block.blocks), data+curr_index, 4);
    curr_index += 4;

    memcpy(&(super_block->super_block.inode_blocks), data+curr_index, 4);
    curr_index += 4;

    memcpy(&(super_block->super_block.inodes), data+curr_index, 4);
    curr_index += 4;
    return true;
} 

// this utilisy function transforms an inode struct from a char array/ stream of bytes
bool inode_table_from_bytes(Block* inode_table, char* data) {
    if(inode_table == NULL || data == NULL) return false;
    for(size_t i = 0; i < INODES_PER_BLOCK; i++){
        memcpy(inode_table->inodes + i, data + (INODE_SIZE * i), INODE_SIZE);
    }
    return true;
}

bool block_pointers_from_bytes(Block* block_pointers, char *data) {
    if(block_pointers == NULL || data == NULL) return false;
    for(size_t i = 0; i < POINTERS_PER_BLOCK; i++){
        memcpy(block_pointers->block_pointers + i, data + ((i * sizeof(uint32_t))),  sizeof(uint32_t));
    }
    return true;
}

// this utility function transforms an inode struct into a char array/ stream of bytes
bool inode_table_to_bytes(Block* inode_table, char* data) {
    if(inode_table == NULL || data == NULL) return false;
    for(size_t i = 0; i < INODES_PER_BLOCK; i ++) {
        memcpy(data+= INODE_SIZE * i, inode_table->inodes + i, INODE_SIZE);
    }
    return true;
}

// this functiona intializes the in memory block bitmap
bool fs_initialize_free_block_bitmap(FileSystem *fs){
    // intialize free _blocks and also set all to true except inode and super block
    fs->free_blocks = malloc(sizeof(bool) * fs->meta.blocks);
    fs->free_blocks[0] = false;
    for(int i = fs->meta.inode_blocks + 1; i < fs->meta.blocks; i++){
        fs->free_blocks[i] = true;
    }

    char buffer[BLOCK_SIZE];
    Block inode_block;
    // iterate through the inode blocks
    for(size_t i = 1; i <= fs->meta.inode_blocks; i++){
        // read the inode table from disk
        if(disk_read(fs->disk, i, buffer) < 0){
            error("error in reading from buffer");
            return false;
        }
        // convert from buffer to inode block in memory struct
        inode_table_from_bytes(&inode_block,  buffer);
        // iterate through the inodes, if valid then find the blocks its points to and mark them as used
        for(int idx = 0; idx < INODES_PER_BLOCK; idx++){
            if(inode_block.inodes[idx].valid == 1){
                for(int j = 0; j < POINTERS_PER_INODE; j++){
                    if(inode_block.inodes[idx].direct[j] > fs->meta.inode_blocks && inode_block.inodes[idx].direct[j] < fs->meta.blocks) {
                        fs->free_blocks[inode_block.inodes[idx].direct[j]] = false;
                    }
                }
                if(inode_block.inodes[idx].size > POINTERS_PER_INODE * BLOCK_SIZE) {
                    char data[BLOCK_SIZE];
                    Block block_pointers;
                    if(!disk_read(fs->disk, inode_block.inodes[idx].indirect, data)){
                        return false;
                    }
                    if(!block_pointers_from_bytes(&block_pointers, data)){
                        return false;
                    }
                    ssize_t leftoverblocks = (inode_block.inodes[idx].size - (POINTERS_PER_INODE * BLOCK_SIZE));
                    size_t curr = 0;
                    while(leftoverblocks > 0){
                        fs->free_blocks[block_pointers.block_pointers[curr]] = false;
                        leftoverblocks -= BLOCK_SIZE;
                        curr += 1;
                    }
                }
            }
        }
    }
    return true;
}

bool get_superblock_from_disk(Disk* disk, Block* super_block){
    // This should be set to block_size and not sizeof(super_block)
    char superblock_data[BLOCK_SIZE];
    if(disk_read(disk, 0, superblock_data) != BLOCK_SIZE){
        return false;
    }
    if(!superblock_from_bytes(super_block, superblock_data)){
        return false;
    }
    return true;
}

// intialize the fs meta from the super block on disk
bool fs_initialize_meta(FileSystem *fs, Block* super_block, Disk* disk){
    // This should be set to block_size and not sizeof(super_block)
    fs->disk = disk;
    disk->mounted = true;
    fs->meta = (super_block->super_block);
    return true;
}

bool verify_superblock(Block* super_block, Disk* disk) {
    if(super_block == NULL || disk == NULL){
        return false;
    }
    uint32_t num_inode_blocks = round(ceil(0.1 * disk->blocks));
    super_block->super_block.magic_number = MAGIC_NUMBER;
    super_block->super_block.blocks = disk->blocks;
    super_block->super_block.inode_blocks = round(ceil(0.1 * disk->blocks));
    super_block->super_block.inodes = num_inode_blocks * INODES_PER_BLOCK;
    super_block->super_block.total_inodes = num_inode_blocks * INODES_PER_BLOCK;
    super_block->super_block.total_blocks = disk->blocks;
    return true;
}

