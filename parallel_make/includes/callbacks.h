/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once
#include <stdbool.h>
#include <stdlib.h>
/**
 * This header file contains all the callbacks used for our containers.
 */

/**
 * Declaration for a callback function that:
 *
 * Takes in a void pointer to arbitary data.
 * Allocates heap memory for said data.
 * Copies the values over from said data into the allocated heap memory.
 * Return a void pointer to the copy of said data on heap memory.
 *
 * Note that when a user adds an element to the container this function is used.
 */
typedef void *(*copy_constructor_type)(void *elem);

/**
 * Declaration for a callback function that:
 *
 * Takes in a void pointer to arbitary data on the heap.
 * Frees up the memory that data is taking up on the heap.
 *
 * Note that when a user removes an element from the container this function is
 * used.
*/
typedef void (*destructor_type)(void *elem);

/**
 * Declaration for a callback function that:
 *
 * Returns a pointer representing
 * the default value of the type of elements the container is storing.
 *
 * Note that whenever a default value of an object is needed this function is
 * used.
*/
typedef void *(*default_constructor_type)(void);

/**
 * Callback function the user should provide that takes in a pointer to
 * arbitrary data and returns a hash value to be used by the container.
 */
typedef size_t (*hash_function_type)(void *elem);

/**
 * The following are 'shallow' callback functions.
 * These are to be used when the user does not provide
 * their own callback functions for the container.
 */

/**
 * Returns a shallow copy of the pointer passed into this
 * copy constructor. This means
 *   `shallow_copy_constructor(elem) == elem`
 * evaluates to true.
 */
void *shallow_copy_constructor(void *elem);

/**
 * This function does nothing.
 */
void shallow_destructor(void *elem);

/**
 * This function returns NULL.
 */
void *shallow_default_constructor(void);

/**
 * This function returns 0, which means that its usage in a dictionary
 * or set ensures that all elements will be placed in the same hash
 * bucket.
 *
 * This is advantageous because it means that the user will never
 * encounter a "hash miss" (i.e. the hash function will never fail to find
 * the right bucket); this is disadvantageous because the data structure will
 * have O(n) runtime performance for most operations.
 */
size_t shallow_hash_function(void *elem);

/**
 * hash function for pointers.
 */

/**
 * This function simply returns whatever is passed into it casted as a void
 * pointer. That is to say,
 *   pointer_hash_function(elem) == (void *) elem
 * always evaluates as true.
 *
 * Unlike shallow_hash_function(), this should provide roughly O(1) operations
 * for set and dictionary.
 *
 * If any compare function other than shallow_compare() or default_compare() is
 * used, do NOT use this hash function! You might instead want to use
 * shallow_hash_function().
 */
size_t pointer_hash_function(void *elem);

/**
 * string callback functions.
 */

/**
 * Wrapper for strdup(). Returns a dynamically allocated
 * deep copy of 'elem', which must be a valid C string
 */
void *string_copy_constructor(void *elem);

/**
 * Wrapper for free.
 */
void string_destructor(void *elem);

/**
 * Returns a dynamically allocated NULL byte (empty C string).
 */
void *string_default_constructor(void);

/**
 * Uses the djb2 algorithm to generate a hash for 'elem', which must be a
 * valid C-string (or NULL pointer). The implementation is a modified version
 * of the implementation found here:
 *
 * http://www.cse.yorku.ca/~oz/hash.html
 *
 * the only difference being that NULL strings are hashed to 163.
 */
size_t string_hash_function(void *elem);

// The following is code generated:

/**
 * char callback functions.
 */
void *char_copy_constructor(void *elem);

void char_destructor(void *elem);

void *char_default_constructor(void);

size_t char_hash_function(void *elem);

/**
 * double callback functions.
 */
void *double_copy_constructor(void *elem);

void double_destructor(void *elem);

void *double_default_constructor(void);

size_t double_hash_function(void *elem);

/**
 * float callback functions.
 */
void *float_copy_constructor(void *elem);

void float_destructor(void *elem);

void *float_default_constructor(void);

size_t float_hash_function(void *elem);

/**
 * int callback functions.
 */
void *int_copy_constructor(void *elem);

void int_destructor(void *elem);

void *int_default_constructor(void);

size_t int_hash_function(void *elem);

/**
 * long callback functions.
 */
void *long_copy_constructor(void *elem);

void long_destructor(void *elem);

void *long_default_constructor(void);

size_t long_hash_function(void *elem);

/**
 * short callback functions.
 */
void *short_copy_constructor(void *elem);

void short_destructor(void *elem);

void *short_default_constructor(void);

size_t short_hash_function(void *elem);

/**
 * unsigned char callback functions.
 */
void *unsigned_char_copy_constructor(void *elem);

void unsigned_char_destructor(void *elem);

void *unsigned_char_default_constructor(void);

size_t unsigned_char_hash_function(void *elem);

/**
 * unsigned int callback functions.
 */
void *unsigned_int_copy_constructor(void *elem);

void unsigned_int_destructor(void *elem);

void *unsigned_int_default_constructor(void);

size_t unsigned_int_hash_function(void *elem);

/**
 * unsigned long callback functions.
 */
void *unsigned_long_copy_constructor(void *elem);

void unsigned_long_destructor(void *elem);

void *unsigned_long_default_constructor(void);

size_t unsigned_long_hash_function(void *elem);

/**
 * unsigned short callback functions.
 */
void *unsigned_short_copy_constructor(void *elem);

void unsigned_short_destructor(void *elem);

void *unsigned_short_default_constructor(void);

size_t unsigned_short_hash_function(void *elem);
