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
/**
 * In computer science, an associative array, map, symbol table, or dictionary
 * is an abstract data type composed of a collection of (key, value) pairs,
 * such that each possible key appears at most once in the collection.
 *
 * Operations associated with this data type allow:
 *   -addition of a pair to the collection
 *   -removal of a pair from the collection
 *   -modification of an existing pair (provided that the key modification does
 *    not alter its hash or compare behavior)
 *   -lookup of a value associated with a particular key
 *
 * https://en.wikipedia.org/wiki/Associative_array
 *
 * Internally, this dictionary is implemented using a set of key-value pairs,
 * and therefore will share similar characteristics to the set, such as FIFO
 * key iteration.
 */

/* Forward declare dictionary structure. */
typedef struct dictionary dictionary;

struct key_value_pair {
    void *key;
    void **value;
};

typedef struct key_value_pair key_value_pair;

/**
 * Allocate and return a pointer to a new dictionary (on the heap).
 *
 * If you would like to make the dictionary store 'shallow' copies of the
 * elements passed in as keys and values thereto, then you may pass in NULL
 * for all the parameters (or use shallow_to_shallow_dictionary_create())
 *
 * (ex.
 *  dictionary_create(hash_function, compare, NULL, NULL, NULL, NULL)).
 *
 * This means that every time an element is added to or removed from the
 * dictionary, the pointer to that element is copied or removed instead of
 * using the user supplied copy constructor and destructor.
 *
 * If you supply NULL for the hash_function then all elements will hash to the
 * same index causing most operations to require a linear scan of the
 * dictionary.
 *
 * If you supply NULL for the compare then elements will be compared by their
 * virtual address (pointer value).
 */
dictionary *dictionary_create(hash_function_type hash_function, compare comp,

                              copy_constructor_type key_copy_constructor,
                              destructor_type key_destructor,

                              copy_constructor_type value_copy_constructor,
                              destructor_type value_destructor);

/**
 * Destroys all container elements by calling on the user provided destructor
 * for every element, and deallocates all the storage capacity allocated by
 * the dictionary.
 * Complexity: O(n)
 */
void dictionary_destroy(dictionary *this);

// Capacity:

/**
 * Returns whether 'this' is empty.
 * Complexity: O(1)
 */
bool dictionary_empty(dictionary *this);

/**
 * Returns the number of elements in 'this'.
 * Complexity: O(1)
 */
size_t dictionary_size(dictionary *this);

// Accessors and Modifiers

/**
 * Returns whether 'key' is present as a key of 'this'.
 * Complexity: O(1)
 */
bool dictionary_contains(dictionary *this, void *key);

/**
 * Returns the value of 'key' in 'this'. The return value is a shallow copy of
 * the data present within the graph.
 *
 * If 'key' does not exist in 'this', this method results in undefined
 * behavior.
 * Complexity: O(1)
 */
void *dictionary_get(dictionary *this, void *key);

/**
 * Returns a key-value pair, which allows the user to get or set the value
 * associated with 'key' directly. If 'key' does not exist in 'this', then
 * this method returns a key_value_pair with NULL 'key' and 'value' members.
 * Complexity: O(1)
 */
key_value_pair dictionary_at(dictionary *this, void *key);

/**
 * Adds the 'key'-'value' pair into 'this'.
 *
 * If 'key' already exist in 'this', then the old value is destroyed and
 * replaced with 'value'.
 * Complexity: O(1)
 */
void dictionary_set(dictionary *this, void *key, void *value);

/**
 * Removes 'key' and it's associated value from 'this'.
 *
 * If 'key' does not exist in 'this', this method results in undefined
 * behavior.
 * Complexity: O(1)
 */
void dictionary_remove(dictionary *this, void *key);

/**
 * Destroys all elements of 'this'.
 * Complexity: O(n)
 */
void dictionary_clear(dictionary *this);

/**
 * Returns a vector that contains all the keys of 'this'. These keys
 * (but not the vector itself) represent shallow copies of data present
 * in the dictionary and therefore should not be destroyed.
 *
 * The keys will be ordered in a FIFO order, i.e. keys last inserted earlier
 * will come before keys inserted later.
 * Complexity: O(n)
 */
vector *dictionary_keys(dictionary *this);

/**
 * Returns a vector that contains all the values of 'this'. These
 * values (but not the vector itself) represent shallow copies of data present
 * in the graph and therefore should not be destroyed.
 *
 * The values will be ordered the same as their corresponding keys.
 *
 * Complexity: O(n)
 */
vector *dictionary_values(dictionary *this);

// The following is code generated:

/* Note: 'shallow' implies that the dictionary uses the
 * pointer hash function and shallow comparator (for shallow keys) and
 * shallow copy constructors and destructors.
 */

/**
 * Creates a dictionary which maps shallow(s) to shallow(s).
 */
dictionary *shallow_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to string(s).
 */
dictionary *shallow_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to char(s).
 */
dictionary *shallow_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to double(s).
 */
dictionary *shallow_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to float(s).
 */
dictionary *shallow_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to int(s).
 */
dictionary *shallow_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to long(s).
 */
dictionary *shallow_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to short(s).
 */
dictionary *shallow_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to unsigned char(s).
 */
dictionary *shallow_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to unsigned int(s).
 */
dictionary *shallow_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to unsigned long(s).
 */
dictionary *shallow_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps shallow(s) to unsigned short(s).
 */
dictionary *shallow_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to shallow(s).
 */
dictionary *string_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to string(s).
 */
dictionary *string_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to char(s).
 */
dictionary *string_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to double(s).
 */
dictionary *string_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to float(s).
 */
dictionary *string_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to int(s).
 */
dictionary *string_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to long(s).
 */
dictionary *string_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to short(s).
 */
dictionary *string_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to unsigned char(s).
 */
dictionary *string_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to unsigned int(s).
 */
dictionary *string_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to unsigned long(s).
 */
dictionary *string_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps string(s) to unsigned short(s).
 */
dictionary *string_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to shallow(s).
 */
dictionary *char_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to string(s).
 */
dictionary *char_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to char(s).
 */
dictionary *char_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to double(s).
 */
dictionary *char_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to float(s).
 */
dictionary *char_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to int(s).
 */
dictionary *char_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to long(s).
 */
dictionary *char_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to short(s).
 */
dictionary *char_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to unsigned char(s).
 */
dictionary *char_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to unsigned int(s).
 */
dictionary *char_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to unsigned long(s).
 */
dictionary *char_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps char(s) to unsigned short(s).
 */
dictionary *char_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to shallow(s).
 */
dictionary *double_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to string(s).
 */
dictionary *double_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to char(s).
 */
dictionary *double_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to double(s).
 */
dictionary *double_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to float(s).
 */
dictionary *double_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to int(s).
 */
dictionary *double_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to long(s).
 */
dictionary *double_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to short(s).
 */
dictionary *double_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to unsigned char(s).
 */
dictionary *double_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to unsigned int(s).
 */
dictionary *double_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to unsigned long(s).
 */
dictionary *double_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps double(s) to unsigned short(s).
 */
dictionary *double_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to shallow(s).
 */
dictionary *float_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to string(s).
 */
dictionary *float_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to char(s).
 */
dictionary *float_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to double(s).
 */
dictionary *float_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to float(s).
 */
dictionary *float_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to int(s).
 */
dictionary *float_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to long(s).
 */
dictionary *float_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to short(s).
 */
dictionary *float_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to unsigned char(s).
 */
dictionary *float_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to unsigned int(s).
 */
dictionary *float_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to unsigned long(s).
 */
dictionary *float_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps float(s) to unsigned short(s).
 */
dictionary *float_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to shallow(s).
 */
dictionary *int_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to string(s).
 */
dictionary *int_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to char(s).
 */
dictionary *int_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to double(s).
 */
dictionary *int_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to float(s).
 */
dictionary *int_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to int(s).
 */
dictionary *int_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to long(s).
 */
dictionary *int_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to short(s).
 */
dictionary *int_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to unsigned char(s).
 */
dictionary *int_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to unsigned int(s).
 */
dictionary *int_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to unsigned long(s).
 */
dictionary *int_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps int(s) to unsigned short(s).
 */
dictionary *int_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to shallow(s).
 */
dictionary *long_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to string(s).
 */
dictionary *long_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to char(s).
 */
dictionary *long_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to double(s).
 */
dictionary *long_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to float(s).
 */
dictionary *long_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to int(s).
 */
dictionary *long_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to long(s).
 */
dictionary *long_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to short(s).
 */
dictionary *long_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to unsigned char(s).
 */
dictionary *long_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to unsigned int(s).
 */
dictionary *long_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to unsigned long(s).
 */
dictionary *long_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps long(s) to unsigned short(s).
 */
dictionary *long_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to shallow(s).
 */
dictionary *short_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to string(s).
 */
dictionary *short_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to char(s).
 */
dictionary *short_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to double(s).
 */
dictionary *short_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to float(s).
 */
dictionary *short_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to int(s).
 */
dictionary *short_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to long(s).
 */
dictionary *short_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to short(s).
 */
dictionary *short_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to unsigned char(s).
 */
dictionary *short_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to unsigned int(s).
 */
dictionary *short_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to unsigned long(s).
 */
dictionary *short_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps short(s) to unsigned short(s).
 */
dictionary *short_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to shallow(s).
 */
dictionary *unsigned_char_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to string(s).
 */
dictionary *unsigned_char_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to char(s).
 */
dictionary *unsigned_char_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to double(s).
 */
dictionary *unsigned_char_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to float(s).
 */
dictionary *unsigned_char_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to int(s).
 */
dictionary *unsigned_char_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to long(s).
 */
dictionary *unsigned_char_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to short(s).
 */
dictionary *unsigned_char_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to unsigned char(s).
 */
dictionary *unsigned_char_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to unsigned int(s).
 */
dictionary *unsigned_char_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to unsigned long(s).
 */
dictionary *unsigned_char_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned char(s) to unsigned short(s).
 */
dictionary *unsigned_char_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to shallow(s).
 */
dictionary *unsigned_int_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to string(s).
 */
dictionary *unsigned_int_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to char(s).
 */
dictionary *unsigned_int_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to double(s).
 */
dictionary *unsigned_int_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to float(s).
 */
dictionary *unsigned_int_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to int(s).
 */
dictionary *unsigned_int_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to long(s).
 */
dictionary *unsigned_int_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to short(s).
 */
dictionary *unsigned_int_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to unsigned char(s).
 */
dictionary *unsigned_int_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to unsigned int(s).
 */
dictionary *unsigned_int_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to unsigned long(s).
 */
dictionary *unsigned_int_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned int(s) to unsigned short(s).
 */
dictionary *unsigned_int_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to shallow(s).
 */
dictionary *unsigned_long_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to string(s).
 */
dictionary *unsigned_long_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to char(s).
 */
dictionary *unsigned_long_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to double(s).
 */
dictionary *unsigned_long_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to float(s).
 */
dictionary *unsigned_long_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to int(s).
 */
dictionary *unsigned_long_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to long(s).
 */
dictionary *unsigned_long_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to short(s).
 */
dictionary *unsigned_long_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to unsigned char(s).
 */
dictionary *unsigned_long_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to unsigned int(s).
 */
dictionary *unsigned_long_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to unsigned long(s).
 */
dictionary *unsigned_long_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned long(s) to unsigned short(s).
 */
dictionary *unsigned_long_to_unsigned_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to shallow(s).
 */
dictionary *unsigned_short_to_shallow_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to string(s).
 */
dictionary *unsigned_short_to_string_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to char(s).
 */
dictionary *unsigned_short_to_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to double(s).
 */
dictionary *unsigned_short_to_double_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to float(s).
 */
dictionary *unsigned_short_to_float_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to int(s).
 */
dictionary *unsigned_short_to_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to long(s).
 */
dictionary *unsigned_short_to_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to short(s).
 */
dictionary *unsigned_short_to_short_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to unsigned char(s).
 */
dictionary *unsigned_short_to_unsigned_char_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to unsigned int(s).
 */
dictionary *unsigned_short_to_unsigned_int_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to unsigned long(s).
 */
dictionary *unsigned_short_to_unsigned_long_dictionary_create(void);

/**
 * Creates a dictionary which maps unsigned short(s) to unsigned short(s).
 */
dictionary *unsigned_short_to_unsigned_short_dictionary_create(void);
