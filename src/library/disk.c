// implementation of the disk emulator for simple FS
#include "../include/disk.h"
#include "../include/log.h"
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

// Perform sanity check
bool    disk_sanity_check(Disk *disk, size_t blocknum, const char *data);

// We have to read and write entire blocks to truly emulate a disk, 
// We can write to specific bytes in a disk, we have to read entire blocks and write entire blocks

/**
 *
 * Opens disk at specified path with the specified number of blocks by doing
 * the following:
 *
 * Allocates Disk structure and sets appropriate attributes.
 * Opens file descriptor to specified path.
 * Truncates file to desired file size (blocks * BLOCK_SIZE).
 *
 * @param       path        Path to disk image to create.
 * @param       blocks      Number of blocks to allocate for disk image.
 *
 * @return      Pointer to newly allocated and configured Disk structure (NULL (void pointer to 0)
 *              on failure).
 **/

Disk* disk_open(const char * path, size_t blocks) {
    // todo: possible to write a function or macro to make the intialization cleaner
    Disk* disk;
    int fd;
    // todo: check if theres a proper way to check this
    if(blocks == LONG_MAX){
        debug("Error in block size of %zu", blocks);
        return (void*)0;
    }
    // open file with create if non existant, read write permission
    if ((fd = open(path, O_CREAT|O_RDWR, 0777)) < 0) {
        debug("Error in opening file with path: %s due to: %s", path, strerror(errno));
        return (void*)0;
    };
    disk = malloc(sizeof(Disk));
    disk->blocks = blocks;
    disk->reads = 0;
    disk->writes = 0;
    // only set to true when FS is mounted
    disk->mounted = false;
    disk->fd = fd;
    return disk;
}

/**
 * Close disk structure by doing the following:
 *
 * Close disk file descriptor.
 * Report number of disk reads and writes.
 * Releasing disk structure memory.
 *
 * @param       disk        Pointer to Disk structure.
 */

void disk_close(Disk *disk) {
    // todo: possible to write a function or macro to make the intialization cleaner
    if(close(disk->fd) < 0) debug("error in closing: %s", strerror(errno)); ;
    free(disk);
}

/**
 * Read data from disk from specified block to data buffer by doing a sanity check, 
 * seeking the specified block and writing into the data buffer( must be block_size) from the disk block
 * 
 * @param disk 
 * @param block
 * @param data
 * 
 * @return whether or not it is safe to perform read/write operations
**/

ssize_t disk_read(Disk* disk, size_t block, char* data){
    if(!disk_sanity_check(disk, block, data)){
        return DISK_FAILURE;
    }
    int curr_offset;
    if((curr_offset = lseek(disk->fd, block * BLOCK_SIZE, SEEK_SET)) < 0) {
        debug("SEEK FAILED");
        return DISK_FAILURE;
    }
    if(read(disk->fd, data, BLOCK_SIZE) < 0 ){
        debug("error in reading: %s at block %zu at offset %d with val %d", strerror(errno), block * BLOCK_SIZE , curr_offset, errno); 
        return DISK_FAILURE;
    }
    disk->reads += 1;
    return BLOCK_SIZE;
}

/**
 * Write data to disk at specified block from data buffer by doing a sanity check, 
 * seeking the specified block and writing data buffer( must be block_size) to disk block
 * 
 * @param disk 
 * @param block
 * @param data
 * 
 * @return whether or not it is safe to perform read/write operations
**/
ssize_t disk_write(Disk *disk, size_t block, char *data) {
    if(!disk_sanity_check(disk, block, data)){
        return DISK_FAILURE;
    }
    if(lseek(disk->fd, block * BLOCK_SIZE, SEEK_SET) < 0) {
        debug("SEEK FAILED");
        return DISK_FAILURE;
    }
    if(write(disk->fd, data, BLOCK_SIZE) < 0 ){
        debug("error in writing: %s", strerror(errno)); 
        return DISK_FAILURE;
    }
    disk->writes += 1;
    return BLOCK_SIZE;
}

/**
 * Sanity check before read or write operation, check for valid disk, block and data
 * 
 * @param disk 
 * @param block
 * @param data
 * 
 * @return whether or not it is safe to perform read/write operations
**/
bool    disk_sanity_check(Disk *disk, size_t block, const char *data) {
    if(disk == (void*)0 || data == (void*)0 || block >= disk->blocks) return false;
    return true;
}
  