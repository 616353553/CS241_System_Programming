/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#include <assert.h>

/**
 * 'INITIAL_CAPACITY' the initial size of the dynamically.
 */
const size_t INITIAL_CAPACITY = 8;
/**
 * 'GROWTH_FACTOR' is how much the vector will grow by in automatic reallocation
 * (2 means double).
 */
const size_t GROWTH_FACTOR = 2;

struct vector {
    /* The function callback for the user to define the way they want to copy
     * elements */
    copy_constructor_type copy_constructor;

    /* The function callback for the user to define the way they want to destroy
     * elements */
    destructor_type destructor;

    /* The function callback for the user to define the way they a default
     * element to be constructed */
    default_constructor_type default_constructor;

    /* Void pointer to the beginning of an array of void pointers to arbitrary
     * data. */
    void **array;

    /**
     * The number of elements in the vector.
     * This is the number of actual objects held in the vector,
     * which is not necessarily equal to its capacity.
     */
    size_t size;

    /**
     * The size of the storage space currently allocated for the vector,
     * expressed in terms of elements.
     */
    size_t capacity;
};

/**
 * IMPLEMENTATION DETAILS
 *
 * The following is documented only in the .c file of vector,
 * since it is implementation specfic and does not concern the user:
 *
 * This vector is defined by the struct above.
 * The struct is complete as is and does not need any modifications.
 *
 * The only conditions of automatic reallocation is that
 * they should happen logarithmically compared to the growth of the size of the
 * vector inorder to achieve amortized constant time complexity for appending to
 * the vector.
 *
 * For our implementation automatic reallocation happens when -and only when-
 * adding to the vector makes its new  size surpass its current vector capacity
 * OR when the user calls on vector_reserve().
 * When this happens the new capacity will be whatever power of the
 * 'GROWTH_FACTOR' greater than or equal to the target capacity.
 * In the case when the new size exceeds the current capacity the target
 * capacity is the new size.
 * In the case when the user calls vector_reserve(n) the target capacity is 'n'
 * itself.
 * We have provided get_new_capacity() to help make this less ambigious.
 */

static size_t get_new_capacity(size_t target) {
    /**
     * This function works according to 'automatic reallocation'.
     * Start at 1 and keep multiplying by the GROWTH_FACTOR untl
     * you have exceeded or met your target capacity.
     */
    size_t new_capacity = 1;
    while (new_capacity < target) {
        new_capacity *= GROWTH_FACTOR;
    }
    return new_capacity;
}

vector *vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor,
                      default_constructor_type default_constructor) {
    if (copy_constructor && destructor && default_constructor) {
        vector *v = malloc(sizeof(vector));
        v->copy_constructor = copy_constructor;
        v->destructor = destructor;
        v->default_constructor = default_constructor;
        v->array = malloc(sizeof(void *) * INITIAL_CAPACITY);
        v->size = 0;
        v->capacity = INITIAL_CAPACITY;
        return v;
    } else {
        copy_constructor_type cpy = copy_constructor ? copy_constructor: shallow_copy_constructor;
        destructor_type des = destructor ? destructor : shallow_destructor;
        default_constructor_type def = default_constructor ? default_constructor : shallow_default_constructor;
        return vector_create(cpy, des, def);
    }
}

void vector_destroy(vector *this) {
    assert(this);
    size_t index = 0;
    for ( ; index < this->size; ++index) {
        this->destructor(this->array[index]);
    }
    free(this->array);
    free(this);
}

void **vector_begin(vector *this) {
    return this->array;
}

void **vector_end(vector *this) {
    return this->array + this->size;
}

size_t vector_size(vector *this) {
    assert(this);
    return this->size;
}

void vector_resize(vector *this, size_t n) {
    assert(this);
    if (n > this->capacity) {
        this->capacity = get_new_capacity(n);
        this->array = realloc(this->array, this->capacity * sizeof(void *));
        for (size_t i = this->size; i < n; ++i){
            this->array[i] = this->default_constructor();
        }
    } else if (n > this->size) {
        for (size_t index = this->size; index < n; ++index) {
            this->array[index] = this->default_constructor();
        }
    } else {
        size_t curr_size = this->size;
        while (curr_size > n) {
            this->destructor(this->array[--curr_size]);
        }
    }
    this->size = n;
}

size_t vector_capacity(vector *this) {
    assert(this);
    return this->capacity;
}

bool vector_empty(vector *this) {
    assert(this);
    return this->size <= 0;
}

void vector_reserve(vector *this, size_t n) {
    assert(this);
    if (this->capacity < n) {
        this->capacity = get_new_capacity(n);
        this->array = realloc(this->array, this->capacity * sizeof(void *));
    }
}

void **vector_at(vector *this, size_t position) {
    assert(this);
    if (this->size > position) {
        return this->array + position;
    } else {
        assert(0);
    }
}

void vector_set(vector *this, size_t position, void *element) {
    assert(this);
    if (this->size > position) {
        this->destructor(this->array[position]);
        this->array[position] = this->copy_constructor(element);
    } else {
        assert(0);
    }
}

void *vector_get(vector *this, size_t position) {
    assert(this);
    if (this->size > position) {
        return this->array[position];
    } else {
        assert(0);
    }
}

void **vector_front(vector *this) {
    assert(this);
    return this->array;
}

void **vector_back(vector *this) {
    assert(this);
    return this->array + this->size - 1;
}

void vector_push_back(vector *this, void *element) {
    assert(this);
    if (this->size == this->capacity) {
        this->capacity = get_new_capacity(this->capacity + 1);
	this->array = realloc(this->array, this->capacity * sizeof(void *));
    }
    this->array[this->size] = this->copy_constructor(element);
    this->size++;
}

void vector_pop_back(vector *this) {
    assert(this);
    this->destructor(this->array[this->size - 1]);
    this->size--;
}

void vector_insert(vector *this, size_t position, void *element) {
    assert(this);
    if (this->size == this->capacity) {
        this->capacity = get_new_capacity(this->capacity + 1);
        this->array = realloc(this->array, this->capacity * sizeof(void *));
    }
    size_t index = this->size;
    for ( ; index > position; --index) {
        this->array[index] = this->array[index - 1];
    }
    this->array[position] = this->copy_constructor(element);
    this->size++;
}

void vector_erase(vector *this, size_t position) {
    assert(this);
    assert(position < vector_size(this));
    this->destructor(this->array[position]);
    size_t index = position;
    for ( ; index < this->size; ++index){
        this->array[index] = this->array[index + 1];
    }
    this->size--;
}

void vector_clear(vector *this) {
    size_t index = 0;
    for ( ; index < this->size; ++index){
        this->destructor(this->array[index]);
    }
    this->size = 0;
}

// The following is code generated:
vector *shallow_vector_create() {
    return vector_create(shallow_copy_constructor, shallow_destructor,
                         shallow_default_constructor);
}
vector *string_vector_create() {
    return vector_create(string_copy_constructor, string_destructor,
                         string_default_constructor);
}
vector *char_vector_create() {
    return vector_create(char_copy_constructor, char_destructor,
                         char_default_constructor);
}
vector *double_vector_create() {
    return vector_create(double_copy_constructor, double_destructor,
                         double_default_constructor);
}
vector *float_vector_create() {
    return vector_create(float_copy_constructor, float_destructor,
                         float_default_constructor);
}
vector *int_vector_create() {
    return vector_create(int_copy_constructor, int_destructor,
                         int_default_constructor);
}
vector *long_vector_create() {
    return vector_create(long_copy_constructor, long_destructor,
                         long_default_constructor);
}
vector *short_vector_create() {
    return vector_create(short_copy_constructor, short_destructor,
                         short_default_constructor);
}
vector *unsigned_char_vector_create() {
    return vector_create(unsigned_char_copy_constructor,
                         unsigned_char_destructor,
                         unsigned_char_default_constructor);
}
vector *unsigned_int_vector_create() {
    return vector_create(unsigned_int_copy_constructor, unsigned_int_destructor,
                         unsigned_int_default_constructor);
}
vector *unsigned_long_vector_create() {
    return vector_create(unsigned_long_copy_constructor,
                         unsigned_long_destructor,
                         unsigned_long_default_constructor);
}
vector *unsigned_short_vector_create() {
    return vector_create(unsigned_short_copy_constructor,
                         unsigned_short_destructor,
                         unsigned_short_default_constructor);
}
