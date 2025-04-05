/* File system header (FS.H) */
#ifndef _FS_H
#define _FS_H

#include <stdint.h>

/**
 * Define values for file system constants
 */
/* Partition type */
#define SIMPLE_PARTITION         0x1111

/* Inode constants */
#define INVALID_INODE            0

/* Permission bits for inode mode */
#define INODE_MODE_AC_USER_R     0x001
#define INODE_MODE_AC_USER_W     0x002
#define INODE_MODE_AC_USER_X     0x004
#define INODE_MODE_AC_OTHER_R    0x010
#define INODE_MODE_AC_OTHER_W    0x020
#define INODE_MODE_AC_OTHER_X    0x040
#define INODE_MODE_AC_GRP_R      0x100
#define INODE_MODE_AC_GRP_W      0x200
#define INODE_MODE_AC_GRP_X      0x400
#define INODE_MODE_AC_ALL        0x777

/* File type bits for inode mode */
#define INODE_MODE_REG_FILE      0x10000
#define INODE_MODE_DIR_FILE      0x20000
#define INODE_MODE_DEV_FILE      0x40000

/* Directory entry types */
#define DENTRY_TYPE_REG_FILE     0x1
#define DENTRY_TYPE_DIR_FILE     0x2

/* Block size and related constants */
#define BLOCK_SIZE               0x400  /* 1KB */
#define MAX_DIRECT_BLOCKS        6
#define MAX_FILENAME_LENGTH      256
#define MAX_VOLUME_NAME_LENGTH   24

/**
 * Partition structure
 * ASSUMPTION: data block size: 1KB
 *
 * Partition layout (4MB):
 * - Superblock: 1KB
 * - Inode table: 7KB (224 inodes * 32 bytes each)
 * - Data blocks: ~4KB (1KB blocks array)
 */

/**
 * Super block structure
 */
typedef struct super_block {
    uint32_t partition_type;        /* Partition identifier */
    uint32_t block_size;            /* Size of each block in bytes */
    uint32_t inode_size;            /* Size of each inode in bytes */
    uint32_t first_inode;           /* Index of the first inode */

    uint32_t num_inodes;            /* Total number of inodes */
    uint32_t num_inode_blocks;      /* Number of blocks used for inodes */
    uint32_t num_free_inodes;       /* Number of unused inodes */

    uint32_t num_blocks;            /* Total number of data blocks */
    uint32_t num_free_blocks;       /* Number of unused data blocks */
    uint32_t first_data_block;      /* Index of the first data block */
    
    char volume_name[MAX_VOLUME_NAME_LENGTH]; /* Volume name */
    uint8_t padding[960];           /* Padding to make struct 1024 bytes */
} super_block_t;

/**
 * 32-byte inode structure
 */
typedef struct inode {
    uint32_t mode;                  /* File type and permissions */
    uint32_t locked;                /* 1 if opened for write, 0 otherwise */
    uint32_t date;                  /* Creation/modification timestamp */
    uint32_t size;                  /* File size in bytes */
    int32_t  indirect_block;        /* Index to indirect block, -1 if unused */
    uint16_t blocks[MAX_DIRECT_BLOCKS]; /* Direct block pointers */
} inode_t;

/**
 * Data block structure
 */
typedef struct block {
    uint8_t data[BLOCK_SIZE];       /* Block data */
} block_t;

/**
 * Physical partition structure
 */
typedef struct partition {
    super_block_t super_block;      /* Superblock */
    inode_t inode_table[224];       /* Inode table */
    block_t data_blocks[4088];      /* Data blocks (4096-8) */
} partition_t;

/**
 * Directory entry structure
 */
typedef struct dentry {
    uint32_t inode;                 /* Inode number */
    uint32_t dir_length;            /* Length of this directory entry */
    uint32_t name_len;              /* Length of the filename */
    uint32_t file_type;             /* Type of file */
    union {                         /* Filename */
        uint8_t name[MAX_FILENAME_LENGTH];
        uint8_t n_pad[16][16];      /* Padding for alignment */
    };
} dentry_t;

/* File system function prototypes would go here */

#endif /* _FS_H */