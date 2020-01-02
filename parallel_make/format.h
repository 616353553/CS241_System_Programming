/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once

/**
 * Prints a failure message stating that 'target' cannot be
 * built because it has cyclical dependencies. This function should
 * only be called on goals, i.e. the positional arguments to make
 * (or the very first target in the makefile if none are specified).
 *
 * Example Makefile:
 *   a: b
 *   b: c
 *   c: a
 *
 * $make
 * > print_cycle_failure("a");
 * $make b
 * > print_cycle_failure("b");
 * $make a b c
 * > print_cycle_failure("a");
 * > print_cycle_failure("b");
 * > print_cycle_failure("c");
 */
void print_cycle_failure(char *target);
