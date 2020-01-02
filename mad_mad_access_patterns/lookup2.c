/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

void search_word(void *, char *);

/*
    Look up a few nodes in the tree and print the info they contain.
    This version uses mmap to access the data.

    ./lookup2 <data_file> <word> [<word> ...]
*/

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printArgumentUsage();
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        openFail(argv[1]);
        exit(1);
    }
    struct stat file_stat;
    fstat(fd, &file_stat);
    void *addr = mmap(0, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
        mmapFail(argv[1]);
        exit(1);
    }
    char header[BINTREE_ROOT_NODE_OFFSET + 1];
    memcpy(header, addr, BINTREE_ROOT_NODE_OFFSET);
    header[BINTREE_ROOT_NODE_OFFSET] = '\0';
    if (strcmp(header, BINTREE_HEADER_STRING) != 0)
    {
        formatFail(argv[1]);
        exit(1);
    }
    for (int i = 2; i < argc; ++i)
    {
        search_word(addr, argv[i]);
    }
    munmap(addr, file_stat.st_size);
    close(fd);
    return 0;
}

void search_word(void *addr, char *word)
{
    assert(addr != NULL && word != NULL);
    BinaryTreeNode *curr_node = addr + BINTREE_ROOT_NODE_OFFSET;
    while (curr_node != NULL)
    {
        int result = strcmp(word, curr_node->word);
        if (result == 0)
        {
            printFound(word, curr_node->count, curr_node->price);
            curr_node = NULL;
        }
        else if (result > 0)
        {
            if (curr_node->right_child == 0)
            {
                printNotFound(word);
                curr_node = NULL;
            }
            else
            {
                curr_node = addr + curr_node->right_child;
            }
        }
        else
        {
            if (curr_node->left_child == 0)
            {
                printNotFound(word);
                curr_node = NULL;
            }
            else
            {
                curr_node = addr + curr_node->left_child;
            }
        }
    }
}
