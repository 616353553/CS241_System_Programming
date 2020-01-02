/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

// void print_arr(char **arr) {
//     if (arr == NULL) {
//         printf("%p\n", arr);
//         return;
//     }
//     char **curr = arr;
//     printf("{");
//     while(*curr) {
//         printf("\"%s\",\n", *curr);
//         ++curr;
//     }
//     printf("%s}\n", *curr);
// }


// static void print_result(char *str, char **result, char **target) {
//     printf("Original string:\n");
//     printf("\"%s\"\n\n", str);
//     printf("Result:\n");
//     print_arr(result);
//     printf("\n");
//     printf("Should be:\n");
//     print_arr(target);
//     printf("*********************************\n");
// }

static size_t arr_size(char **arr) {
    if (arr == NULL) {
        return 0;
    }
    size_t size = 0;
    char **curr = arr;
    while (*curr != NULL) {
        ++size;
        ++curr;
    }
    return size;
}

static int tester(char *str, char **target, char **(*camelCaser)(const char *), void (*destroy)(char **)) {
    char **result = camelCaser(str);
    size_t size_1 = arr_size(result);
    size_t size_2 = arr_size(target);
    if (size_1 != size_2) {
        //print_result(str, result, target);
        destroy(result);
        return 0;
    }
    for (size_t i = 0; i < size_1; ++i) {
        if (strcmp(result[i], target[i])){
            //print_result(str, result, target);
            destroy(result);
            return 0;
        }
    }
    //print_result(str, result, target);
    destroy(result);
    return 1;
}
/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *), void (*destroy)(char **)) {

    if (!tester(NULL, NULL, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("", 
                (char*[]){NULL}, camelCaser, destroy)) {
        return 0;
    }
    
    if (!tester("    ", 
                (char*[]){NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("THIS IS A SIMPLE TEST. should not fail this. ...", 
                (char*[]){"thisIsASimpleTest", "shouldNotFailThis", "", "", "", NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("04Try to \a use. SOME 3SCAPEs\b and? see What\fwould happen?\n 7his should\" work just\rfine: no matter\t how long\v. IT IS!", 
                (char*[]){"04tryTo\aUse", "some3Scapes\bAnd", "seeWhatWouldHappen", "7hisShould", "workJustFine", "noMatterHowLong", "itIs", NULL}, camelCaser, destroy)) {
        return 0;
    } 

    if (!tester("  \n Start wITh, 2SPaCes; and CHECK if IT SUccEED!", 
                (char*[]){"startWith", "2spaces", "andCheckIfItSucceed", NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("Try 2 NOT end with a punctuat1ON, start counting from 2 to 10: one, two, three, four", 
                (char*[]){"try2NotEndWithAPunctuat1on", "startCountingFrom2To10", "one", "two", "three", NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester(".......", 
                (char*[]){	"", "", "", "", "", "", "", NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("1 Like 3ggs, and   ? I also l1ke chicken :)", 
                (char*[]){"1Like3Ggs", "and", "iAlsoL1keChicken", "", NULL}, camelCaser, destroy)) {
        return 0;
    }

    if (!tester("hello\n\tthere, how are u doin' today\" my lil' homie?", 
                (char*[]){"helloThere", "howAreUDoin", "today", "myLil", "homie", NULL}, camelCaser, destroy)) {
        return 0;
    } 

    if (!tester(" \0None of these should appear.", 
                (char*[]){NULL}, camelCaser, destroy)) {
        return 0;
    }

    return 1;
}
