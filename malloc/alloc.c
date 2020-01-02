/**
* Malloc Lab
* CS 241 - Fall 2018
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct meta_data {
	char free;
	size_t size;
	// prev empty block
	struct meta_data *prev;
	// next empty block
	struct meta_data *next;
} mata_data;

static mata_data* free_lists_head[16];
static mata_data* free_lists_tail[16];

static size_t sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072};

static int defrag_count = 0;


int get_list_num(size_t);
mata_data* grow_heap(size_t);
void recycle_block(mata_data*);
void detach(mata_data*, int);
void merge_with_next_if_possible(mata_data*);
void split_block_if_possible(mata_data*, size_t);
mata_data* find_first_fit(size_t, int);
mata_data* find_best_fit(size_t, int);
void defrag(int);

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
	void *ptr = malloc(num * size);
	if (ptr != NULL)
		memset(ptr, 0, num * size);
	return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
	if (size == 0) {
		return NULL;
	}
	int list_num = get_list_num(size);
	if (list_num == 15) {
		defrag(list_num);
	} else if (list_num == 14) {
		defrag_count++;
		if (defrag_count > 6) {
			defrag(list_num);
			defrag_count = 0;
		}
	}
	mata_data *block = NULL;
	// search for fitting block
	if (free_lists_head[list_num] != NULL) {
		block = (get_list_num(size) >= 5) ? find_best_fit(size, list_num) : find_first_fit(size, list_num);
		if (block != NULL) {
			detach(block, list_num);
			block->free = 0;
			split_block_if_possible(block, size);
			return (void *)(block + 1);
		}
	}
	// ask for more memory
	block = grow_heap(size);
	if (block == NULL)
		return NULL;
	return (void *)(block + 1);
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
	if (ptr == NULL || ptr >= sbrk(0)) {
		return;
	}
	mata_data* to_free = (mata_data *)ptr - 1;
	if (to_free->free == 1) {
		return;
	}
	merge_with_next_if_possible(to_free);
	if (get_list_num(to_free->size) >= 5 && (ptr + to_free->size >= sbrk(0))) {
		sbrk(0 - (to_free->size + sizeof(mata_data)));
		return;
	}
	to_free->free = 1;
	recycle_block(to_free);
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
	if (ptr == NULL) {
		return malloc(size);
	}
	if (size == 0) {
		free(ptr);
		return NULL;
	}
	mata_data* to_realloc = (void *)ptr - sizeof(mata_data);
	if (size <= to_realloc->size) {
		split_block_if_possible(to_realloc, size);
		return ptr;
	} else {
		merge_with_next_if_possible(to_realloc);
		if (to_realloc->size >= size) {
			return ptr;
		}
		void* new_ptr = malloc(size);
		if (new_ptr == NULL) {
			return NULL;
		}
		memcpy(new_ptr, ptr, to_realloc->size);
		free(ptr);
		return new_ptr;
	}
}


/**
 * Get the index of empty list with given size
 */
int get_list_num(size_t size) {
	for (int i = 0; i < 15; ++i) {
		if (size <= sizes[i]) {
			return i;
		}
	}
	return 15;
}

/**
 * Increase the heap
 */
mata_data* grow_heap(size_t size) {
	mata_data* new_block = sbrk(sizeof(mata_data) + size);
	if (new_block == (void*) -1) {
		return NULL;
	}
	*new_block = (mata_data){0, size, NULL, NULL};
	return new_block;
}

/**
 * Recycle a block by inserting it to the begining of an empty list
 */
void recycle_block(mata_data* node) {
    if (node == NULL)
        return;
	int list_num = get_list_num(node->size);
	node->prev = NULL;
	node->next = free_lists_head[list_num];
	if (free_lists_head[list_num] == NULL)
		free_lists_tail[list_num] = node;
	else
		free_lists_head[list_num]->prev = node;
	free_lists_head[list_num] = node;
}


/**
 * Detach a block from given target list.
 */
void detach(mata_data* node, int list_num) {
	mata_data* prev = node->prev;
	mata_data* next = node->next;
	if (prev == NULL && next == NULL) {
		free_lists_head[list_num] = NULL;
		free_lists_tail[list_num] = NULL;
	} else if (prev == NULL) {
		next->prev = NULL;
		free_lists_head[list_num] = next;
	} else if (next == NULL) {
		prev->next = NULL;
		free_lists_tail[list_num] = prev;
	} else {
		prev->next = next;
		next->prev = prev;
	}
	node->next = NULL;
	node->prev = NULL;
}

/**
 * Merge given block with its next block if possible.
 */
void merge_with_next_if_possible(mata_data* node) {
	mata_data* next_block = (void*)(node + 1) + node->size;
	if ((void*) next_block < sbrk(0) && next_block->free == 1) {
		int next_list_num = get_list_num(next_block->size);
		detach(next_block, next_list_num);
		node->size += sizeof(mata_data) + next_block->size;
	}
}

/**
 * Split a block into 2 if possible
 */
void split_block_if_possible(mata_data* to_split, size_t new_size) {
    if (to_split == NULL) {
        return;
    }
	if (to_split->size > new_size + sizeof(mata_data) + 4) {
		mata_data* new_block = (void *)(to_split + 1) + new_size;
		new_block->size = to_split->size - new_size - sizeof(mata_data);
		new_block->free = 1;
		recycle_block(new_block);
		to_split->size = new_size;
	}
}

/**
 * Find the first block in the given empty list where the size can fit the block.
 */
mata_data* find_first_fit(size_t size, int list_num) {
	mata_data* curr = free_lists_head[list_num];
	while (curr != NULL) {
		if (curr->size >= size) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

/**
 * Find the best-fit block from empty list.
 * Return NULL if nothing is found
 */
mata_data* find_best_fit(size_t size, int list_num) {
	mata_data* best_fit = NULL;
	mata_data* curr;
	for (curr = free_lists_head[list_num]; curr; curr = curr->next) {
		if (curr->size >= size
				&& ((!best_fit) || (best_fit->size > curr->size))) {
			best_fit = curr;
		}
	}
	return best_fit;
}


/**
 * Merge blocks with its neighbor if possible
 */
void defrag(int list_num) {
	mata_data *curr = free_lists_head[list_num];
	while (curr != NULL) {
		if (curr->free == 1) {
			merge_with_next_if_possible(curr);
		}
		if (((void*) ((char*) curr + sizeof(mata_data) + curr->size)) >= sbrk(0)) {
			detach(curr, list_num);
			sbrk(0 - (curr->size + sizeof(mata_data)));
			return;
		}
		curr = curr->next;
	}
}
