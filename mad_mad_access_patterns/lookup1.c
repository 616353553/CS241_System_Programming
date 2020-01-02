/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void search_word(FILE *, char *);

/*
    Look up a few nodes in the tree and print the info they contain.
    This version uses fseek() and fread() to access the data.
    
    ./lookup1 <data_file> <word> [<word> ...]
*/

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printArgumentUsage();
        exit(1);
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        openFail(argv[1]);
        exit(1);
    }
    char header[BINTREE_ROOT_NODE_OFFSET + 1];
    fread(header, 1, BINTREE_ROOT_NODE_OFFSET, file);
    header[BINTREE_ROOT_NODE_OFFSET] = '\0';
    if (strcmp(header, BINTREE_HEADER_STRING) != 0)
    {
        formatFail(argv[1]);
        exit(1);
    }
    for (int i = 2; i < argc; ++i)
    {
        search_word(file, argv[i]);
    }
    fclose(file);
    return 0;
}

void search_word(FILE *file, char *word)
{
    assert(file != NULL && word != NULL);
    // reset the position to the beginning of the file with offset '4'
    fseek(file, BINTREE_ROOT_NODE_OFFSET, SEEK_SET);
    BinaryTreeNode *curr_node = malloc(sizeof(BinaryTreeNode));
    fread(curr_node, sizeof(BinaryTreeNode), 1, file);
    while (curr_node != NULL)
    {
        char buffer_size = 1;
        while (fgetc(file) != 0)
        {
            ++buffer_size;
        }
        // reset position
        fseek(file, 0 - buffer_size, SEEK_CUR);
        char *curr_word = malloc(buffer_size);
        fread(curr_word, 1, buffer_size, file);
        int result = strcmp(word, curr_word);
        if (result == 0)
        {
            printFound(word, curr_node->count, curr_node->price);
            free(curr_node);
            curr_node = NULL;
        }
        else if (result > 0)
        {
            if (curr_node->right_child != 0)
            {
                fseek(file, curr_node->right_child, SEEK_SET);
                fread(curr_node, sizeof(BinaryTreeNode), 1, file);
            }
            else
            {
                printNotFound(word);
                free(curr_node);
                curr_node = NULL;
            }
        }
        else
        {
            if (curr_node->left_child != 0)
            {
                fseek(file, curr_node->left_child, SEEK_SET);
                fread(curr_node, sizeof(BinaryTreeNode), 1, file);
            }
            else
            {
                printNotFound(word);
                free(curr_node);
                curr_node = NULL;
            }
        }
        free(curr_word);
    }
    free(curr_node);
}
