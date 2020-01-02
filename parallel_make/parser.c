/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "callbacks.h"
#include "compare.h"
#include "graph.h"
#include "parser.h"
#include "rule.h"
#include "vector.h"

/**
 * Creates dynamically sized array of tokens from string. The tokens
 * themselves are NOT dynamically allocated, however.
 *
 * Tokens are separated by spaces.
 */
static char **split_line_on_spaces(char *line) {
    assert(line != NULL);
    size_t space_count = 0;
    for (char *ptr = line; *ptr; ++ptr)
        if (*ptr == ' ')
            ++space_count;
    char **tokens = malloc((2 + space_count) * sizeof(char *));
    size_t tokIdx = 0;
    while (1) {
        tokens[tokIdx] = strtok(line, " ");
        line = NULL;
        if (!tokens[tokIdx])
            break;
        if (*tokens[tokIdx]) // If strtok gives empty string, don't increment
                             // idx
            ++tokIdx;
    };
    return realloc(tokens, sizeof(char *) * (tokIdx + 1));
}

/**
 * Vertex value copy constructor invoked by the graph when its vertex value
 * is set. This copy constructor takes in a string `s` and returns a rule_t
 * struct initialized so that its target is a deep copy of `s`.
 */
static void *str_to_rule_constructor(void *s) {
    if (!s)
        return NULL;
    rule_t *out = malloc(sizeof(rule_t));
    rule_init(out);
    out->target = (char *)string_copy_constructor(s);
    return out;
}

/**
 * Rule destructor invoked by the graph when it needs to destroy a vertex.
 */
static void rule_destroy_v(void *r) {
    rule_t *rule = (rule_t *)r;
    rule_destroy(rule);
}

/**
 * Return true if the current line refers to a valid Makefile target name
 */
static bool is_makefile_target(char const *line) {
    return isalnum(line[0]) || (line[0] == '.') || line[0] == '/';
}

/**
 * Discard all line number information from vertices for user's sake
 */
static void zero_out_states(graph *dependency_graph) {
    vector *vertices = graph_vertices(dependency_graph);
    VECTOR_FOR_EACH(vertices, target, {
        ((rule_t *)graph_get_vertex_value(dependency_graph, target))->state = 0;
    });
    vector_destroy(vertices);
}

/**
 * Return a pointer to the first non-whitespace character in `str`, or to
 * the null byte if none exists.
 */
static char *skip_whitespace(char const *str) {
    while (*str)
        if (!isspace(*(str++)))
            return (char *)(str - 1);
    return (char *)str;
}

/**
 * Return a pointer to the first whitespace character in `str`, or to
 * the null byte if none exists.
 */
static char *find_whitespace(char const *str) {
    while (*str)
        if (isspace(*(str++)))
            return (char *)(str - 1);
    return (char *)str;
}

/**
 * Removes everything in `str` after the location of the first whitespace
 * character (if any) by setting that character to a null byte.
 */
static char *terminate_whitespace(char *str) {
    char *search = find_whitespace(str);
    *search = '\0';
    return search;
}

/**
 * Removes all trailing whitespace characters (if any) by replacing them with
 * null bytes.
 */
static char *strip_back_whitespace(char *str) {
    size_t len = strlen(str);
    char *back = str + len - 1;
    while (back >= str && isspace(*back)) {
        *back-- = '\0';
    }
    return str;
}

/**
 * Return whether the location `ptr` in `str` is enclosed by quotes or not.
 * This is determined by counting all non-escaped quote characters
 */
static bool in_quotes(char const *str, char const *ptr) {
    bool inQuotes = false;
    for (char const *p = str; p < ptr; ++p) {
        if (*p == '\\')
            ++p;
        else if (*p == '"')
            inQuotes = !inQuotes;
    }
    return inQuotes;
}

/**
 * Identifies where comment starts (if at all) and deletes everything after
 * this point by inserting a null byte.
 */
static void strip_back_comments(char *str) {
    char *comment_str = str;
    while ((comment_str = strstr(comment_str, "#"))) {
        // Ignore # symbol if it occurs within quotes
        if (!in_quotes(str, comment_str)) {
            // Terminate comment
            *comment_str = '\0';
            break;
        } else {
            // Keep looking for comments
            ++comment_str;
        }
    }
}

/**
 * Return a FILE pointer to the makefile at the path `makefile_name` if
 * `makefile_name` is not NULL and refers to an actual file.
 */
static FILE *open_makefile(const char *makefile_name, char **goals) {
    // Open the makefile
    if (!makefile_name && (!goals || !*goals)) {
        fprintf(stderr, "parmake: *** No targets specified and no makefile "
                        "found.  Stop.\n");
        exit(2);
    } else if (!makefile_name) {
        fprintf(stderr, "parmake: *** No rule to make target '%s'.  Stop.\n",
                goals[0]);
        exit(2);
    }
    FILE *f = fopen(makefile_name, "r");
    if (!f) {
        fprintf(stderr, "parmake: %s: No such file or directory\n",
                makefile_name);
        exit(2);
    }
    return f;
}

graph *parser_parse_makefile(const char *makeFileName, char **goals) {
    FILE *f = open_makefile(makeFileName, goals);
    graph *dependency_graph = graph_create(
        string_hash_function, string_compare, string_copy_constructor,
        string_destructor, str_to_rule_constructor, rule_destroy_v, NULL, NULL);
    // capture first rule name in case user did not specify goals
    char *first_target = NULL;

    rule_t *curr_rule = NULL;

    char *lineBuf = NULL;
    size_t bytes = 0;
    ssize_t len = 0;
    size_t line_number = 0;
    // Used to identify redefined rules
    int in_command_block = 0;

    // First add sentinel vertex
    graph_set_vertex_value(dependency_graph, "", "");

    while ((len = getline(&lineBuf, &bytes, f)) != -1) {
        ++line_number;
        // Remove newline / carriage return
        if (len && lineBuf[len - 1] == '\n') {
            lineBuf[--len] = '\0';
            if (len && lineBuf[len - 1] == '\r')
                lineBuf[--len] = '\0';
        }

        strip_back_whitespace(lineBuf);
        char *line = skip_whitespace(lineBuf);
        strip_back_comments(line);

        if (!(*line))
            continue;

        if (lineBuf[0] == '\t') {
            // Recipe line
            if (!curr_rule) {
                fprintf(stderr,
                        "%s:%zu: *** recipe commences before first target.  "
                        "Stop.\n",
                        makeFileName, line_number);
                exit(2);
            }
            // First command in block
            if (!in_command_block) {
                // This rule has no commands yet
                if (!curr_rule->state) {
                    curr_rule->state = (int)line_number;
                } else {
                    // This rule's commands are being redefined
                    fprintf(stderr,
                            "%s:%zu: warning: overriding recipe for target "
                            "'%s'\n",
                            makeFileName, line_number, curr_rule->target);
                    fprintf(stderr,
                            "%s:%zu: warning: ignoring old recipe for target "
                            "'%s'\n",
                            makeFileName, (size_t)curr_rule->state,
                            curr_rule->target);
                    curr_rule->state = (int)line_number;
                    vector_clear(curr_rule->commands);
                }
            }
            in_command_block = 1;
            vector_push_back(curr_rule->commands, line);
        } else if (is_makefile_target(line)) {
            in_command_block = 0;
            // Found start of new rule line
            curr_rule = NULL;
            // Find first colon to split on
            char *depLine = strstr(line, ":");
            // Did we actually find it?
            if (!depLine) {
                fprintf(stderr, "%s:%zu: *** missing separator.  Stop.\n",
                        makeFileName, line_number);
                exit(2);
            }

            // Remove the colon, and setup target name and dependency string
            depLine[0] = '\0';
            ++depLine;
            // Remove trailing whitespace and other nonsense
            terminate_whitespace(line);

            // We found a new rule, so let's push this to the dependency graph
            if (!graph_contains_vertex(dependency_graph, line)) {
                // Found a completely new rule, so we must add it to the graph
                graph_set_vertex_value(dependency_graph, line, line);
                // Check if this is the first target in the Makefile, saving it
                // if so
                if (!first_target)
                    first_target = string_copy_constructor(line);
            }

            curr_rule = graph_get_vertex_value(dependency_graph, line);
            char **dependencies = split_line_on_spaces(depLine);

            for (char **depPtr = dependencies; *depPtr; ++depPtr) {
                // We found a new rule, so push it to the dependency graph
                if (!graph_contains_vertex(dependency_graph, *depPtr)) {
                    graph_set_vertex_value(dependency_graph, *depPtr, *depPtr);
                }
                // Create a dependency edge
                graph_add_edge(dependency_graph, line, *depPtr);
            }
            free(dependencies);
        } else {
            fprintf(stderr, "%s:%zu: *** missing separator.  Stop.\n",
                    makeFileName, line_number);
            exit(2);
        }
    }
    free(lineBuf);
    // If no goals specified, build the first rule ever defined
    char *default_targets[] = {first_target, NULL};
    if (!goals || !(*goals)) {
        if (!first_target) {
            fprintf(stderr, "parmake: *** No targets.  Stop.\n");
            exit(2);
        }
        goals = default_targets;
    }
    for (char **targetPtr = goals; *targetPtr; ++targetPtr) {
        if (!graph_contains_vertex(dependency_graph, *targetPtr)) {
            fprintf(stderr,
                    "parmake: *** No rule to make target '%s'.  Stop.\n",
                    *targetPtr);
            exit(1);
        } else {
            graph_add_edge(dependency_graph, "", *targetPtr);
        }
    }
#ifdef EASY_MODE
    remove_unnecessary_targets(dependency_graph, goals);
#endif
    zero_out_states(dependency_graph);
    free(first_target);
    fclose(f);
    return dependency_graph;
}
