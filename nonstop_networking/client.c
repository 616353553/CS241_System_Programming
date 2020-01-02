/**
 * Networking Lab
 * CS 241 - Fall 2018
 */

#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"
#include "format.h"

#define HEADER_SIZE 1024
#define BUFFER_SIZE 2048

void connnect_to_server(char *host, char *port);
char **parse_args(int argc, char **argv);
verb check_args(char **args);
size_t parse_header(char *buffer, char ***result);
int read_from_server(size_t (*handler)(char *, size_t));
size_t list_handler(char *buffer, size_t buffer_len);
size_t get_handler(char *buffer, size_t buffer_len);
size_t write_buffer(char *buffer, size_t buffer_len, int fd);
void free_array(char **list);

// global variables
static int local_fd = -1;
static int server_fd = -1;
static size_t file_size = (size_t)-1;
static size_t file_read = 0;
static char **args = NULL;
static verb method = V_UNKNOWN;

int main(int argc, char **argv) {
    args = parse_args(argc, argv);
    method = check_args(args);
    connnect_to_server(args[0], args[1]);
    // GET, PUT, DELETE, LIST
    if (method == GET) {
        // local_fd = open(args[4], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        // send header, avoid using dprintf()
        write(server_fd, "GET ", 4);
        write(server_fd, args[3], strlen(args[3]));
        write(server_fd, "\n", 1);
        shutdown(server_fd, SHUT_WR);
        // read binary data
        if (read_from_server(get_handler) < 0) {
            free(args);
            exit(1);
        }
        if (file_size != (size_t)-1) {
            if (file_size > file_read) {
                print_too_little_data();
                unlink(args[4]);
            } else if (file_size < file_read) {
                print_received_too_much_data();
                unlink(args[4]);
            }
        }
    } else if (method == PUT) {
        struct stat st;
        stat(args[4], &st);
        file_size = st.st_size;
        char *file_size_str = size_to_string(file_size);
        // send header, avoid using dprintf()
        write(server_fd, "PUT ", 4);
        write(server_fd, args[3], strlen(args[3]));
        write(server_fd, "\n", 1);
        write(server_fd, file_size_str, 8);
        free(file_size_str);
        local_fd = open(args[4], O_RDONLY);
        if (local_fd == -1) {
            perror("open");
            free(args);
            exit(1);
        }
        // send binary data
        char *buffer = calloc(BUFFER_SIZE, 1);
        size_t buffer_len = 0;
        while (1) {
            size_t byte_read = read(local_fd, buffer + buffer_len, BUFFER_SIZE - buffer_len - 1);
            if (byte_read == (size_t)-1) {
                if (errno != EINTR) {
                    perror("read");
                    free(buffer);
                    free(args);
                    exit(1);
                }
            } else if (byte_read > 0) {
                buffer_len += byte_read;
                buffer[buffer_len] = '\0';
                size_t byte_write = write_buffer(buffer, buffer_len, server_fd);
                if (byte_write == (size_t)-1) {
                    free(buffer);
                    free(args);
                    exit(1);
                } else if (byte_write > 0) {
                    shift_forward(buffer, buffer_len, byte_write);
                    buffer_len -= byte_write;
                }
            } else {
                break;
            }
        }
        shutdown(server_fd, SHUT_WR);
        free(buffer);
        free(args);
        exit(read_from_server(NULL) < 0 ? 1 : 0);
    } else if (method == DELETE) {
        // send header, avoid using dprintf()
        write(server_fd, "DELETE ", 7);
        write(server_fd, args[3], strlen(args[3]));
        write(server_fd, "\n", 1);
        shutdown(server_fd, SHUT_WR);
        // read binary data
        if (read_from_server(NULL) < 0) {
            free(args);
            exit(1);
        }
    } else if (method == LIST) {
        // send header, avoid using dprintf()
        write(server_fd, "LIST\n", 5);
        shutdown(server_fd, SHUT_WR);
        // read binary data
        if (read_from_server(list_handler) < 0) {
            free(args);
            exit(1);
        }
        if (file_size > file_read) {
            print_too_little_data();
        } else if (file_size < file_read) {
            print_received_too_much_data();
        }
    }
    free(args);
    return 0;
}


void connnect_to_server(char *host, char *port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket error");
        exit(1);
    }
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }
    if (connect(server_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect error");
        exit(1);
    }
    freeaddrinfo(result);
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}


size_t parse_header(char *buffer, char ***result) {
    int num_token = 0;
    char **response = calloc(3, sizeof(char *)); 
    size_t offset = 0;
    for (size_t i = 0; i < strlen(buffer); ++i) {
        if (buffer[i] == '\n') {
            size_t str_len = i - offset;
            char *ptr = malloc(str_len + 1);
            strncpy(ptr, buffer + offset, str_len);
            ptr[str_len] = '\0';
            response[num_token++] = ptr;
            offset = i + 1;
            if (num_token == 2) {
                break;
            }
        }
    }
    response[num_token] = NULL;
    if (num_token == 0) {
        return 0;
    } else if (num_token == 1) {
        if (strcmp(response[0], "OK") == 0) {
            *result = response;
            return 3;
        } else if (strcmp(response[0], "ERROR") == 0) {
            free_array(response);
            return 0;
        } else {
            free_array(response);
            return -1;
        }
    } else {
        if (strcmp(response[0], "OK") == 0) {
            free(response[1]);
            response[1] = NULL;
            *result = response;
            return 3;
        } else if (strcmp(response[0], "ERROR") == 0) {
            *result = response;
            return strlen(response[0]) + strlen(response[1]) + 2;
        } else {
            free_array(response);
            return -1;
        }
    }
}

int read_from_server(size_t (*handler)(char *, size_t)) {
    char *buffer = malloc(BUFFER_SIZE);
    size_t buffer_len = 0;
    int header_found = 0;
    while(1) {
        size_t byte_read = read(server_fd, buffer + buffer_len, BUFFER_SIZE - buffer_len - 1);
        if (byte_read == (size_t)-1) {
            if (errno != EINTR) {
                perror("read error");
                free(buffer);
                return -1;
            }
        } else if (byte_read == 0) {
            // server closed connection, no more bytes to read
            free(buffer);
            return 0;
        } else {
            buffer_len += byte_read;
            buffer[buffer_len] = '\0';
            size_t byte_consumed = 0;
            if (header_found == 0) {
                char **result = NULL;
                byte_consumed = parse_header(buffer, &result);
                if (byte_consumed == (size_t)-1) {
                    print_invalid_response();
                    free(buffer);
                    return 0;
                } else if (byte_consumed > 0) {
                    shift_forward(buffer, buffer_len, byte_consumed);
                    buffer_len -= byte_consumed;
                    if (strcmp(result[0], "OK") == 0) {
                        header_found = 1;
                        free_array(result);
                        if (method == GET) {
                            local_fd = open(args[4], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                        }
                    } else {
                        print_error_message(result[1]);
                        free_array(result);
                        free(buffer);
                        return 0;
                    }
                } else {
                    if (buffer_len >= HEADER_SIZE) {
                        print_invalid_response();
                        free(buffer);
                        return 0;
                    }
                }
            } 
            if (handler != NULL) {
                byte_consumed = (*handler)(buffer , buffer_len);
                if (byte_consumed == (size_t)-1) {
                    print_connection_closed();
                    free(buffer);
                    return -1;
                }
                file_read += byte_consumed;
                shift_forward(buffer, buffer_len, byte_consumed);
                buffer_len -= byte_consumed;
            } else {
                print_success();
                free(buffer);
                return 0;
            }
        }
    }
}

size_t list_handler(char *buffer, size_t buffer_len) {
    return write_buffer(buffer, buffer_len, STDOUT_FILENO);
}

size_t get_handler(char *buffer, size_t buffer_len) {
    return write_buffer(buffer, buffer_len, local_fd);
}

size_t write_buffer(char *buffer, size_t buffer_len, int fd) {
    size_t byte_consumed = 0;
    if (file_size == (size_t)-1) {
        if (buffer_len >= 8) {
            file_size = string_to_size(buffer);
            byte_consumed += 8;
            file_read -= 8;
        } else {
            return 0;
        }
    }
    size_t byte_write = write(fd, buffer + byte_consumed, buffer_len - byte_consumed);
    if (byte_write == (size_t)-1) {
        if (errno == EPIPE) {
            print_connection_closed();
            return -1;
        } else if (errno == EINTR) {
            return 0;
        } else {
            perror("write");
            return -1;
        }
    } else if (byte_write > 0) {
        byte_consumed += byte_write;
    }
    return byte_consumed;
}

// free array of strings
void free_array(char **list) {
    char **curr = list;
    while (*curr != NULL) {
        free(*curr);
        ++curr;
    }
    free(list);
}