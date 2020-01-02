/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"
#include "sstring.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


vector *handle_escape_character(const char *);

char *get_filename(int argc, char *argv[]) {
    return argv[argc - 1];
}

sstring *handle_create_string() {
    return cstr_to_sstring("");
}

document *handle_create_document(const char *path_to_file) {
    return document_create_from_file(path_to_file);
}

void handle_cleanup(editor *editor) {
    document_destroy(editor->document);
}

void handle_display_command(editor *editor, size_t start_line, ssize_t max_lines, 
                            size_t start_col_index, ssize_t max_cols) {
    size_t max_line_no = document_size(editor->document);
    if (max_line_no <= 0) {
        print_document_empty_error();
        return;
    }
    if (max_lines != -1)
        max_line_no = MIN(start_line + max_lines - 1, max_line_no);
    for (size_t line_no = start_line; line_no <= max_line_no; ++line_no)
        print_line(editor->document, line_no, start_col_index, max_cols);
}

void handle_insert_command(editor *editor, location loc, const char *line) {
    assert(loc.line_no > 0);
    size_t max_line_no = document_size(editor->document);
    if (loc.line_no <= max_line_no) {
        const char * old_str = document_get_line(editor->document, loc.line_no);
        if (loc.idx <= strlen(old_str)) {
            char * new_str = malloc(strlen(old_str) + strlen(line) + 1);
            strncpy(new_str, old_str, loc.idx);
            strcpy(new_str + loc.idx, line);
            strcpy(new_str + strlen(new_str), old_str + loc.idx);
            document_set_line(editor->document, loc.line_no, new_str);
            free(new_str);
        }
    } else {
        if (loc.idx == 0) {
            document_insert_line(editor->document, loc.line_no, line);
        }
    }
}

void handle_append_command(editor *editor, size_t line_no, const char *line) {
    vector *parts = handle_escape_character(line);
    size_t num_parts = vector_size(parts);
    for (size_t i = 0; i < num_parts; ++i) {
        document_insert_line(editor->document, line_no + i + 1, vector_get(parts, i));
    }
    vector_destroy(parts);
}

void handle_write_command(editor *editor, size_t line_no, const char *line) {
    assert(line_no > 0);
    if (line_no <= document_size(editor->document)) {
        document_delete_line(editor->document, line_no);
    }
    handle_append_command(editor, line_no - 1, line);
}

void handle_delete_command(editor *editor, location loc, size_t num_chars) {
    const char *old_str = document_get_line(editor->document, loc.line_no);
    size_t actual_num_chars = MIN(strlen(old_str) - loc.idx, num_chars);
    char *new_str = malloc(strlen(old_str) - actual_num_chars + 1);
    strncpy(new_str, old_str, loc.idx);
    strcpy(new_str + loc.idx, old_str + loc.idx + actual_num_chars);
    document_set_line(editor->document, loc.line_no, new_str);
    free(new_str);
}

void handle_delete_line(editor *editor, size_t line_no) {
    if (line_no > document_size(editor->document)){
        for (size_t i = document_size(editor->document); i > 0; --i)
            document_delete_line(editor->document, i);
    } else {
        document_delete_line(editor->document, line_no);
    }
}

location handle_search_command(editor *editor, location loc, const char *search_str) {
    if (search_str == NULL || strcmp(search_str, "") == 0)
        return (location){0, 0};
    size_t num_lines = document_size(editor->document);
    for (size_t i = loc.line_no; i <= num_lines; ++i) {
        const char *str = document_get_line(editor->document, i);
        const char *start = str;
        if (i == loc.line_no) {
            if (loc.idx < strlen(str))
                start += loc.idx;
            else
                continue;
        }
        char *ptr = strstr(start, search_str);
        if (ptr)
            return (location){i, ptr - str};
    }
    // wraps to the begining of the document
    for (size_t i = 1; i <= loc.line_no; ++i) {
        const char *str = document_get_line(editor->document, i);
        char *ptr = strstr(str, search_str);
        if (ptr)
            return (location){i, ptr - str};
    }
    return (location){0, 0};
}

void handle_merge_line(editor *editor, size_t line_no) {
    const char *str1 = document_get_line(editor->document, line_no);
    const char *str2 = document_get_line(editor->document, line_no + 1);
    char *new_str = malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(new_str, str1);
    strcpy(new_str + strlen(str1), str2);
    document_set_line(editor->document, line_no, new_str);
    document_delete_line(editor->document, line_no + 1);
    free(new_str);
}

void handle_split_line(editor *editor, location loc) {
    const char *old_str = document_get_line(editor->document, loc.line_no);
    char *new_str = malloc(strlen(old_str) + 2);
    strncpy(new_str, old_str, loc.idx);
    new_str[loc.idx] = '\n';
    strcpy(new_str + loc.idx + 1, old_str + loc.idx);
    handle_write_command(editor, loc.line_no, new_str);
    free(new_str);
}

void handle_save_command(editor *editor) {
    document_write_to_file(editor->document, editor->filename);
}

vector *handle_escape_character(const char *line) {
    char *line_cpy = malloc(strlen(line) + 1);
    size_t len = 0;
    const char *curr = line;
    const char *term = line + strlen(line);
    while (curr <= term) {
        if (*curr != '\\') {
            line_cpy[len] = *curr;
            ++len;
            ++curr;
        } else if (curr + 1 < term) {
            char next_char = *(curr + 1);
            if (next_char == 'n')
                line_cpy[len] = '\n';
            else if (next_char == '\\')
                line_cpy[len] = '\\';
            else
                line_cpy[len] = next_char;
            ++len;
            curr += 2;
        } else {
            ++curr;
        }
    }
    vector *v = string_vector_create();
    char *start = line_cpy;
    char *pos = NULL;
    while ((pos = strchr(start, '\n')) != NULL) {
        *pos = '\0';
        vector_push_back(v, (void*) start);
        *pos = '\n';
        start = pos + 1;
    }
    vector_push_back(v, (void*) start);
    free(line_cpy);
    return v;
}