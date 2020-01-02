/**
* Pointers Gone Wild Lab
* CS 241 - Spring 2018
*/

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // 1
    first_step(81);

    // 2
    int num = 132;
    second_step(&num);

    // 3
    num = 8942;
    int * ptr = &num;
    double_step(&ptr);

    // 4
    char c_arr[] = {0, 0, 0, 0, 0, 15, 0, 0, 0};
    strange_step(c_arr);

    // 5
    char* str = "Hi!";
    empty_step((void *)str);

    // 6
    str = "gulu";
    two_step((void *)str, str);

    // 7
    str = "how are you doing?";
    three_step(str, str + 2, str + 4);

    // 8
    str = "aaaqi";
    step_step_step(str, str + 2, str);

    // 9
    str = "a";
    it_may_be_odd(str, 'a');

    // 10
    char sentence[] = "best,CS241,ever";
    tok_step(sentence);

    // 11
    char temp[5];
    temp[0] = 1;
    temp[1] = 1;
    temp[2] = 1;
    temp[3] = 3;
    temp[4] = '\0';
    the_end((void *)temp, (void *)temp);
    return 0;
}
