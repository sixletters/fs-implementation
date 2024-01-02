#ifndef FS_H
#define FS_H

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
typedef struct GroupsDescriptor GroupsDescriptor;
// we use union for block as it is a union data type, a type that could take up multiple types
typedef union  Block      Block;
// FileSystem contains information on disk FS is mounted on, as well as the superblock
typedef struct FileSystem FileSystem;

// The super block is completely empty besides 24 bytes of data, we can add in a bitmap in here in the future
struct SuperBlock {
    uint32_t magic_number;
    uint32_t total_blocks; // total number of blocks in FS
    uint32_t total_inodes; // total number of inodes in FS

    uint32_t    blocks; // total number of blocks per block group
    uint32_t    inode_blocks; // number of blocks reserved for inodes per blockgroup, total number of data blocks, would be total number of blocks - 1 (superblocl) -  
    uint32_t    inodes; // number of inodes per block group
};

// this shall be extended in the future
// struct GroupsDescriptor {
//     uint32_t magic_number;
//     uint32_t    blocks; // Number of blocks per block group
//     uint32_t    inode_blocks; // Number of blocks reserved for inodes per blockgroup
//     uint32_t    inodes; // number of inodes per block group
// };

// 5 * 4 bytes( uin32_t ) ( the direct pointers) +  3 *  4bytes = 32 bytes size of one Inode structure
// extend to have 2 and 3 indirect pointers as well
struct Inode {
    uint32_t valid;
    uint32_t size;
    uint32_t    direct[POINTERS_PER_INODE]; // an array of uint32, where each number represents a pointer or "block number", not pointer is not an actual pointer.
    uint32_t    indirect;  // block number or "pointer" to indirect block of pointer
};

// A block of data is 4KB, and is a union of the different types it can take on
union Block {
    SuperBlock super_block; // 24 bytes only
    // GroupsDescriptor groups_descriptor; 
    Inode inodes[INODES_PER_BLOCK]; // 32 * 128 (Inodes per block -> 4096 / 32 = 128)
    uint32_t block_pointers[POINTERS_PER_BLOCK]; // a pointer is 4 bytes, POINTERS per block = 4096/4 = 1028
    char data[BLOCK_SIZE]; // 4096 bytes
};

struct FileSystem {
    Disk *disk;
    bool *free_blocks; // free block bit map, currently an in memory array of free blocks, to be extended to be on disk in the future
    SuperBlock meta; // FS metadata
};

// sfs functions
// Right not we shall only read and write to an inode number, in the future we will provde
// a directory structure as well as mapping to name
void fs_debug(Disk *disk);
bool fs_format(Disk *disk);
// mount the file system
bool    fs_mount(FileSystem *fs, Disk *disk);
// unmount the file system from a mountpoint
void    fs_unmount(FileSystem *fs);
ssize_t fs_create(FileSystem *fs);
// remove an inode from a file system, same as rm
bool    fs_remove(FileSystem *fs, size_t inode_number);
ssize_t fs_stat(FileSystem *fs, size_t inode_number);

// Read and write to an inode, inputs being data to be written or read to, the size as well as the offset.
ssize_t fs_read(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset);
ssize_t fs_write(FileSystem *fs, size_t inode_number, char *data, size_t length, size_t offset);

// utility functions needed
// superblock to/from bytes
bool superblock_to_bytes(Block* supser_block, char* data);
bool superblock_from_bytes(Block* supser_block, char* data);

// inode table to/from bytes
bool inode_table_from_bytes(Block* inode_table, char* data);
bool inode_table_to_bytes(Block* inode_table, char* data);

// block pointser to/from bytes
bool block_pointers_from_bytes(Block* block_pointers, char *data);
bool block_pointers_to_bytes(Block* block_pointers, char *data);

// intializes the free block bitmap of fs meta
bool fs_initialize_free_block_bitmap(FileSystem *fs);
// intializes the meta of fs
bool fs_initialize_meta(FileSystem *fs, Block* super_block, Disk* disk);
// retrieves the super block (block 0) from disk
bool get_superblock_from_disk(Disk* disk, Block* super_block);
// verifies and sets appropriate values for super_block
bool verify_superblock(Block* super_block, Disk* disk);
#endif