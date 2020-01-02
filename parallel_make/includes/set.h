/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once
#include "callbacks.h"
#include "compare.h"
#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>

#define ALPHA 0.7

/**
* In computer science, a set is an abstract data type that can store
* certain values, without any particular order, and no repeated values.
* It is a computer implementation of the mathematical concept of a finite set.
* Unlike most other collection types, rather than retrieving a specific element
* from a set, one typically tests a value for membership in a set.
*
* Some set data structures are designed for static or frozen sets that do not
* change after they are constructed. Static sets allow only query operations on
* their elements — such as checking whether a given value is in the set,
* or enumerating the values in some arbitrary order.
* Other variants, called dynamic or mutable sets, allow also the insertion
* and deletion of elements from the set.
*
* https://en.wikipedia.org/wiki/Set_(abstract_data_type)
*
* This set implementation also features an internal doubly linked list
* that runs from the oldest inserted element to the newest inserted element.
* Therefore, iteration through the set will always return elements in a FIFO
* pattern (removing an element and reinserting it will put it at the back
* of the queue).
*/

/* Forward declare set structure. */
typedef struct set set;

/**
 * Allocate and return a pointer to a new set (on the heap).
 *
 * All parameters are optional.
 *
 * If you would like to make the set store  'shallow' copies of the elements
 * inserted thereinto, then you may pass in NULL for the parameters (or use
 * shallow_set_create()).
 *
 * (ex. set_create(hash_function, comp, NULL, NULL)).
 *
 * This means that every time an element is to be copied or removed from the
 * set, the pointer to that element is copied or removed instead of using
 * a user supplied copy constructor and destructor.
 *
 * If you supply NULL for the hash_function, then all elements will hash to the
 * same index, causing all operations to require a linear scan of the set.
 *
 * If you supply NULL for the compare then elements will be compared by their
 * virtual address.
 *
 * WARNING: The hash function and comparator need to be consistent. i.e.
 * if two elements compare as equal, then they MUST have the same hash;
 * otherwise, accessing this set will result in undefined behavior.
 */
set *set_create(hash_function_type hash_function, compare comp,
                copy_constructor_type copy_constructor,
                destructor_type destructor);

/**
 * Destroys all container elements by calling on the user provided destructor
 * for every element, and deallocates all the storage capacity allocated by
 * the set.
 * Complexity: O(|this|)
 */
void set_destroy(set *this);

// Core Set-Theoretical Operations:
//
// Note: For set-theoretical operations it is undefined behavior if
// the two sets do not share the same:
// hash_function, compare function, copy_constructor, and destructor.
//

/**
 * Returns the union of sets 's' and 't'.
 * Complexity: O(|s| + |t|)
 */
set *set_union(set *s, set *t);

/**
 * Returns the intersection of sets 's' and 't'.
 * Complexity: O(min(|s|, |t|))
 */
set *set_intersection(set *s, set *t);

/**
 * Returns the difference of sets 's' and 't'.
 * Complexity: O(|t|)
 */
set *set_difference(set *s, set *t);

/**
 * Returns whether 's' is a subset of 't'.
 * Complexity: O(|s|)
 */
bool set_subset(set *s, set *t);

/**
 * Returns whether 's' equals 't'
 * (i.e. contain all and only the same elements).
 * Complexity: O(min(|s|, |t|))
 */
bool set_equals(set *s, set *t);

// Static Set Operations:

/**
 * Returns whether the set 'this' contains 'element'.
 * Complexity: O(1)
 */
bool set_contains(set *this, void *element);

/**
 * Returns the element that equals 'element' in 'this', where equality is
 * defined by the compare function.
 *
 * It's undefined behavior if the element can't be found.
 * Complexity: O(1)
 */
void *set_find(set *this, void *element);

/**
 * Returns whether 'this' is empty.
 * Complexity: O(1)
 */
bool set_empty(set *this);

/**
 * Returns the number of elements in 'this'.
 * Complexity: O(1)
 */
size_t set_cardinality(set *this);

/**
 * Returns a flat vector of the set's elements, for iteration. These elements
 * represent shallow copies of data present in the graph and therefore should
 * not be destroyed. The vector itself must be destroyed.
 *
 * The elements in the vector will be ordered based on their last time of
 * insertion into the set.
 *
 * Complexity: O(|this|)
 */
vector *set_elements(set *this);

// Dynamic Set Operations:

/**
 * Adds an 'element' to 'this', if not already present.
 * Complexity: O(1)
 */
void set_add(set *this, void *element);

/**
 * Removes an 'element' from 'this', if present.
 * Complexity: O(1)
 */
void set_remove(set *this, void *element);

/**
 * Removes all elements from 'this' and shrinks the set capacity to fit.
 * Complexity: O(|this|)
 */
void set_clear(set *this);

/**
 * Increase the capacity of this set (i.e. the maximum number of elements it
 * can hold before growing) to at least `capacity` This allows for faster
 * insertions of a large number of items into the set at the cost of higher
 * memory usage.
 *
 * Note that the actual resultant set capacity is implementation specific,
 * since the set capacity must be one of several prime numbers.
 *
 * If the current set capacity is greater than or equal to `capacity`, this
 * function does nothing.
 *
 * Complexity: O(|this|)
 */
void set_reserve(set *this, size_t capacity);

/**
 * Decrease the capacity of this set (i.e. the maximum number of elements it
 * can hold before growing) to the minimum needed to hold three times as many
 * as its current elements. This improved memory consumption comes at the
 * cost of a slightly longer time needed to insert a large number of elements
 * into the set.
 *
 * Note that the actual resultant set capacity is implementation specific,
 * since the bucket count must be one of several prime numbers.
 *
 * If the current set capacity is at a minimum or is greater than or equal to
 * three times the number of elements, this function does nothing.
 *
 * Complexity: O(|this|)
 */
void set_shrink_to_fit(set *this);

/**
 * Set iteration macro. `setname` is the name of the set. `varname` is the name
 * of a temporary (local) variable to refer to each element in the set, and
 * `callback` is a block of code that gets executed upon each element in the
 * set.
 *
 * Example usage:
 * ```
 *     set *s = string_set_create();
 *     set_add(s, "Hi!"); set_add(s, "I'm");
 *     set_add(s, "Set"); set_add(s, "\n");
 *     SET_FOR_EACH(s, thing, {
 *       fprintf("%s\n", (char *) thing);
 *       if (!strcmp((char *) thing, "Set")){
 *         break;
 *       }
 *     });
 * ```
 */
#define SET_FOR_EACH(setname, varname, callback)       \
    do {                                               \
        size_t _len = set_cardinality(setname);        \
        vector *_elements = set_elements(setname);     \
        for (size_t _i = 0; _i < _len; ++_i) {         \
            void *varname = vector_get(_elements, _i); \
            { callback; }                              \
        }                                              \
        vector_destroy(_elements);                     \
    } while (0)

// The following is code generated:

/**
 * Creates a set that performs no memory management and compares elements
 * purely by pointer value.
 */
set *shallow_set_create(void);

/**
 * Creates a set meant for string(s).
 */
set *string_set_create(void);

/**
 * Creates a set meant for char(s).
 */
set *char_set_create(void);

/**
 * Creates a set meant for double(s).
 */
set *double_set_create(void);

/**
 * Creates a set meant for float(s).
 */
set *float_set_create(void);

/**
 * Creates a set meant for int(s).
 */
set *int_set_create(void);

/**
 * Creates a set meant for long(s).
 */
set *long_set_create(void);

/**
 * Creates a set meant for short(s).
 */
set *short_set_create(void);

/**
 * Creates a set meant for unsigned char(s).
 */
set *unsigned_char_set_create(void);

/**
 * Creates a set meant for unsigned int(s).
 */
set *unsigned_int_set_create(void);

/**
 * Creates a set meant for unsigned long(s).
 */
set *unsigned_long_set_create(void);

/**
 * Creates a set meant for unsigned short(s).
 */
set *unsigned_short_set_create(void);
