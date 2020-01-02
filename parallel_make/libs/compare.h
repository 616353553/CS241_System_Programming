/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once
#include <stdbool.h>
#include <string.h>

/**
 * The concept Compare is a set of requirements expected by some of the
 * standard library facilities from the user-provided function object types.
 *
 * The return value of the function call operation applied to an object of type
 * Compare, when contextually converted to bool, yields true if the first
 * argument of the call appears before the second in the strict weak ordering
 * relation induced by this Compare type, and false otherwise.
 *
 * Requirements:
 *
 * equiv(a, b), an expression equivalent to !comp(a, b) && !comp(b, a)
 *
 * comp(a, b) - implicitly convertible to bool
 *  Establishes strict weak ordering relation with the following properties:
 *    For all a, comp(a,a)==false
 *    If comp(a,b)==true then comp(b,a)==false
 *    If comp(a,b)==true and comp(b,c)==true then comp(a,c)==true
 *
 * equiv(a, b) - bool
 *  Establishes equivalence relationship with the following properties:
 *    For all a, equiv(a,a)==true
 *    If equiv(a,b)==true, then equiv(b,a)==true
 *    If equiv(a,b)==true and equiv(b,c)==true, then equiv(a,c)==true
 *
 * http://en.cppreference.com/w/cpp/concept/Compare
 */

/**
 * typedef for a compare type.
 *
 * Returns:
 *  a negative value if 'a' comes before 'b'
 *  a positive value if 'b' comes before 'a'
 *  0 in all other cases
 */
typedef int (*compare)(void *a, void *b);

/**
 * A compare callback function that orders 'a' and 'b' with
 * with their virtual addresses.
 *
 * Returns:
 *  a negative value if 'a' has a lower virtual address than 'b''s virtual
 *  address
 *  a negative value if 'b' has a lower virtual address than 'a''s virtual
 *  address
 *  0 if 'a' has the same virtual address as 'b'
 */
int shallow_compare(void *a, void *b);

/**
 * Takes the compare type 'comp' and returns equiv('a', 'b')
 */
bool compare_equiv(compare comp, void *a, void *b);

/**
 * string compare function: a NULL-safe wrapper for strcmp(). NULL is always
 * evaluated as less than non-NULL.
 */
int string_compare(void *a, void *b);

// Code generated:

/**
 * char compare function.
 */
int char_compare(void *a, void *b);

/**
 * double compare function.
 */
int double_compare(void *a, void *b);

/**
 * float compare function.
 */
int float_compare(void *a, void *b);

/**
 * int compare function.
 */
int int_compare(void *a, void *b);

/**
 * long compare function.
 */
int long_compare(void *a, void *b);

/**
 * short compare function.
 */
int short_compare(void *a, void *b);

/**
 * unsigned char compare function.
 */
int unsigned_char_compare(void *a, void *b);

/**
 * unsigned int compare function.
 */
int unsigned_int_compare(void *a, void *b);

/**
 * unsigned long compare function.
 */
int unsigned_long_compare(void *a, void *b);

/**
 * unsigned short compare function.
 */
int unsigned_short_compare(void *a, void *b);
