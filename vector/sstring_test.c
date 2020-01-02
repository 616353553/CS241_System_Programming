/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"
#include "string.h"
#include "vector.h"

int main(int argc, char *argv[]) {
    sstring *str = cstr_to_sstring("0123456789abcdefghijklmnopqrstuvwxyz");
    printf("old string is: %s\n", "0123456789abcdefghijklmnopqrstuvwxyz");
    char *new_str = sstring_slice(str, atoi(argv[1]), atoi(argv[2]));
    printf("new string is: %s\n", new_str);
    free(new_str);
    sstring_destroy(str);
    return 0;
}
