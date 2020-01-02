/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "format.h"


int is_valid_args(int, char ***, char **);
int arr_size(void **arr);
char ***parse_input(char **argv, int *env_size, char ***cmd);
char **parse_env(const char*);
void print_args(const int, char ***, char **);
void print_arr(char **); 
void destroy(char ***);


int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_env_usage();
    }
    int env_size = 1;
    char **cmd = NULL;
    char ***variables = parse_input(argv, &env_size, &cmd);
    // print_args(env_size, variables, cmd);
    
    if (!is_valid_args(env_size, variables, cmd)) {
        destroy(variables);
        print_env_usage();
    }

    for (int i = 1; i < env_size + 1; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            print_fork_failed();
        } else if (pid == 0) {
            int num_env = arr_size((void**)variables);
            for (int j = 0; j < num_env; ++j) {
                char *key = variables[j][0];
                char *val = variables[j][arr_size((void**)variables[j]) > 2 ? i : 1];
                if (val[0] == '%') {
                    if ((val = getenv(val + 1)) == NULL)
                        val = "";
                }
                if (setenv(key, val, 1) == -1) {
                    print_environment_change_failed();
                }
            }
            execvp(cmd[0], cmd);
            destroy(variables);
            exit(1);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (!WIFEXITED(status) || WEXITSTATUS(status)) {
                destroy(variables);
                print_exec_failed();
            }
        }
    }
    destroy(variables);
    return 0;
}


char ***parse_input(char **argv, int *env_size, char ***cmd) {
    size_t num_env = 0;
    char **curr = argv + 1;
    char ***variables = malloc((num_env + 1) * sizeof(char**));
    while (*curr != NULL) {
        if (strcmp(*curr, "-n") == 0 && curr == argv + 1) {
            *env_size = atoi(*(++curr));
            ++curr;
            continue;
        } else if (!strcmp(*curr, "--")) {
            *cmd = curr + 1;
            break;
        }
        variables[num_env] = parse_env(*curr);
        variables = realloc(variables, (++num_env + 1) * sizeof(char**));
        ++curr;
    }
    variables[num_env] = NULL;
    return variables;
}


char **parse_env(const char* ptr) {
    size_t size = 0;
    char **result = malloc((size + 1) * sizeof(char*));
    const char *begin = ptr;
    const char *end = strchr(begin, '=');
    while (end) {
        result[size] = malloc(end - begin + 1);
        strncpy(result[size], begin, end - begin);
        result[size][end - begin] = '\0';
        result = realloc(result, (++size + 1) * sizeof(char*));
        begin = end + 1;
        end = strchr(begin, ',');
    }
    if (size > 0 && strlen(begin) > 0) {
        result[size] = malloc(strlen(begin) + 1);
        strcpy(result[size], begin);
        result = realloc(result, (++size + 1) * sizeof(char*));
    }
    result[size] = NULL;
    return result;
}


int is_valid_args(int env_size, char ***variables, char **cmd) {
    if (env_size <= 0 || cmd == NULL || *cmd == NULL)
        return 0;
    int outer_size = arr_size((void**)variables);
    if (outer_size == 0)
        return 0;
    for (int i = 0; i < outer_size; ++i) {
        int inner_size = arr_size((void**)variables[i]);
        if (inner_size != env_size + 1 && inner_size != 2)
            return 0;
        for (int j = 0; j < inner_size; ++j) {
            int str_size = strlen(variables[i][j]);
            for (int k = 0; k < str_size; ++k) {
                char c = variables[i][j][k];
                if (isalpha(c) || isdigit(c) || c == '_')
                    continue;
                else if (j > 0 && k == 0 && c == '%')
                    continue;
                else
                    return 0;
            }
        }
    }
    return 1;
}


int arr_size(void **arr) {
    if (arr == NULL)
        return 0;
    int count = 0;
    void **curr = arr;
    while(*curr) {
        ++count;
        ++curr;
    }
    return count;
}

void print_args(const int env_size, char ***variables, char **cmd) {
    printf("env_size: %d\n", env_size);
    int num_env = arr_size((void**)variables);
    puts("env:");
    for (int i = 0; i < num_env; ++i) {
        printf("\tenv %d - ", i);
        int env_size = arr_size((void**)variables[i]);
        for (int j = 0; j < env_size; ++j) {
            if (j == 0)
                printf("key: %s\n", variables[i][j]);
            else
                printf("\t        val %d: %s\n", j - 1, variables[i][j]);
        }
    }
    int cmd_size = arr_size((void**)cmd);
    puts("cmd:");
    for (int i = 0; i < cmd_size; ++i)
        printf("\t%s ", cmd[i]);
    puts("\n*****************");
}

void print_arr(char **arr) {
    int size = arr_size((void**) arr);
    for (int i = 0; i < size; ++i) {
        puts(arr[i]);
    }
    printf("%s\n", NULL);
}

void destroy(char ***arr) {
    if (arr == NULL)
        return;
    int outer = arr_size((void**)arr);
    for (int i = 0; i < outer; ++i) {
        int inner = arr_size((void**)arr[i]);
        for (int j = 0; j < inner; ++j) {
            free(arr[i][j]);
        }
        free(arr[i]);
    }
    free(arr);
}

