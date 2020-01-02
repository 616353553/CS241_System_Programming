/**
* Shell Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#pragma once
/**
 * C is notorious for having relatively annoying string manipulation interfaces
 * as compared to other languages such as C++ and python. In this MP, we're
 * going to define and implement sstring, a wrapper around C strings that
 * provides a higher level interface to dealing with strings. You will actually
 * define the implementation of `sstring` in sstring.c. Since the struct
 * definition is only present in the `.c` file, that definition is essentially
 * 'private' and as a result, outside of the implementing `.c` file, the
 * internal members of sstring cannot be accessed. As a consequence, an
 * `sstring` can only be declared on the heap (at least, not without some
 * non-trivial refactoring).
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Forward declare the structure */
typedef struct sstring sstring;

/**
 * Construct a sstring from a c string. This function will allocate any memory
 * requried to create a sstring. On the heap.
 */
sstring *cstr_to_sstring(const char *input);

/**
 * Return a c-string from an sstring. This function will allocate the memory to
 * store the returned c string on the heap.
 */
char *sstring_to_cstr(sstring *this);

/**
 * This function takes in two sstrings and appends the contents of the second
 * to the first.
 * Example:
 *
 * sstring *str1 = cstr_to_sstring("abc");
 * sstring *str2 = sstring_to_cstr("def");
 * sstring_append(str1, str2);
 * sstring_to_cstr(str1); // == "abcdef"
 */
int sstring_append(sstring *this, sstring *addition);

/**
 * Takes in an sstring and splits the string into a vector of c-strings on a
 * given delimiter. This should be analogous to python3's split function on
 * strings. You can check what the output should be for a given source string,
 * INPUT and a delimiter D, by running `python3 -c 'print("INPUT".split('D'))'`
 * in a shell.
 *
 * Example:
 *
 * sstring_split(cstr_to_sstring("This is a sentence."), ' ');
 * // == [ "This", "is", "a", "sentence." ]
 */
vector *sstring_split(sstring *this, char delimiter);

/**
 * Replaces the first instance of `target` after `offset` bytes of data with
 * `substitution`. You can assume that `offset` is always less than the length
 * of the data in `this`.
 *
 * Example:
 *
 * sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
 * sstring_substitute(replace_me, 18, "{}", "friend");
 * sstring_to_cstr(replace_me); // == "This is a {} day, friend!"
 * sstring_substitute(replace_me, 0, "{}", "good");
 * sstring_to_cstr(replace_me); // == "This is a good day, friend!"
 */
int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution);

/**
 * Return a c-string representing the bytes between the starting and ending
 * indicies. You can assume that start and end are always positive and satisfy
 * the following rule:
 *  0 <= start <= end <= number of bytes held by `this`
 *
 *
 * Example:
 * sstring *slice_me = cstr_to_sstring("1234567890");
 * sstring_slice(slice_me, 2, 5);
 * // == "345" (don't forget about the null byte!)
 */
char *sstring_slice(sstring *this, int start, int end);

/**
 * Destroys this sstring object, freeing all of its dynamic memory. `this`
 * shall be a valid sstring allocated on the heap.
 */
void sstring_destroy(sstring *this);
