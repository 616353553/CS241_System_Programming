/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#include <stdio.h>

void print_vector(char *, vector *);

int main(int argc, char *argv[]) {
    // create vector

    vector *v = vector_create(string_copy_constructor, string_destructor, string_default_constructor);

    print_vector("resize empty vector to 0:", v);

    vector_push_back(v, "elem0");    
    vector_push_back(v, "elem1");
    vector_push_back(v, "elem2");
    vector_push_back(v, "elem3");
    vector_push_back(v, "elem4");
    vector_push_back(v, "elem5");
    vector_push_back(v, "elem6");
    vector_push_back(v, "elem7");
    vector_push_back(v, "elem8");
    vector_push_back(v, "elem9");
    vector_push_back(v, "elem10");
    vector_push_back(v, "elem11");
    vector_push_back(v, "elem12");
    print_vector("Before modify:", v);

    vector_resize(v, 0);
    print_vector("after resize to 0", v);

    // vector_resize(v, 11);
    // print_vector("after resize to 11:", v);

    // vector_resize(v, 20);
    // print_vector("after resize to 20", v);

    // vector_insert(v, 2, "NEW ELEMENT 2");
    // print_vector("after insert at 2:", v);

    // vector_set(v, 4, "elem four");
    // print_vector("after set at 4:", v);

    // vector_insert(v, vector_size(v), "elem7");
    // print_vector("Edge case-insert at last index:", v);

    // vector_erase(v, 0);
    // print_vector("after erase at 0:", v);

    // vector_resize(v, 16);
    // print_vector("after resize to 16:", v);

    // vector_resize(v, 7);
    // print_vector("after resize to 7:", v);

    // vector_resize(v, 17);
    // print_vector("after resize to 17:", v);

    // vector_reserve(v, 5);
    // print_vector("after reserve size 5:", v);

    // vector_reserve(v, 17);
    // print_vector("after reserve size 17:", v);

    // vector_reserve(v, 33);
    // print_vector("after reserve size 33:", v);

    printf("begin address: %p\n", vector_begin(v));
    printf("end address: %p\n", vector_end(v));
    printf("front address: %p\n", vector_front(v));
    printf("back address: %p\n", vector_back(v));

    vector_destroy(v);
    return 0;
}


void print_vector(char* description, vector *v){
    puts(description);
    char **begin = (char **)vector_begin(v);
    char **end = (char **)vector_end(v);
    while (begin < end) {
        printf("v[%ld]: %s\n", begin - (char **)vector_begin(v), *begin);
        begin++;
    }
    printf("size of vector: %zu\n", vector_size(v));
    printf("capacity of vector: %zu\n", vector_capacity(v));
    printf("vector is empty: %d\n", vector_empty(v));
    puts("*****************************");
}
