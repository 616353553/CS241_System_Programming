/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#pragma once
#include "callbacks.h"
#include "compare.h"
#include "dictionary.h"
#include "vector.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * A graph is a set of vertices and a set of edges.
 *
 * This library implements a graph by storing a dictionary of vertices and
 * an adjacency list for each vertex (composed of a set and dictionary for
 * O(1) operations)
 *
 * Every vertex in the graph needs to have a key for the dictionary.
 * Every edge is directed and uses a (source, dest) pair as the key.
 *
 * This graph is internally implemented using a dictionary of key-vertex
 * pairs, where each vertex itself is implemented using a dictionary of
 * key-value pairs. Therefore, it share similar characteristics to the
 * dictionary, such as FIFO key iteration.
 */

/* Forward declare graph structure. */
typedef struct graph graph;

// Member Functions:

/**
 * Allocate and return a pointer to a new graph (on the heap).
 *
 * If you would like to make the graph store 'shallow' copies of the elements
 * passed in as keys and values thereto, then you may pass in NULL for all
 * the parameters (or use shallow_graph_create()).
 *
 * (ex.
 *  graph_create(hash_function, compare, NULL, NULL, NULL, NULL, NULL, NULL)).
 *
 * This means that every time an element is to be added to or removed from the
 * graph, the pointer to that element is copied or removed instead of using
 * a user supplied copy constructor and destructor.
 *
 * If you supply NULL for the hash_function then all elements will hash to the
 * same index causing most operations to require a linear scan of the graph.
 *
 * If you supply NULL for the compare then elements will be compared by their
 * virtual address (pointer value).
 */
graph *graph_create(hash_function_type vertex_hash_function,
                    compare vertex_comp,

                    copy_constructor_type vertex_key_copy_constructor,
                    destructor_type vertex_key_destructor,

                    copy_constructor_type vertex_value_copy_constructor,
                    destructor_type vertex_value_destructor,

                    copy_constructor_type edge_value_copy_constructor,
                    destructor_type edge_value_destructor);

/**
 * Destroys all container elements by calling on the user provided destructor
 * for every element, and deallocates all the storage capacity allocated by the
 * graph.
 * Complexity: O(V+E)
 */
void graph_destroy(graph *this);

// Graph Connectivity Functions:

/**
 * Returns whether there exists directed edge ('s', 't') in 'this'.
 *
 * Note that 's' and 't' must represent valid vertices, or this method will
 * result in undefined behavior.
 * Complexity: O(1)
 */
bool graph_adjacent(graph *this, void *s, void *t);

/**
 * Returns a vector containing all vertex keys 't' in 'this' such that 't' is
 * adjacent to 's', i.e. there exists edge ('s', 't') in 'this'. These keys
 * (but not the vector itself) represent shallow copies of data present
 * in the graph and therefore should not be destroyed.
 *
 * Note that this method returns neighbors in a FIFO order, based on which
 * neighbors were last assigned to this vertex.
 *
 * 's' must represent a valid vertex, or this method will result in undefined
 * behavior.
 * Complexity: O(degree(s))
 */
vector *graph_neighbors(graph *this, void *s);

/**
 * Returns a vector containing all vertex keys 's' in 'this' such that 't' is
 * adjacent to 's', i.e. there exists edge ('s', 't') in 'this'. These keys
 * (but not the vector itself) represent shallow copies of data present in the
 * graph and therefore should not be destroyed.
 *
 * Note that this method returns antineighbors in a FIFO order, based on which
 * antineighbors were last assigned to this vertex.
 *
 * 't' must represent a valid vertex, or this method will result in undefined
 * behavior.
 * Complexity: O(antidegree(t))
 */
vector *graph_antineighbors(graph *this, void *t);

/**
 * Returns the number of neighbors of a graph vertex with key 's'.
 *
 * 's' must represent a valid vertex, or this method will result in undefined
 * behavior.
 * Complexity: O(1)
 */
size_t graph_vertex_degree(graph *this, void *s);

/**
 * Returns the number of antineighbors of a graph vertex with key 't'.
 *
 * 't' must represent a valid vertex, or this method will result in undefined
 * behavior.
 * Complexity: O(1)
 */
size_t graph_vertex_antidegree(graph *this, void *t);

// Vertex Functions:

/**
 * Returns a vector containing all vertex keys in this graph. These keys
 * (but not the vector itself) represent shallow copies of data present
 * in the graph and therefore should not be destroyed.
 * Complexity: O(V)
 */
vector *graph_vertices(graph *this);

/**
 * Returns the number of vertices in the graph.
 * Complexity: O(1)
 */
size_t graph_vertex_count(graph *this);

/**
 * Returns whether there exists a vertex with key 'key' in 'this'.
 * Complexity: O(1)
 */
bool graph_contains_vertex(graph *this, void *key);

/**
 * Adds a vertex with 'key' to 'this'. The value associated with this vertex
 * is obtained by calling the vertex value copy constructor upon NULL.
 *
 * Note: if the vertex, 'key', already exists in 'this', then nothing happens.
 * Complexity: O(1)
 *
 * WARNING: The vertex value copy constructor and destructor must handle NULL
 * if this method is to be used.
 */
void graph_add_vertex(graph *this, void *key);

/**
 * Removes a vertex with 'key' from 'this' and its associated value.
 * All incoming and outgoing edges will be destroyed.
 *
 * 'key' must represent a valid vertex in 'this', or this method will result
 * in undefined behavior.
 * Complexity: O(degree(key) + antidegree(key))
 */
void graph_remove_vertex(graph *this, void *key);

/**
 * Gets the value associated with the vertex 'key' in 'this'. The return value
 * is a shallow copy of the data present within the graph.
 *
 * 'key' must represent a valid vertex in 'this', or this method will result
 * in undefined behavior.
 * Complexity: O(1)
 */
void *graph_get_vertex_value(graph *this, void *key);

/**
 * Sets the value associated with the vertex 'key' in 'this' to 'value'.
 *
 * If the vertex, 'key', already has a value in 'this', then the old value is
 * replaced with 'value'.
 * Complexity: O(1)
 */
void graph_set_vertex_value(graph *this, void *key, void *value);

// Edge Functions:

/**
 * Returns the number of edges in this graph.
 * Complexity: O(1)
 */
size_t graph_edge_count(graph *this);

/**
 * Adds a directed edge from 's' to 't'. The value associated with this edge
 * is obtained by calling the edge value copy constructor upon NULL.
 *
 * Note: if a directed edge from 's' to 't' already exists in 'this', then
 * nothing happens.
 * Complexity: O(1)
 *
 * WARNING: The edge value copy constructor and destructor must handle NULL
 * values if this method is to be used.
 */
void graph_add_edge(graph *this, void *s, void *t);

/**
 * Removes the directed edge from 's' to 't' and it's associated value.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 * Complexity: O(1)
 */
void graph_remove_edge(graph *this, void *s, void *t);

/**
 * Gets the value associated with the directed edge from 's' to 't' in 'this'.
 * The return value is a shallow copy of the data present within the graph.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 * Complexity: O(1)
 */
void *graph_get_edge_value(graph *this, void *s, void *t);

/**
 * Sets the value associated with the the directed edge from 's' to 't' in
 * 'this' to 'value'.
 *
 * Note: if the directed edge from 's' to 't' already has a value in 'this',
 * then the old value is replaced with 'value'.
 *
 * 's' and 't' must represent valid vertices in 'this', and the edge ('s','t')
 * must exist, or this method will result in undefined behavior.
 * Complexity: O(1)
 */
void graph_set_edge_value(graph *this, void *s, void *t, void *value);

/**
 * Creates a graph that manages no memory and compares vertex keys by their
 * virtual addresses.
 */
graph *shallow_graph_create(void);
