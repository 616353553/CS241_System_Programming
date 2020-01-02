/**
 * Networking Lab
 * CS 241 - Fall 2018
 */

#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include "common.h"
#include "format.h"
#include "vector.h"
#include "dictionary.h"
#include "server.h"

#define MAX_CLIENTS 128
#define HEADER_SIZE 1024
#define BUFFER_SIZE 2048

typedef struct client_info_t {
    int header_found;
    char *buffer;
    size_t buffer_len;
    int sock_fd;
    verb method;
    char *file_name;
    int file_fd;
    size_t file_size;
    size_t byte_read;
    int finished;
} client_info;

void signal_handler(int signal);
int create_server_socket(char *port);
int read_from_client(client_info *data);
int parse_header(client_info *data);
size_t handle_put(client_info *data);
size_t handle_get(client_info *data);
size_t handle_list(client_info *data);
size_t get_file_index(char *file_name);
int delete_file(char *file_name);
int respond_header(int sock_fd, int failed, const char *error_mesg);
size_t write_to_socket(int sock_fd, char *buffer, size_t size, int strict);
void shift_buffer_forward(client_info *data, size_t offset);
void clean_up();

static vector *file_names = NULL;
static dictionary *clients = NULL;
static char *temp_dir = NULL;


void print_buffer(char *buffer, size_t buffer_size) {
    puts("=== printing buffer ===");
    for (size_t i = 0; i < buffer_size; ++i) {
        printf("buffer[%zu]: '%c'(%d)\n", i, buffer[i], (unsigned int)buffer[i]);
    }
    puts("=======================");
}

void print_data(client_info *data) {
    puts("=============================");
    printf("header_found: %d\n", data->header_found);
    printf("buffer head: %p\n", data->buffer);
    printf("buffer len: %zu\n", data->buffer_len);
    printf("sock_fd: %d\n", data->sock_fd);
    puts("=============================");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("./server <port>\n");
        exit(1);
    }

    char template[] = "XXXXXX";
    temp_dir = mkdtemp(template);
    if (temp_dir == NULL) {
        perror("mkdtemp");
        exit(1);
    }
    print_temp_directory(temp_dir);
    // change directory to temp_dir
    if (chdir(temp_dir)) {
        perror("chdir");
        rmdir(temp_dir);
        exit(1);
    }
    // register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    file_names = string_vector_create();
    clients = int_to_shallow_dictionary_create();

    int server_fd = create_server_socket(argv[1]);
    if (server_fd == -1) {
        clean_up();
        exit(1);
    }
    
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen");
        clean_up();
        exit(1);
    }

    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll");
        clean_up();
        exit(1);
    }
    struct epoll_event temp, events[MAX_CLIENTS];
    // set epoll to be "edge triggered" listener
    memset(&temp, 0, sizeof(struct epoll_event));
    temp.events = EPOLLIN | EPOLLET;
    temp.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &temp) == -1) {
        perror("epoll_ctl error");
        clean_up();
        exit(1);
    }

    while (1) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_CLIENTS, -1);
        if (num_fds == -1) {
            if (errno != EINTR) {
                perror("epoll_wait");
                clean_up();
                exit(1);
            }
        }
        for (int i = 0; i < num_fds; ++i) {
            // when server_fd get trigered, which means new conectinos incoming
            if (events[i].data.fd == server_fd) {
                while (1) {
                    int client_fd = accept(server_fd, NULL, NULL);
                    if (client_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            perror("accept");
                            clean_up();
                            exit(1);
                        }
                    }
                    // set client_fd to be non-blocking
                    int flags = fcntl(client_fd, F_GETFL);
                    if (flags == -1 || fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
                        perror("fcntl error");
                        clean_up();
                        exit(1);
                    }
                    // add client_fd to epoll so that it will be listened
                    temp.events = EPOLLIN | EPOLLET;
                    temp.data.fd = client_fd;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &temp) == -1) {
                        perror("epoll_ctl");
                        clean_up();
                        exit(1);
                    }
                    client_info *data = malloc(sizeof(client_info));
                    data->header_found = 0;
                    data->buffer = malloc(BUFFER_SIZE);
                    data->buffer_len = 0;
                    data->sock_fd = client_fd;
                    data->method = V_UNKNOWN;
                    data->file_name = NULL;
                    data->file_fd = -1;
                    data->file_size = -1;
                    data->byte_read = 0;
                    data->finished = 0;
                    dictionary_set(clients, &client_fd, data);
                }
            }
            // when fd other than server_fd get triggered, which means new data incoming
            else {
                int client_fd = events[i].data.fd;
                client_info *data = dictionary_get(clients, &client_fd);
                if (read_from_client(data) == -1) {
                    clean_up();
                    exit(1);
                }
                // remove from epoll
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1) {
                    perror("epoll_ctl");
                    clean_up();
                    exit(1);
                } else {
                    shutdown(client_fd, SHUT_RDWR);
                    close(client_fd);
                    free(data->buffer);
                    free(data->file_name);
                    if (data->file_fd != -1) {
                        close(data->file_fd);
                    }
                    free(data);
                    dictionary_remove(clients, (void *)&client_fd);
                }
            }
        }
    }
    close(server_fd);
    // TODO: remove directories
    vector_destroy(file_names);
    return 0;
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        clean_up();
        exit(1);
    }
}

int create_server_socket(char *port) {
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sock_fd == -1) {
        perror("socket");
        return -1;
    }
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int retval = getaddrinfo(NULL, port, &hints, &result);
    if (retval != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
        close(sock_fd);
        freeaddrinfo(result);
        return -1;
    }
    // make port reuseable
    int opt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind");
        close(sock_fd);
        freeaddrinfo(result);
        return -1;
    }
    freeaddrinfo(result);
    return sock_fd;
}

// return -1 when error occured
// return 0 otehrwise
int read_from_client(client_info *data) {
    while (1) {
        size_t byte_read = read(data->sock_fd, data->buffer + data->buffer_len, 
                                BUFFER_SIZE - data->buffer_len - 1);
        if (byte_read == (size_t)-1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            } else {
                perror("read");
                clean_up();
                exit(1);
            }
        } else if (byte_read > 0) {
            data->byte_read += byte_read;
            data->buffer_len += byte_read;
            data->buffer[data->buffer_len] = '\0';
            // if header is not parsed yet
            if (data->header_found == 0) {
                size_t header_len = parse_header(data);
                if (header_len == (size_t)-1) {
                    respond_header(data->sock_fd, 1, err_bad_request);
                    return 0;
                } else if (header_len > 0) {
                    data->header_found = 1;
                    shift_buffer_forward(data, header_len);
                    data->byte_read -= header_len;
                } else {
                    continue;
                }
            }
            // handle binary bytes after handling header
            size_t byte_consumed = 0;
            if (data->method == PUT) {
                byte_consumed = handle_put(data);
                if (byte_consumed == (size_t)-1) {
                    return -1;
                } else {
                    shift_buffer_forward(data, byte_consumed);
                }
            } else if (data->method == V_UNKNOWN) {
                respond_header(data->sock_fd, 1, err_bad_request);
                return -1;
            } else {
                if (data->buffer_len > 0) {
                    respond_header(data->sock_fd, 1, err_bad_request);
                    return -1;
                }
                if (data->method == GET) {
                    if (handle_get(data) == (size_t)-1) {
                        return -1;
                    } else {
                        return 0;
                    }
                } else if (data->method == LIST) {
                    if (handle_list(data) == (size_t)-1) {
                        return -1;
                    } else {
                        return 0;
                    }
                } else {
                    if (delete_file(data->file_name) == -1) {
                        respond_header(data->sock_fd, 1, err_no_such_file);
                    } else {
                        respond_header(data->sock_fd, 0, NULL);
                    }
                    return 0;
                }
            }
        } else {
            data->finished = 1;
            if (data->method == PUT) {
                if (data->byte_read != data->file_size + 8) {
                    if (data->byte_read > data->file_size + 8) {
                        print_received_too_much_data();
                    } else {
                        print_too_little_data();
                    }
                    // remove the file
                    close(data->file_fd);
                    unlink(data->file_name);
                    size_t file_index = get_file_index(data->file_name);
                    if (file_index != (size_t)-1) {
                        vector_erase(file_names, file_index);
                    }
                    respond_header(data->sock_fd, 1, err_bad_file_size);
                } else {
                    respond_header(data->sock_fd, 0, NULL);
                }
            } else {
                respond_header(data->sock_fd, 0, NULL);
            }
            return 0;
        }
    }
}


// returns the length of header (including '\n')
// 0 when header need more bytes
// -1 when header is invalid/error occured
int parse_header(client_info *data) {
    // make a clone of the buffer content
    char clone[HEADER_SIZE];
    memcpy(clone, data->buffer, HEADER_SIZE);
    char *header_end = strstr(clone, "\n");
    // if '\n' not found, then more bytes are needed..
    if (header_end == NULL) {
        if (data->buffer_len >= HEADER_SIZE) {
            return -1;
        } else {
            return 0;   
        }
    } else if (header_end - clone + 1 > HEADER_SIZE) {
        return -1;
    }
    *header_end = '\0';
    // split strings
    char *method = strtok(clone, " ");
    char *file_name = strtok(NULL, " ");
    char *trap = strtok(NULL, " ");
    // invalid header
    if (method == NULL || trap != NULL) {
        return -1;
    } else {
        data->method = str_to_verb(method);
        if (data->method == V_UNKNOWN) {
            return -1;
        } else if (data->method == LIST) {
            if (file_name != NULL) {
                return -1;
            }
        } else {
            if (file_name == NULL) {
                return -1;
            } else {
                data->file_name = malloc(strlen(file_name) + 1);
                strcpy(data->file_name, file_name);
            }
        }
    }
    return header_end - clone + 1;
}


size_t handle_put(client_info *data) {
    //open file descriptor if possible
    if (data->file_fd == -1) {
        data->file_fd = open(data->file_name, O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (data->file_fd < 0) {
            perror("open");
            return -1;
        }
        if (get_file_index(data->file_name) == (size_t)-1) {
            vector_push_back(file_names, data->file_name);
        }
    }
    // handle binary data
    size_t byte_consumed = 0;
    // get file size if neccessary
    if (data->file_size == (size_t)-1) {
        if (data->buffer_len < 8) {
            return 0;
        } else {
            data->file_size = string_to_size(data->buffer);
            byte_consumed += 8;
        }
    }
    // write to file
    size_t byte_write = write(data->file_fd, data->buffer + byte_consumed, 
                            data->buffer_len - byte_consumed);
    if (byte_write == (size_t)-1 && errno != EINVAL) {
        perror("write");
        return -1;
    }
    return byte_consumed + byte_write;
}


size_t handle_get(client_info *data) {
    if (get_file_index(data->file_name) == (size_t)-1) {
        respond_header(data->sock_fd, 1, err_no_such_file);
        return 0;
    } else {
        data->file_fd = open(data->file_name, O_RDONLY);
        if (data->file_fd  < 0) {
            perror("open");
            return -1;
        }
        respond_header(data->sock_fd, 0, NULL);
        // get file size
        struct stat st;
        stat(data->file_name, &st);
        char *size_str = size_to_string(st.st_size);
        memcpy(data->buffer, size_str, 8);
        data->buffer_len = 8;
        free(size_str);
        // write file content
        while (1) {
            size_t byte_read = read(data->file_fd, data->buffer + data->buffer_len,
                                    BUFFER_SIZE - data->buffer_len - 1);
            if (byte_read == (size_t)-1) {
                if (errno != EINTR) {
                    close(data->file_fd);
                    return -1;
                }
            } else if (byte_read > 0) {
                data->buffer_len += byte_read;
                data->buffer[data->buffer_len] = '\0';
                size_t byte_write = write_to_socket(data->sock_fd, data->buffer, data->buffer_len, 0);
                if (byte_write == (size_t)-1) {
                    close(data->file_fd);
                    return -1;
                } else if (byte_write > 0) {
                    shift_buffer_forward(data, byte_write);
                }
            } else {
                size_t byte_write = write_to_socket(data->sock_fd, data->buffer, data->buffer_len, 1);
                if (byte_write == (size_t)-1) {
                    return -1;
                } else {
                    return 0;
                }
            }
        }
    }
}



size_t handle_list(client_info *data) {
    respond_header(data->sock_fd, 0, NULL);
    size_t num_files = vector_size(file_names);
    size_t total_bytes = 0;
    for (size_t i = 0; i < num_files; ++i) {
        total_bytes += strlen(vector_get(file_names, i));
        if (i + 1 != num_files) {
            total_bytes += 1;
        }
    }
    size_t i = 0;
    char *size_str = size_to_string(total_bytes);
    memcpy(data->buffer, size_str, 9);
    data->buffer_len = 8;
    free(size_str);
    while (i < num_files) {
        char *file_name = vector_get(file_names, i);
        if ((BUFFER_SIZE - data->buffer_len) > (strlen(file_name) + 1)) {
            strcpy(data->buffer + data->buffer_len, file_name);
            data->buffer_len += strlen(file_name);
            if (i + 1 != num_files) {
                data->buffer[data->buffer_len++] = '\n';
            }
            data->buffer[data->buffer_len] = '\0';
            ++i;
        } else {
            size_t byte_write = write_to_socket(data->sock_fd, data->buffer, data->buffer_len, 0);
            if (byte_write == (size_t)-1) {
                return -1;
            }
            shift_buffer_forward(data, byte_write);
        }
    }
    size_t byte_write = write_to_socket(data->sock_fd, data->buffer, data->buffer_len, 1);
    if (byte_write == (size_t)-1) {
        return -1;
    }
    shift_buffer_forward(data, byte_write);
    return 0;
}


size_t get_file_index(char *file_name) {
    for (size_t i = 0; i < vector_size(file_names); ++i) {
        if (strcmp((char *)vector_get(file_names, i), file_name) == 0) {
            return i;
        }
    }
    return (size_t)-1;
}

int delete_file(char *file_name) {
    size_t index = get_file_index(file_name);
    if (index != (size_t)-1) {
        vector_erase(file_names, index);
        unlink(file_name);
        return 0;
    }
    return -1;
}

int respond_header(int sock_fd, int failed, const char *error_mesg) {
    char buffer[HEADER_SIZE + 1];
    if (failed == 0) {
        sprintf(buffer, "OK\n");
    } else {
        sprintf(buffer, "ERROR\n%s\n", error_mesg);
    }
    return write_to_socket(sock_fd, buffer, strlen(buffer), 1);
}


size_t write_to_socket(int sock_fd, char *buffer, size_t size, int strict) {
    size_t byte_write_total = 0;
    while (byte_write_total < size) {
        size_t byte_write = write(sock_fd, buffer + byte_write_total, size - byte_write_total);
        if (byte_write == (size_t)-1) {
            if ((errno == EAGAIN || errno == EWOULDBLOCK) && strict == 0) {
                return byte_write_total;
            } else if (errno != EINTR) {
                perror("write");
                return -1;
            }
        } else if (byte_write > 0) {
            byte_write_total += byte_write;
        }
    }
    return byte_write_total;
}

void shift_buffer_forward(client_info *data, size_t offset) {
    shift_forward(data->buffer, data->buffer_len, offset);
    data->buffer_len -= offset;
}

void clean_up() {
    if (temp_dir != NULL) {
        size_t num_files = vector_size(file_names);
        for (size_t i = 0; i < num_files; ++i) {
            unlink(vector_get(file_names, i));
        }
        chdir("..");
        rmdir(temp_dir);
    }
    if (file_names != NULL) {
        vector_destroy(file_names);
    }
    if (clients != NULL) {
        vector *keys = dictionary_keys(clients);
        size_t num_keys = vector_size(keys);
        for (size_t i = 0; i < num_keys; ++i) {
            client_info *info = dictionary_get(clients, vector_get(keys, i));
            free(info->buffer);
            free(info->file_name);
            free(info);
        }
        vector_destroy(keys);
        dictionary_destroy(clients);
    }
}