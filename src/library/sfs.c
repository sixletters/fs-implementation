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
    printf("Inodes:\n");
    printf("    %u inode blocks\n"   , block.super_block.inode_blocks);
    printf("    %u inodes\n"         , block.super_block.inodes);
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
    // retrieve superblock from disk
    if(disk_read(disk, 0, (char*)(&super_block)) != BLOCK_SIZE) return false;
    // verify and set appropriate values for superblock
    if(!verify_superblock(&super_block, disk)) return false;
    // treat super block like a stream of bytes and typecast to char array
    if(disk_write(disk, 0, (char*)(&super_block)) == DISK_FAILURE) return false;
    // todo
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
    if(fs == NULL || disk == NULL){
        error("fs or disk is a null pointer");
        return false;
    }
    if(disk->mounted){
        error("Disk has already been mounted");
        return false;
    }
    // format the disk with appropriate information
    if(!fs_format(disk)) return false;

    Block super_block;
    // retrieve the super block from disk
    if(disk_read(disk, 0, (char*)(&super_block)) != BLOCK_SIZE) return false;
    // intialize meta with the fetched info from on disk superblock
    if(!fs_initialize_meta(fs, &super_block, disk)) return false;
    // intialize free blocks and also set all to true except inode and super block
    if(!fs_initialize_free_block_bitmap(fs)) return false;
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
 * todo: use an inode_bitmap instead of iterating through entire inode tables
 *
 * @param       fs      Pointer to FileSystem structure.
 * @return      Inode number of allocated Inode.
 **/
ssize_t fs_create(FileSystem *fs){
    // iterate through all the inode blocks
    for(ssize_t i = 1; i < fs->meta.inode_blocks + 1; i++){
        Block inode_super_block;
        // retrieve inode from disk
        if(disk_read(fs->disk, i, (char*)(&inode_super_block)) != BLOCK_SIZE) return -1;
        // iterate through all inodes inode table
        for(ssize_t j = 0; j < INODES_PER_BLOCK; j++){
            // if free then handle
            if(inode_super_block.inodes[j].valid == false){
                inode_super_block.inodes[j].valid = true;
                if(disk_write(fs->disk, i, (char*)&inode_super_block) == DISK_FAILURE) return -1;
                return (i-1) * INODES_PER_BLOCK + j;
            }
        }
    }
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
    Block block;
    int inode_block_number = (inode_number/INODES_PER_BLOCK) + 1;
    int inode_offset = (inode_number % INODES_PER_BLOCK);
    if(inode_block_number > fs->meta.inode_blocks){
        error("inode block number exceeds number of blocks provided");
        return false;
    }
    // read inode table from disk
    if(disk_read(fs->disk, inode_block_number, (char*)(&block)) != BLOCK_SIZE) return false;
    if(!block.inodes[inode_offset].valid){
        error("not valid inode to remove");
        return false;
    }
    for(int i = 0; i < POINTERS_PER_INODE ; i++){
        fs->free_blocks[block.inodes[inode_offset].direct[i]] = true;
    }
    if(block.inodes[inode_offset].size > POINTERS_PER_INODE * BLOCK_SIZE) {
        // read and free the indirect blocks
        Block indirect_block;
        fs->free_blocks[block.inodes[inode_offset].indirect] = true;
        if(disk_read(fs->disk, block.inodes[inode_offset].indirect, (char*)&indirect_block) != BLOCK_SIZE) {
            error("error in reading from block");
            return false;
        }
        ssize_t leftoverblocks = (block.inodes[inode_offset].size - (POINTERS_PER_INODE * BLOCK_SIZE));
        size_t curr = 0;
        while(leftoverblocks > 0){
            fs->free_blocks[indirect_block.block_pointers[curr]] = true;
            leftoverblocks -= BLOCK_SIZE;
            curr += 1;
        }
    }

    block.inodes[inode_offset].size = 0;
    block.inodes[inode_offset].valid = false;
    // write inode table back to disk
    // I realise I dont have to do all the conversion to stream of bytes, we can simply cast it as an array of bytes and move on.
    disk_write(fs->disk, inode_block_number,(char*)&block);
    return true;
};

/**
 * Return size of specified Inode.
 *
 * @param       fs              Pointer to FileSystem structure.
 * @param       inode_number    Inode to remove.
 * @return      Size of specified Inode (-1 if does not exist).
 **/
ssize_t fs_stat(FileSystem *fs, size_t inode_number){
    if(fs == NULL) {
        return -1;
    }
    int inode_block_number = (inode_number / INODES_PER_BLOCK )+ 1;
    if(inode_block_number > fs->meta.inode_blocks){
        return -1;
    }
    int inode_offset = inode_number % INODES_PER_BLOCK;
    Block block;
    disk_read(fs->disk, inode_block_number, (char*)(&block));
    if(block.inodes[inode_offset].valid){
        return block.inodes[inode_offset].size;
    } 
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

/**
 * function that intializes and sets the free blocks bit map in memory
**/
bool fs_initialize_free_block_bitmap(FileSystem *fs){
    // intialize free _blocks and also set all to true except inode and super block
    fs->free_blocks = malloc(sizeof(bool) * fs->meta.blocks);
    fs->free_blocks[0] = false;
    for(int i = fs->meta.inode_blocks + 1; i < fs->meta.blocks; i++){
        fs->free_blocks[i] = true;
    }

    Block inode_block;
    // iterate through the inode blocks
    for(size_t i = 1; i <= fs->meta.inode_blocks; i++){
        // read the inode table from disk
        if(disk_read(fs->disk, i, (char*)(&inode_block)) < 0){
            error("error in reading from buffer");
            return false;
        }
        // iterate through the ivinodes, if valid then find the blocks its points to and mark them as used
        for(int idx = 0; idx < INODES_PER_BLOCK; idx++){
            if(inode_block.inodes[idx].valid == 1){
                // for each direct pointer to block, we set the free block entry of that block to false
                for(int j = 0; j < POINTERS_PER_INODE; j++){
                    if(inode_block.inodes[idx].direct[j] > fs->meta.inode_blocks && inode_block.inodes[idx].direct[j] < fs->meta.blocks) {
                        fs->free_blocks[inode_block.inodes[idx].direct[j]] = false;
                    }
                }
                // Check if the size is bigger than total number of direct pointers to block
                // in which case we can set the indirect block to false, it is being used as a block that holds pointers to other blocks
                if(inode_block.inodes[idx].size > POINTERS_PER_INODE * BLOCK_SIZE) {
                    fs->free_blocks[inode_block.inodes[idx].indirect] = false;

                    // Read the pointer block from memory 
                    Block block_pointers;
                    if(disk_read(fs->disk, inode_block.inodes[idx].indirect, (char*)(&block_pointers)) != BLOCK_SIZE) return false;

                    // Calculate left over in bytes
                    ssize_t leftoverblocks_bytes = (inode_block.inodes[idx].size - (POINTERS_PER_INODE * BLOCK_SIZE));
                    size_t curr = 0;

                    // while there are still bytes that are left over, we set the free blocks to false
                    while(leftoverblocks_bytes > 0){
                        fs->free_blocks[block_pointers.block_pointers[curr]] = false;
                        leftoverblocks_bytes -= BLOCK_SIZE;
                        curr += 1;
                    }
                }
            }
        }
    }
    return true;
}


/**
 * function that intialize the fs meta from the super block on disk
 * 1. if any of fs, disk or super_block is NUll, then return
 * 2. set fs disk
 * 3. set mounted = true
 * 4. set fs meta
**/
bool fs_initialize_meta(FileSystem *fs, Block* super_block, Disk* disk){
    if (fs == NULL || super_block == NULL || disk == NULL) return false;
    fs->disk = disk;
    disk->mounted = true;
    fs->meta = (super_block->super_block);
    return true;
}

/** function that verifies and edit the superblock to appropriate values
 * 1. checks if either disk or block is null
 * 2. magic_number = MAGIC_NUMBEr
 * 3. blocks = disk-> blocks
 * 4. inode_blocks = ceil(10% of total num of blocks)
 * 5. total inodes = inode_blocks * INODES per block
 * 6. total blocks = blocks (extended with block group in future)
**/
bool verify_superblock(Block* super_block, Disk* disk) {
    if(super_block == NULL || disk == NULL) return false;
    uint32_t num_inode_blocks = round(ceil(0.1 * disk->blocks));
    super_block->super_block.magic_number = MAGIC_NUMBER;
    super_block->super_block.blocks = disk->blocks;
    super_block->super_block.inode_blocks = num_inode_blocks;
    super_block->super_block.inodes = num_inode_blocks * INODES_PER_BLOCK;
    super_block->super_block.total_inodes = num_inode_blocks * INODES_PER_BLOCK;
    super_block->super_block.total_blocks = disk->blocks;
    return true;
}
