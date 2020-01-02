/**
* Shell Lab
* CS 241 - Fall 2018
*/

#include "format.h"
#include "shell.h"
#include "vector.h"
#include <string.h>
#include "sstring.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define STATUS_KILLED "Killed"

typedef struct process {
    char *command;
    char *status;
    pid_t pid;
} process;



void signal_handler(int);
size_t set_process_status(pid_t, char*);
void handle_args(int, char**);
int is_returnable(int);
int get_command_id(char*);
void add_process(char*, char*, pid_t);
char *get_directory(char*);
int execute(char *);
int execute_command(int, char*, int);
void shell_processes();
void shell_exit(int);
int shell_execute_external(char*, char**);
int is_background_command(char**);
char **split_input(char*);
char *concat_strings(char**);
size_t get_process_idx(pid_t pid);
void input_destroy(char**);



static vector *history = NULL;
static vector *processes = NULL;
static char *history_file_name = NULL;
static FILE *source = NULL;

int shell(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);
    history = string_vector_create();
    processes = shallow_vector_create();
    source = stdin;
    handle_args(argc, argv);

    // store shell pid in processes
    char *arg_string = concat_strings(argv);
    add_process(arg_string, STATUS_RUNNING, getpid());
    
    // here is the real stuff...
    size_t byte_read = 0;
    char *line = NULL;
    char *wd_buffer = malloc(256);
    print_prompt(get_directory(wd_buffer), getpid());
    while (getline(&line, &byte_read, source) != -1) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) -1] = '\0';
        char *line_cpy = malloc(strlen(line));
        strcpy(line_cpy, line);
        execute(line_cpy);
        print_prompt(get_directory(wd_buffer), getpid());
        fflush(stdout);
    }
    free(line);
    shell_exit(0);
    return 0;
}


void signal_handler(int sig) {    
    if (sig == SIGINT) {
        // capture SIGINT, and do nothing
    } else if (sig == SIGCHLD) {
        pid_t pid = 0;
        int status = 0;
        if ((pid = waitpid(-1, &status, WNOHANG)) != -1) {
            set_process_status(pid, STATUS_KILLED);
        }
    }
}

size_t set_process_status(pid_t pid, char *status) {
    for (size_t i = 0; i < vector_size(processes); ++i) {
        process *p = vector_get(processes, i);
        if (p->pid == pid) {
            p->status = status;
            return i;
        }
    }
    return -1;
}


void handle_args(int argc, char *argv[]) {
    source = stdin;
    int c;
    while ((c = getopt(argc, argv, "h:f:")) != -1) {
        if (c == 'h') {
            FILE *history_file = fopen(optarg, "r");
            if (!history_file) {
                print_history_file_error();
                shell_exit(1);
            }
            char *line = NULL;
            size_t byte_read = 0;
            while (getline(&line, &byte_read, history_file) != -1) {
                if (line[strlen(line) - 1] == '\n')
                    line[strlen(line) -1] = '\0';
                vector_push_back(history, (void *)line);
            }
            free(line);
            fclose(history_file);
            history_file = NULL;
            history_file_name = get_full_path(optarg);
        } else if (c == 'f') {
            FILE *script_file = fopen(optarg, "r");
            if (!script_file) {
                print_script_file_error();
                shell_exit(1);
            }
            source = script_file;
        } else if (c == '?') {
            print_usage();
            shell_exit(1);
        }
    }
}


int execute(char *command) {
    char *loc = NULL;
    char *operator = NULL;
    if ((loc = strstr(command, "&&")) != NULL) {
        operator = "&&";
    } else if ((loc = strstr(command, "||")) != NULL) {
        operator = "||";
    } else if ((loc = strstr(command, ";")) != NULL) {
        operator = ";";
    }
    if (loc != NULL) {
        memcpy(loc, "\0\0", strlen(operator));
        char *command_1 = command;
        char *command_2 = loc + strlen(operator);
        int command_id_1 = get_command_id(command_1);
        int command_id_2 = get_command_id(command_2);
        if (strcmp(operator, "&&") == 0) {
            if (is_returnable(command_id_1)) {
                int retval = execute_command(command_id_1, command_1, 0);
                if (retval == 0) {
                    if (is_returnable(command_id_2)) {
                        memcpy(loc, operator, strlen(operator));
                        vector_push_back(history, command);
                        return execute_command(command_id_2, command_2, 0);
                    }
                } else {
                    memcpy(loc, operator, strlen(operator));
                    vector_push_back(history, command);
                    return retval;
                }
            }
        } else if (strcmp(operator, "||") == 0) {
            if (is_returnable(command_id_1)) {
                int retval = execute_command(command_id_1, command_1, 0);
                if (retval != 0) {
                    if (is_returnable(command_id_2)) {
                        memcpy(loc, operator, strlen(operator));
                        vector_push_back(history, command);
                        return execute_command(command_id_2, command_2, 0);
                    }
                } else {
                    memcpy(loc, operator, strlen(operator));
                    vector_push_back(history, command);
                    return 0;
                }
            }
        } else {
            if (is_returnable(command_id_1)) {
                execute_command(command_id_1, command_1, 0);
                if (is_returnable(command_id_2)) {
                    memcpy(loc, operator, strlen(operator));
                    vector_push_back(history, command);
                    return execute_command(command_id_2, command_2, 0);
                }
            }
        }
        memcpy(loc, operator, strlen(operator));
        vector_push_back(history, command);
        print_invalid_command(command);
        return 1;
    } else {
        return execute_command(get_command_id(command), command, 1);
    }
}

int is_returnable(int command_id) {
    if (command_id == 2 || command_id == 3 || command_id == 4 || command_id == 9) {
        return 0;
    }
    return 1;
}

/**
 * 0: invalid command
 * 1: cd
 * 2: !history
 * 3: #<n>
 * 4: !<prefix>
 * 5: ps
 * 6: kill
 * 7: stop
 * 8: cont
 * 9: exit
 * 10: external
 */

int get_command_id(char *command) {
    char **inputs = split_input(command);
    char **curr = inputs;
    // count inputs size
    size_t size = 0;
    while (*curr != NULL) {
        ++size;
        ++curr;
    }
    // validate inputs
    int command_idx;
    if (size == 0) {
        command_idx = 0;
    } else if (strcmp(inputs[0], "cd") == 0) {
        command_idx = (size == 2) ? 1 : 0;
    } else if (strcmp(inputs[0], "!history") == 0) {
        command_idx = 2;
    } else if (inputs[0][0] == '#') {
        command_idx = 3;
    } else if (inputs[0][0] == '!') {
        command_idx = 4;
    } else if (strcmp(inputs[0], "ps") == 0) {
        command_idx = (size == 1) ? 5 : 0;
    } else if (strcmp(inputs[0], "kill") == 0) {
        command_idx = (size == 2) ? 6 : 0;
    } else if (strcmp(inputs[0], "stop") == 0) {
        command_idx = (size == 2) ? 7 : 0;
    } else if (strcmp(inputs[0], "cont") == 0) {
        command_idx = (size == 2) ? 8 : 0;
    } else if (strcmp(inputs[0], "exit") == 0) {
        command_idx = 9;
    } else {
        command_idx = 10;
    }
    // free inputs
    input_destroy(inputs);
    return command_idx;
}


void add_process(char *command, char *status, pid_t pid) {
    for (size_t i = 0; i < vector_size(processes); ++i) {
        process *p = vector_get(processes, i);
        if (p->pid == pid) {
            p->command = command;
            p->status = status;
            return;
        }
    }
    process *p = malloc(sizeof(process));
    p->command = command;
    p->status = status;
    p->pid = pid;
    vector_push_back(processes, p);
}


char *get_directory(char *buffer) {
    if (getcwd(buffer, 256) == NULL) {
        shell_exit(1);
    }
    return buffer;
}



int execute_command(int command_id, char *command, int save_to_history) {
    char **inputs = split_input(command);
    char **curr = inputs;
    size_t size = 0;
    while (*curr != NULL) {
        ++size;
        ++curr;
    }
    if (is_returnable(command_id) && save_to_history) {
        vector_push_back(history, command);
    }
    if (command_id == 0) {
        print_invalid_command(command);
        return 1;
    } else if (command_id == 1) {
        if (chdir(inputs[1]) == -1) {
            print_no_directory(inputs[1]);
            return 1;
        }
        return 0;
    } else if (command_id == 2) {
        if (size != 1) {
            print_invalid_command(command);
            return 1;
        }
        for (size_t i = 0; i < vector_size(history); ++i) {
            print_history_line(i, vector_get(history, i));
        }
        return 0;
    } else if (command_id == 3) {
        if (size != 1 || strlen(inputs[0]) == 1) {
            print_invalid_command(command);
            return 1;
        }
        size_t index = atoi(inputs[0] + 1);
        if (index < vector_size(history)) {
            char *cmd = vector_get(history, index);
            print_command(cmd);
            return execute_command(get_command_id(cmd), cmd, save_to_history);
        }
        print_invalid_index();
        return 1;
    } else if (command_id == 4) {
        if (size != 1) {
            print_invalid_command(command);
            return 1;
        }
        char *prefix = inputs[0] + 1;
        for (size_t i = vector_size(history); i > 0; --i) {
            char *cmd = vector_get(history, i - 1);
            if (strncmp(cmd, prefix, strlen(prefix)) == 0) {
                print_command(cmd);
                return execute_command(get_command_id(cmd), cmd, save_to_history);
            }
        }
        print_no_history_match();
        return 1;
    } else if (command_id == 5) {
        shell_processes();
        return 0;
    } else if (command_id == 6) {
        pid_t target_pid = atoi(inputs[1]);
        size_t idx;
        if ((idx = set_process_status(target_pid, STATUS_KILLED)) == (size_t)-1) {
            print_no_process_found(target_pid);
            return 1;
        }
        kill(target_pid, SIGKILL);
        print_killed_process(target_pid, ((process*)vector_get(processes, idx))->command);
        return 0;
    } else if (command_id == 7) {
        pid_t target_pid = atoi(inputs[1]);
        size_t idx;
        if ((idx = set_process_status(target_pid, STATUS_STOPPED)) == (size_t)-1) {
            print_no_process_found(target_pid);
            return 1;
        }
        kill(target_pid, SIGSTOP);
        print_stopped_process(target_pid, ((process*)vector_get(processes, idx))->command);
        return 0;
    } else if (command_id == 8) {
        pid_t target_pid = atoi(inputs[1]);
        size_t idx;
        if ((idx = set_process_status(target_pid, STATUS_RUNNING)) == (size_t)-1) {
            print_no_process_found(target_pid);
            return 1;
        }
        kill(target_pid, SIGCONT);
        return 0;
    } else if (command_id == 9) {
        if (size != 1) {
            print_invalid_command(command);
            return 1;
        }
        shell_exit(0);
        return 0;
    } else {
        fflush(stdout);
        return shell_execute_external(command, inputs);
    }
    input_destroy(inputs);
}


void shell_processes() {
    for (size_t i = 0; i < vector_size(processes); ++i) {
        process *p = vector_get(processes, i);
        if (strcmp(p->status, STATUS_KILLED)) {
            print_process_info(p->status, p->pid, p->command);
        }
    }
}


void shell_exit(int status) {
    for (size_t i = 1; i < vector_size(processes); ++i) {
        process *p = vector_get(processes, i);
        if (strcmp(p->status, STATUS_KILLED) && getpgid(p->pid) != p->pid) {
            kill(p->pid, SIGKILL);
        }
    }
    if (history_file_name != NULL) {
        FILE *history_file = fopen(history_file_name, "w");
        for (size_t i = 0; i < vector_size(history); ++i) {
            fprintf(history_file, "%s\n", (char*)vector_get(history, i));
        }
        fclose(history_file);
    }
    vector_destroy(history);
    vector_destroy(processes);
    if (source != stdin) {
        fclose(source);
    }
    exit(status);
}


int shell_execute_external(char* command, char** inputs) {
    int is_background = is_background_command(inputs);
    pid_t pid = fork();
    add_process(command, STATUS_RUNNING, pid);
    if (pid > 0) {
        print_command_executed(pid);
        int status = 0;
        if (is_background) {
            waitpid(pid, &status, WNOHANG);
        } else {
            int error = waitpid(pid, &status, 0);
            if (error == -1) {
                print_wait_failed();
                shell_exit(-1);
            } else if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0){
                    shell_exit(1);
                }
                fflush(stdout);
                set_process_status(pid, STATUS_KILLED);
            } else if (WIFSIGNALED(status)) {
                set_process_status(pid, STATUS_KILLED);
            }
        }
        return status;
    } else if (pid == 0) {
        if (is_background) {
            if (setpgid(getpid(), getpid()) == -1) {
                print_setpgid_failed();
                fflush(stdout);
                shell_exit(1);
            }
        }
        fflush(stdout);
        execvp(inputs[0], inputs);
        print_exec_failed(command);
        fflush(stdout);
        exit(1);
        return 1;
    } else {
        print_fork_failed();
        shell_exit(1);
        return 1;
    }
}


int is_background_command(char** inputs) {
    char **curr = inputs;
    size_t size = 0;
    while (*curr != NULL) {
        ++size;
        ++curr;
    }
    char *last_elem = inputs[size - 1];
    size_t last_elem_len = strlen(last_elem);
    if (last_elem[last_elem_len - 1] == '&') {
        if (last_elem_len == 1) {
            free(last_elem);
            inputs[size - 1] = NULL;
        } else {
            last_elem[last_elem_len - 1] = '\0';
        }
        return 1;
    }
    return 0;
}


char **split_input(char* input) {
    size_t size = 0;
    char **result = malloc((size + 1) * sizeof(char*));
    char *head = input;
    char *tail = NULL;
    while ((tail = strchr(head, ' ')) != NULL) {
        size_t len = tail - head;
        if (len != 0) {
            char * temp = malloc(len + 1);
            strncpy(temp, head, len);
            temp[len] = '\0';
            result[size++] = temp;
            result = realloc(result, (size + 1) * sizeof(char*));
        }
        head = tail + 1;
    }
    if (*head != '\0') {
        char *temp = malloc(strlen(head) + 1);
        strcpy(temp, head);
        result[size++] = temp;
        result = realloc(result, (size + 1) * sizeof(char*));
    }
    result[size] = NULL;
    return result;
}


char *concat_strings(char **args) {
    char *result = malloc(1);
    result[0] = '\0';
    char **curr = args;
    while (*curr != NULL) {
        result = realloc(result, strlen(result) + strlen(*curr) + 2);
        strcat(result, *curr);
        strcat(result, " ");
        ++curr;
    }
    result[strlen(result) - 1] = '\0';
    return result;
}

size_t get_process_idx(pid_t pid) {
    for (size_t i = 0; i < vector_size(processes); ++i) {
        if (((process*)vector_get(processes, i))->pid == pid)
            return i;
    }
    return -1;
}


void input_destroy(char **input) {
    char **curr = input;
    while (*curr != NULL) {
        free(*curr);
        ++curr;
    }
    free(input);
}

