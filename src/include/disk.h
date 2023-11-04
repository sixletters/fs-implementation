// Disk emulator for Sfs 

#ifndef DISK_H
#define DISK_H

#include <stdbool.h>
#include <stdlib.h>

// Disk Constants
#define BLOCK_SIZE      (1<<12)   // 4KB, compilation will replace BLOCK_SIZE with 1 bitshifted left by 12
#define DISK_FAILURE    (-1)


// Define typedef so we would not have to keep typing typedef struct Disk
// size_t is an unsigned integer type defined by several C/C++ standards
// it is chosen so that it can store the maximum theoretically possible array or object. 
// In other words, the number of bits in size_t is equal to the number of bits required to store the maximum address in the machine's memory.
// For e.g on a 32 bit system, size_t will take 32 bits. It just means a variable of size_t can safely store a pointer.
// Most frequently compiler-based operator sizeof should evaluate to a constant value that is compatitble with size_t
// Used frequently for array indexing -> cannot be negative!
typedef struct Disk Disk;
struct Disk {
    int fd; // file descriptor for disk emulator
    size_t blocks; // number of blocks in disk
    size_t reads; // number of reads to disk
    size_t writes; // number of writes to disk
    bool mounted; // whether disk is mounted
};

// Disk Functions

Disk*	disk_open(const char *path, size_t blocks);
void	disk_close(Disk *disk);

size_t	disk_read(Disk *disk, size_t block, char *data);
size_t	disk_write(Disk *disk, size_t block, char *data);

#endif