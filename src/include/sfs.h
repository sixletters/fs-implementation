
#include "disk.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// File System Constants

// Magic numbers are the first few bytes of a file that are unique to a particular file type. 
// These unique bits are referred to as magic numbers, also sometimes referred to as a file signature.
// These bytes can be used by the system to “differentiate between and recognize different files” without a file extension.
// The format routine places this number into the very first bytes of the superblock as a sort of filesystem "signature". 
// When the filesystem is mounted, the OS looks for this magic number. 
// If it is correct, then the disk is assumed to contain a valid filesystem. If some other number is present, then the mount fails, perhaps because the disk is not formatted or contains some other kind of data.
#define MAGIC_NUMBER        (0xf0f03410)  
#define INODES_PER_BLOCK    (128)   // Number of inodes per block
#define POINTERS_PER_INODE  (5)    // Number of direct pointers per inode
#define POINTERS_PER_BLOCK  (1024)  // Number of pointers per block

// File system structure

// Data structure that contains information like number of blocks, number of blocks for inode table etc.
typedef struct SuperBlock SuperBlock;
typedef struct Inode      Inode;
// we use union for block as it is a union data type, a type that could take up multiple types
typedef union  Block      Block;
// FileSystem contains information on disk FS is mounted on, as well as the superblock
typedef struct FileSystem FileSystem;

struct SuperBlock {
    uint32_t magic_number;
    uint32_t    blocks; // Number of blocks in per block group
    uint32_t    inode_blocks; // Number of blocks reserved for inodes per blockgroup
    uint32_t    inodes; // number of inodes per block group
};

struct Inode {
    uint32_t valid;
    uint32_t size;
    uint32_t    direct[POINTERS_PER_INODE]; // an array of uint32, where each number represents a pointer or "block number", not pointer is not an actual pointer.
    uint32_t    indirect;  // block number or "pointer" to indirect block of pointer
};

// A block of data is 4KB, and is a union of the different types it can take on
union Block {
    SuperBlock super_block;
    Inode inodes[INODES_PER_BLOCK];
    uint32_t block_pointers[POINTERS_PER_BLOCK];
    char data[BLOCK_SIZE];
};
