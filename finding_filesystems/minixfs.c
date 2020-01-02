/**
* Finding Filesystems Lab
* CS 241 - Fall 2018
*/

#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    inode *nody = get_inode(fs, path);
    if (nody != NULL) {
        nody->mode = new_permissions | ((nody->mode >> RWX_BITS_NUMBER) << RWX_BITS_NUMBER);
        clock_gettime(CLOCK_REALTIME, &nody->ctim);
        return 0;
    }
    if (access(path,F_OK) == -1)
        errno = ENOENT;
    return -1;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    inode *nody = get_inode(fs, path);
    if (nody != NULL) {
        nody->uid = (owner != nody->uid - 1) ? owner : nody->uid;
        nody->gid = (group != nody->gid - 1) ? group : nody->gid;
        clock_gettime(CLOCK_REALTIME, &nody->ctim);
        return 0;
    }
    if (access(path,F_OK) == -1)
        errno = ENOENT;
    return -1;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    if (get_inode(fs, path) != NULL) {
        return NULL;
    }
    const char *filename;
    // get parent's inode
    inode *parent_node = parent_directory(fs, path, &filename);
    if (valid_filename(filename) != 1) {
        // if filename is invalid, return NULL
        return NULL;
    }
    // number of fully-used data blocks
    data_block_number num_block = (parent_node->size) / sizeof(data_block);
    size_t remainder = parent_node->size % sizeof(data_block);
    // if all direct data blocks are in-use, add the first indirect data block
    if (num_block >= NUM_DIRECT_INODES && parent_node->indirect == UNASSIGNED_NODE) {
        if (add_single_indirect_block(fs, parent_node) == -1) {
            return NULL;
        }
    }
    inode_number new_node_idx = first_unused_inode(fs);
    if (new_node_idx == -1) {
        // no more available inodes
        return NULL;
    }
    init_inode(parent_node, fs->inode_root + new_node_idx);
    data_block *last_data_block;
    if (remainder == 0) {
        // needs a new data block
        data_block_number new_data_block_idx;
        if (parent_node->indirect == UNASSIGNED_NODE) {
            new_data_block_idx = add_data_block_to_inode(fs, parent_node);
        } else {
            data_block_number *indirect_blocks = (data_block_number *)(fs->data_root + parent_node->indirect);
            new_data_block_idx = add_data_block_to_indirect_block(fs, indirect_blocks);
        }
        if (new_data_block_idx == -1) {
            // no more available data blocks
            return NULL;
        }
        last_data_block = fs->data_root + new_data_block_idx;        
    } else {
        if (parent_node->indirect == UNASSIGNED_NODE) {
            last_data_block = fs->data_root + parent_node->direct[num_block];
        } else {
            data_block_number *indirect_blocks = (data_block_number *)(fs->data_root + parent_node->indirect);
            last_data_block = fs->data_root + indirect_blocks[num_block - NUM_DIRECT_INODES];
        }
    }
    char cpy[FILE_NAME_LENGTH];
    strncpy(cpy, filename, FILE_NAME_LENGTH);
    memcpy(((char *)last_data_block) + remainder, cpy, FILE_NAME_LENGTH);
    sprintf(((char *)last_data_block) + remainder + FILE_NAME_LENGTH, "%08zx", (size_t)new_node_idx);
    parent_node->size += FILE_NAME_ENTRY;
    return fs->inode_root + new_node_idx;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf, size_t count, off_t *off) {
    (void) buf;
    (void) count;
    (void) off;
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
    }
    inode *nody = get_inode(fs, path);
    if (nody == NULL) {
        errno = ENOENT;
        return -1;
    }
    size_t byte_read = 0;
        
    return byte_read;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf, size_t count, off_t *off) {
    inode *nody = get_inode(fs, path);
    if (nody == NULL) {
        nody = minixfs_create_inode_for_path(fs, path);
        if (nody == NULL) {
            errno = ENOSPC;
            return -1;
        }
    }
    size_t block_required = (*off + count) / sizeof(data_block);
    if ((*off + count) % sizeof(data_block) != 0) {
        ++block_required;
    }
    if (minixfs_min_blockcount(fs, path, (int)block_required) == -1) {
        errno = ENOSPC;
        return -1;
    }
    size_t block_index = *off / sizeof(data_block);
    size_t block_offset = *off % sizeof(data_block);
    off_t end = *off + count;
    size_t byte_write = 0;
    nody->size = *off + count;
    while (*off < end) {
        data_block curr_block;
        if (block_index >= NUM_DIRECT_INODES) {
            data_block_number *indirect_blocks = (data_block_number *)(fs->data_root + nody->indirect);
            curr_block = fs->data_root[indirect_blocks[block_index - NUM_DIRECT_INODES]];
        } else {
            curr_block = fs->data_root[nody->direct[block_index]];
        }
        size_t byte_to_write = sizeof(data_block) - block_offset;
        byte_to_write = ((end - *off) < (off_t)byte_to_write) ? (end - *off) : byte_to_write;
        memcpy(curr_block.data, buf + byte_to_write, byte_to_write);
        *off += byte_to_write;
        byte_write += byte_to_write;
        block_offset = 0;
        ++block_index;
    }
    clock_gettime(CLOCK_REALTIME, &nody->mtim);
    clock_gettime(CLOCK_REALTIME, &nody->atim);
    return byte_write;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count, off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    inode *nody = get_inode(fs, path);
    if (nody == NULL) {
        errno = ENOENT;
        return -1;
    }
    if ((uint64_t)*off >= nody->size) {
        return 0;
    }
    size_t block_index = *off / sizeof(data_block);
    size_t block_offset = *off % sizeof(data_block);
    off_t end = ((*off + count) >= nody->size) ? nody->size : (*off + count);
    size_t byte_read = 0;
    while (*off < end) {
        data_block curr_block;
        if (block_index >= NUM_DIRECT_INODES) {
            data_block_number *indirect_blocks = (data_block_number *)(fs->data_root + nody->indirect);
            curr_block = fs->data_root[indirect_blocks[block_index - NUM_DIRECT_INODES]];
        } else {
            curr_block = fs->data_root[nody->direct[block_index]];
        }
        size_t byte_to_read = sizeof(data_block) - block_offset;
        byte_to_read = ((end - *off) < (off_t)byte_to_read) ? (end - *off) : byte_to_read;
        memcpy(buf + byte_read, curr_block.data, byte_to_read);
        *off += byte_to_read;
        byte_read += byte_to_read;
        block_offset = 0;
        ++block_index;
    }
    clock_gettime(CLOCK_REALTIME, &nody->atim);
    return byte_read;
}
