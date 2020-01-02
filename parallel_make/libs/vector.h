/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once
#include "callbacks.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * A C style implementation of std::vector
 (http://www.cplusplus.com/reference/vector/vector/)
 *
 * vectors are sequence containers representing arrays that can change in size.
 *
 * Just like arrays, vectors use contiguous storage locations for their
 elements,
 * which means that their elements can also be accessed using offsets on regular
 pointers to its elements,
 * and just as efficiently as in arrays.
 * But unlike arrays, their size can change dynamically, with their storage
 being handled automatically by the container.
 *
 * Internally, vectors use a dynamically allocated array to store their
 elements.
 * This array may need to be reallocated in order to grow in size when new
 elements are inserted,
 * which implies allocating a new array and moving all elements to it.
 * This is a relatively expensive task in terms of processing time, and thus,
 * vectors do not reallocate each time an element is added to the container.
 *
 * Instead, vector containers may allocate some extra storage to accommodate for
 possible growth,
 * and thus the container may have an actual capacity greater than the storage
 strictly needed to contain its elements (i.e., its size).
 * Libraries can implement different strategies for growth to balance between
 memory usage and reallocations,
 * but in any case, reallocations should only happen at logarithmically growing
 intervals of size
 * so that the insertion of individual elements at the end of the vector can be
 provided with amortized constant time complexity (see push_back).

 * Therefore, compared to arrays, vectors consume more memory in exchange for
 the ability to manage storage and grow dynamically in an efficient way.

 * Compared to the other dynamic sequence containers (deques, lists and
 forward_lists),
 * vectors are very efficient accessing its elements (just like arrays) and
 relatively efficient adding or removing elements from its end.
 * For operations that involve inserting or removing elements at positions other
 than the end, they perform worse than the others,
 * and have less consistent iterators and references than lists and
 forward_lists.
 */

/* Forward declare vector structure. */
typedef struct vector vector;

/**
 * 'INITIAL_CAPACITY' is the macro that will be used to set the capacity of the
 * vector in vector_create().
 */
#define INITIAL_CAPACITY 8
/**
 * 'GROWTH_FACTOR' is how much the vector will grow by in automatic reallocation
 * (2 means double).
 */
#define GROWTH_FACTOR 2

// Member Functions:

/**
 * Allocate and return a pointer to a new vector (on the heap).
 *
 * If you would like to make 'shallow' copies of the elements of the 'vector',
 * then you may pass in NULL for the parameters (ex. vector_create(NULL, NULL,
 * NULL)).
 * Hint: Look at callbacks.h.
 * This means that everytime an element is to be copied or removed from the
 * 'vector'
 * the pointer to that element is copied or removed
 * instead of using the user supplied copy constructor and destructor.
 * This also means means the default constructor will return a NULL pointer.
 */
vector *vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor,
                      default_constructor_type default_constructor);

/**
 * Destroys all container elements by
 * calling on the user provided destructor for every element,
 * and deallocates all the storage capacity allocated by the 'vector'.
*/
void vector_destroy(vector *this);

// Iterators

/**
 * Returns a pointer to the first element in the vector.
 *
 * If the container is empty, the returned iterator value shall not be
 * dereferenced.
 *
 * http://www.cplusplus.com/reference/vector/vector/begin/
 */
void **vector_begin(vector *this);

/**
 * Returns an iterator referring to the past-the-end element in the vector
 * container.
 *
 * The past-the-end element is the theoretical element that would follow the
 * last element in the vector.
 * It does not point to any element, and thus shall not be dereferenced.
 *
 * If the container is empty, this function returns the same as vector_begin()
 *
 * http://www.cplusplus.com/reference/vector/vector/end/
 */
void **vector_end(vector *this);

// Capacity:

/**
 * Returns the number of elements in the 'vector'.
 * This is the number of actual objects held in the 'vector',
 * which is not necessarily equal to its storage capacity.
 *
 * http://www.cplusplus.com/reference/vector/vector/size/
 */
size_t vector_size(vector *this);

/**
 * Resizes the container so that it contains 'n' elements.
 *
 * If 'n' is smaller than the current container size,
 * the content is reduced to its first n elements,
 * removing those beyond (and destroying them).
 *
 * If 'n' is greater than the current container size,
 * the content is expanded by inserting at the end as many elements as needed to
 * reach a size of n.
 * These new elements are created using the user defined default-constructor.
 *
 * If 'n' is also greater than the current container capacity,
 * an automatic reallocation of the allocated storage space takes place.
 *
 * Notice that this function changes the actual content of the container by
 * inserting or erasing elements from it.
 *
 * http://www.cplusplus.com/reference/vector/vector/resize/
 */
void vector_resize(vector *this, size_t n);

/**
 * Returns the size of the storage space currently allocated for the 'vector',
 * expressed in terms of elements.
 *
 * This capacity is not necessarily equal to the 'vector' size.
 * It can be equal or greater, with the extra space allowing to accommodate for
 * growth without the need to reallocate on each insertion.
 *
 * Notice that this capacity does not suppose a limit on the size of the
 * 'vector'.
 * When this capacity is exhausted and more is needed,
 * it is automatically expanded by the container (reallocating it storage
 * space).
 *
 * The capacity of a vector can be explicitly altered by calling
 * vector_reserve().
 *
 * http://www.cplusplus.com/reference/vector/vector/capacity/
 */
size_t vector_capacity(vector *this);

/**
 * Returns whether the vector is empty (i.e. whether its size is 0).
 *
 * This function does not modify the container in any way.
 *
 * http://www.cplusplus.com/reference/vector/vector/empty/
 */
bool vector_empty(vector *this);

/**
 * Requests that the vector capacity be at least enough to contain 'n' elements.
 *
 * If 'n' is greater than the current vector capacity,
 * the function causes the container to reallocate its storage increasing
 * its capacity to 'n' (or greater).

 * In all other cases,
 * the function call does not cause a reallocation and the vector capacity is
 not affected.

 * This function has no effect on the vector size and cannot alter its elements.
 *
 * http://www.cplusplus.com/reference/vector/vector/reserve/
 */
void vector_reserve(vector *this, size_t n);

// Element access:

/**
 * Returns a reference to the element at position 'n' in the vector.
 *
 * The function automatically checks whether 'n' is within the bounds of valid
 * elements in the vector,
 * throwing an assertion (assert.h) if it is not (i.e., if n is greater than, or
 * equal to, its size).
 *
 * http://www.cplusplus.com/reference/vector/vector/at/
 */
void **vector_at(vector *this, size_t n);

/**
 * Sets the 'element' at position 'n' in the 'vector'.
 *
 * Note: That this will destroy whatever is already at position 'n'.
*/
void vector_set(vector *this, size_t n, void *element);

/**
 * Gets the 'element' at position 'n' in the 'vector'.
*/
void *vector_get(vector *this, size_t n);

/**
 * Returns a pointer to the first element in the vector.
 *
 * Calling this function on an empty container causes undefined behavior.
 *
 * http://www.cplusplus.com/reference/vector/vector/front/
 */
void **vector_front(vector *this);

/**
 * Returns a pointer to the last element in the vector.
 *
 * Calling this function on an empty container causes undefined behavior.
 *
 * http://www.cplusplus.com/reference/vector/vector/back/
 */
void **vector_back(vector *this);

// Modifiers:

/**
 * Adds a new 'element' at the end of the vector, after its current last
 * element.
 * This effectively increases the container size by one,
 * which causes an automatic reallocation of the allocated storage space if -and
 * only if-
 * the new vector size surpasses the current vector capacity.
 *
 * http://www.cplusplus.com/reference/vector/vector/push_back/
 */
void vector_push_back(vector *this, void *element);

/**
 * Removes the last element in the vector, effectively reducing the container
 * size by one.
 *
 * This destroys the removed element.
 *
 * http://www.cplusplus.com/reference/vector/vector/pop_back/
 */
void vector_pop_back(vector *this);

/**
 * The 'vector' is extended by inserting a new 'element' before the element at
 * the specified 'position',
 * effectively increasing the container size by 1.
 *
 * This causes an automatic reallocation of the allocated storage space if -and
 * only if-
 * the new 'vector' 'size' surpasses the current 'vector' 'capacity'.
 *
 * Because vectors use an array as their underlying storage,
 * inserting elements in positions other than the vector end
 * causes the container to relocate all the elements that were after position to
 * their new positions.
 * This is generally an inefficient operation compared to
 * the one performed for the same operation by other kinds of sequence
 * containers (such as list or forward_list).
 *
 * http://www.cplusplus.com/reference/vector/vector/insert/
 */
void vector_insert(vector *this, size_t position, void *element);

/**
 * Removes from the vector a single element at the specified 'position'.
 * This effectively reduces the container size by 1 and destroys an element.
 *
 * Because vectors use an array as their underlying storage,
 * erasing elements in positions other than the vector end
 * causes the container to relocate all the elements after the segment erased to
 * their new positions.
 * This is generally an inefficient operation compared to
 * the one performed for the same operation by other kinds of sequence
 * containers (such as list or forward_list).
 *
 * http://www.cplusplus.com/reference/vector/vector/erase/
 */
void vector_erase(vector *this, size_t position);

/**
 * Removes all elements from the vector (which are destroyed),
 * leaving the container with a size of 0.
 *
 * A reallocation is not guaranteed to happen,
 * and the vector capacity is not guaranteed to change due to calling this
 * function.
 *
 * http://www.cplusplus.com/reference/vector/vector/clear/
 */
void vector_clear(vector *this);

/**
 * Vector iteration macro. `vecname` is the name of the vector. `varname` is
 * the name of a temporary (local) variable to refer to each element in the
 * vector, and `callback` is a block of code that gets executed once for
 * each element in the vector until `break;` is called.
 *
 * Example usage:
 * ```
 *     vector *v = string_vector_create();
 *     vector_push_back(v, "Hi!"); vector_push_back(v, "I'm");
 *     vector_push_back(v, "Vector"); vector_push_back(v, "\n");
 *     VECTOR_FOR_EACH(v, thing, {
 *       fprintf("%s\n", (char *) thing);
 *       if (!strcmp((char *) thing, "Set")){
 *         break;
 *       }
 *     });
 * ```
 */
#define VECTOR_FOR_EACH(vecname, varname, callback) \
    do {                                            \
        void **_it = vector_begin(vecname);         \
        void **_iend = vector_end(vecname);         \
        for (; _it != _iend; ++_it) {               \
            void *varname = *_it;                   \
            { callback; }                           \
        }                                           \
    } while (0)

// The following is code generated:

/**
 * Creates a vector that performs no memory management and uses only shallow
 * callback functions.
 */
vector *shallow_vector_create(void);

/**
 * Creates a vector meant for string(s).
 */
vector *string_vector_create(void);

/**
 * Creates a vector meant for char(s).
 */
vector *char_vector_create(void);

/**
 * Creates a vector meant for double(s).
 */
vector *double_vector_create(void);

/**
 * Creates a vector meant for float(s).
 */
vector *float_vector_create(void);

/**
 * Creates a vector meant for int(s).
 */
vector *int_vector_create(void);

/**
 * Creates a vector meant for long(s).
 */
vector *long_vector_create(void);

/**
 * Creates a vector meant for short(s).
 */
vector *short_vector_create(void);

/**
 * Creates a vector meant for unsigned char(s).
 */
vector *unsigned_char_vector_create(void);

/**
 * Creates a vector meant for unsigned int(s).
 */
vector *unsigned_int_vector_create(void);

/**
 * Creates a vector meant for unsigned long(s).
 */
vector *unsigned_long_vector_create(void);

/**
 * Creates a vector meant for unsigned short(s).
 */
vector *unsigned_short_vector_create(void);
