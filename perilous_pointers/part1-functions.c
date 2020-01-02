/**
* Pointers Gone Wild Lab
* CS 241 - Spring 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Checks to see if the input parameter is a passing grade and prints out
 * if the grade is passing using the provided format specifiers. Note that
 * 'grade' is a char pointer pointing to the beginning of a C string. A grade
 * is considered passing iff the numeric interpretation (defined by strtof) is
 * above a 70. Suppose
 * that 'grade' pointed to the string "73.58", then 'grade' would be passing in
 * this example. If 'grade' pointed to "it's over 9000!!!!", then that is still
 * not
 * a passing grade.
 *
 * @param grade
 *     The grade to check.
 */
void one(const char *grade) {
    float grade_num = atof(grade);
    if (grade_num > 70.0)
        printf("%f passed!\n", grade_num);
    else
        printf("%s not passed!\n", grade);
}

/**
 * Have the integer pointer p point to the integer x which is on the stack.
 * Then print the value that 'p' points to and format it as an integer
 */
void two() {
    int x = 4;
    int *p = &x;

    printf("The value of p is: %d\n", *p);
}

/**
 * Prints "x and y are equal." iff the values x and y point to are equal
 * where equality is defined by integer equality.
 * Else print "x and y are different".
 *
 * @param x
 *     First input parameter.
 *
 * @param y
 *     Second input parameter.
 */
void three(const int *x, const int *y) {
    if (*x == *y)
        printf("x and y are equal.\n");
    else
        printf("x and y are different.\n");
}

/**
 * Returns a pointer to a float that points to memory on the heap,
 * which contains a copy of the value that the integer pointer 'x' is pointing
 * to.
 *
 * @param x
 *     Input parameter, whose value will be returned as a (float *).
 *
 * @returns
 *     A new pointer, allocated on the heap and not freed, that
 *     contains the value of the input parameter.
 */
float *four(const int *x) {
    float *p = malloc(sizeof(float));
    *p = (float)*x;
    return p;
}

/**
 * Takes a char pointer 'a' and checks to see if the first char
 * that it points to is alphabetic (upper or lower case).
 *
 * @param a
 *     Input parameter a, which is a char*
 *
 */
void five(const char *a) {
    if ((*a >= 'A' && *a <= 'Z') || (*a >= 'a' && *a <= 'z'))
        printf("a is a letter.\n");
    else
        printf("a is not a letter.\n");
}

/**
 * Concatenates "Hello " and the parameter str, which is guaranteed to be a
 * valid c string, and
 * prints the concatenated string.
 */
void six(const char *str) {
    char *s = malloc(7 + strlen(str));
    strcpy(s, "Hello ");
    strcat(s, str);
    printf("%s\n", s);
    free(s);
}

/**
 * Creates an array of values containing the values {0.0, 0.1, ..., 0.9}.
 */
void seven() {
    float *values = malloc(sizeof(float) * 10);

    int i, n = 10;
    for (i = 0; i < n; i++)
        values[i] = (float)i / n;

    for (i = 0; i < n; i++)
        printf("%f ", values[i]);
    printf("\n");
}

/**
 * Creates a 2D array of values and prints out the values on the diagonal.
 */
void eight(int a) {
    int **values = malloc(sizeof(int *) * 10);
    int i, j;
    for (i = 0; i < 10; i++) {
        values[i] = malloc(sizeof(int) * 10);
        for (j = 0; j < 10; j++)
            values[i][j] = i * j * a;
    }
    for (i = 0; i < 10; i++)
        printf("%d ", values[i][i]);
    printf("\n");
    for (i = 0; i < 10; i++)
        free(values[i]);
    free(values);
}

/**
 * If s points to the string "blue", then print "Orange and BLUE!". If s points
 * to the string
 * "orange", then print "ORANGE and blue!" else just print "orange and blue!".
 * Use strcmp() and
 * friends to test for string equality.
 *
 * @param s
 *     Input parameter, used to determine which string is printed.
 */
void nine(const char *s) {
    if (strcmp(s, "blue") == 0) {
        printf("Orange and BLUE!\n");
    } else if (strcmp(s, "orange") == 0) {
        printf("ORANGE and blue!\n");
    } else {
        printf("orange and blue!\n");
    }
}

/**
 * Prints out the radius of a circle, given its diameter.
 *
 * @param d
 *     The diameter of the circle.
 */
void ten(const int d) {
    printf("The radius of the circle is: %f.\n", d / 2.0);
}

/**
 * @brief
 *     Clears the bits in "value" that are set in "flag".
 *
 * This function will apply the following rules to the number stored
 * in the input parameter "value":
 * (1): If a specific bit is set in BOTH "value" and "flag", that
 *      bit is NOT SET in the result.
 * (2): If a specific bit is set ONLY in "value", that bit IS SET
 *      in the result.
 * (3): All other bits are NOT SET in the result.
 *
 * Examples:
 *    clear_bits(value = 0xFF, flag = 0x55): 0xAA
 *    clear_bits(value = 0x00, flag = 0xF0): 0x00
 *    clear_bits(value = 0xAB, flag = 0x00): 0xAB
 *
 * @param value
 *     The numeric value to manipulate.
 *
 * @param flag
 *     The flag (or mask) used in order to clear bits from "value".
 */
long int clear_bits(long int value, long int flag) {
    return ~flag & value;
}

/**
 * @brief
 *     A little finite automaton in C.
 *
 * This function will use a provided transition function to simulate a DFA over
 * an input string. The function returns the final state.
 *
 * The DFA starts in state 0. For each character in input_string, call the
 * transition function with the current state and the current character. The
 * current state changes to the state the transition function returns.
 *
 * So, in pseudo code:
 *
 * state = 0
 * for character in input_string:
 *     state = transition_function(state, character)
 *
 * NOTE: the transition_function parameter is a "function pointer." For more
 * information about these fun creatures, see this:
 * http://www.cprogramming.com/tutorial/function-pointers.html
 *
 * @param transition - function pointer to a transition function to use
 * @param input_string - string to run the automaton over
 *
 * @return the final state
 */
int little_automaton(int (*transition)(int, char), const char *input_string) {
    int state = 0;
    while(*input_string) {
        state = (*transition)(state, *input_string);
        input_string++;
    }
    return state;
}
