// implementation of the disk emulator for simple FS
#include "../include/disk.h"
#include "../include/log.h"
#include <fcntl.h>
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
 * @return      Pointer to newly allocated and configured Disk structure (NULL
 *              on failure).
 **/

Disk* disk_open(const char * path, size_t blocks) {
    return NULL;
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
    return DISK_FAILURE;
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
    return DISK_FAILURE;
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
    return false;
}