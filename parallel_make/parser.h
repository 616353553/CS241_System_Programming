/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once

#include "graph.h"
#include "rule.h"

/**
 * Parses the file with path given by makeFileName using a syntax very similar
 * to the syntax of GNU Make.
 *
 * You are not required to handle any Makefile syntax which this parser does
 * not recognize. This includes Makefile macros.
 *
 * If the parser reads invalid inputs, it will exit the program with a status
 * of 2 after printing an error message in the style of GNU Make.
 *
 *
 * The parser will return a graph whose vertices represent all the rules
 * contained in the makefile, as well as a sentinel vertex with an empty
 * target ("") whose neighbors are the goals specified by the `targets`
 * argument. That is to say, `graph_neighbors(dep_graph, "")` will return a
 * vector containing the strings found in `targets`.
 *
 * The edges in this graph represent dependencies between rules:
 * `graph_adjacent(dep_graph, "tgt1", "tgt2")` implies that "tgt1" depends on
 * "tgt2". To find all neighbors of a vertex in a vector, use the
 * `graph_neighbors(dep_graph, "tgt")` method.
 *
 * For example, if a Makefile contained 3 targets:
 *
 * a: c b
 *
 * b:
 *
 * c:
 *
 * Calling parser_parse_makefile with a target list containing only "a" would
 * result in a graph whose adjacency list looks like this:
 *
 * ''  : {'a'}
 * 'a' : {'c', 'b'}
 * 'b' : {}
 * 'c' : {}
 *
 * Calling parser_parse_makefile with the target list {"c", "a", "b"} would
 * result in the following graph:
 *
 * ''  : {'c', 'a', 'b'}
 * 'a' : {'c', 'b'}
 * 'b' : {}
 * 'c' : {}
 *
 * If `targets` is empty (eg. targets == NULL), the first target in the file
 * will be used as the only goals (This is consistent with GNU Make).
 * So in this case:
 *
 * ''  : {'a'}
 * 'a' : {'c', 'b'}
 * 'b' : {}
 * 'c' : {}
 *
 * The graph's vertex set is effectively a dictionary that maps targets to
 * to `rule_t` structs. So to access a rule given a target, use
 * `rule_t *rule = (rule_t *)graph_get_vertex_value(dep_graph, "tgt")`. Refer
 * to the rule.h header and the rule.c source file to understand the details
 * of the `rule_t` type.
 *
 * NOTES ON USING THE GRAPH / MEMORY MANAGEMENT / ETC:
 *
 * THE GRAPH IS NOT THREAD SAFE! Do not concurrently modify this graph without
 * enforcing mutual exclusion! In particular, the vectors returned by this
 * graph are invalidated when vertices are removed or added.
 *
 * Vectors: You must call vector_destroy() on every vector returned by this
 * graph to prevent a memory leak. This will not affect the original graph in
 * any way.
 *
 * Vertices: NOTE THAT s, s1, and s2 refer to STRINGS and not rule_t types.
 * Adding a string s with `graph_add_vertex(dep_graph, s)`
 * will create a deep copy of s and initialize the associated value
 * to NULL. Adding a string-rule vertex pair is done through two string
 * arguments. i.e. `graph_set_vertex_value(dep_graph, s1, s2)` will make
 * a deep copy of s1 as a key and construct as a value a `rule_t` on the heap
 * whose target is initialized to a deep copy of s2. Removing a vertex will
 * destroy the associated string key and `rule_t` struct (see rule.h for more
 * info). Destroying the graph will destroy all vertices and the memory
 * associated with their keys and values.
 *
 * Edges: Edge values are not automatically managed by the graph and are
 * initialized to cast (i.e. NOT malloc()ed) integers by default. If you
 * assign other values to the edges, you must manually destroy them before
 * destroying edges to prevent leaks.
 * WARNING: if an edge originates from or terminates at
 * a vertex, removing the vertex will also destroy the edge, resulting in a
 * memory leak if you had assigned dynamic memory to that edge.
 *
 *
 * @param makeFileName Path to valid makefile, or NULL if none available.
 * @param run_targets Null-terminated list of targets as listed on
 * command-line.
 *
 * @return dependency graph representing entire makefile
 */
graph *parser_parse_makefile(const char *makeFileName, char **targets);
