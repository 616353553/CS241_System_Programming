/**
 * Resplendent RPCs Lab
 * CS 241 - Fall 2018
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>

#include "common.h"
#include "dns_query.h"
#include "dns_query_svc_impl.h"

#define CACHE_FILE "cache_files/rpc_server_cache"

char *contact_nameserver(query *argp, char *host, int port) {
    // step 1: Create a socket to communicate through UDP
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket error");
        return NULL;
    }
    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    // step 2: Fill in a `sockaddr_in` with the data for host and port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    struct hostent *serv = gethostbyname(host);
    if (!serv) {
        perror("gethostbyname");
        return NULL;
    }
    memcpy(&addr.sin_addr.s_addr, serv->h_addr, serv->h_length);
    // step 4: send package
    if (argp->host == NULL) {
        return NULL;
    }
    sendto(sock_fd, argp->host, strlen(argp->host), 0,
            (struct sockaddr *)&addr, sizeof(addr));
    // step 5: recieve package
    char *buffer = calloc(MAX_BYTES_IPV4, 1);
    socklen_t addrlen = sizeof(addr);
    ssize_t byte_read = recvfrom(sock_fd, buffer, MAX_BYTES_IPV4 - 1, 0,
                        (struct sockaddr *)&addr, &addrlen);
    if (byte_read <= 0) {
        free(buffer);
        return NULL;
    }
    // step 6:
    if (strcmp(buffer, "-1.-1.-1.-1") == 0) {
        free(buffer);
        return NULL;
    }
    return buffer;
}

void create_response(query *argp, char *ipv4_address, response *res) {
    res->address = malloc(sizeof(host_address));
    char *host = calloc(strlen(argp->host) + 1, 1);
    char *address = calloc(strlen(ipv4_address == NULL ? "" : ipv4_address) + 1, 1);
    if (ipv4_address == NULL) {
        res->success = 0;
        res->address->host = host;
        res->address->host_ipv4_address = address;
    } else {
        strcpy(host, argp->host);
        strcpy(address, ipv4_address);
        res->success = 1;
        res->address->host = host;
        res->address->host_ipv4_address = address;
    }
}

// Stub code

response *dns_query_1_svc(query *argp, struct svc_req *rqstp) {
    printf("Resolving query...\n");
    // check its cache, 'rpc_server_cache'
    // if it's in cache, return with response object with the ip address
    char *ipv4_address = check_cache_for_address(argp->host, CACHE_FILE);
    if (ipv4_address == NULL) {
        // not in the cache. contact authoritative servers like a recursive dns
        // server
        printf(
            "Domain not found in server's cache. Contacting authoritative "
            "servers...\n");
        char *host = getenv("NAMESERVER_HOST");
        int port = strtol(getenv("NAMESERVER_PORT"), NULL, 10);
        ipv4_address = contact_nameserver(argp, host, port);
    } else {
        // it is in the server's cache; no need to ask the authoritative
        // servers.
        printf("Domain found in server's cache!\n");
    }

    static response res;
    xdr_free(xdr_response, &res);  // Frees old memory in the response struct
    create_response(argp, ipv4_address, &res);

    free(ipv4_address);

    return &res;
}
