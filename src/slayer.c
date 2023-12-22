#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <string.h>
#include <stdbool.h>

// file reads
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"
#include "slayer.h"
#include "hlayer.h"

#define LISTEN_BACKLOG 3
#define NUM_PAGES 5

// TODO moved int a wlayer
int check_allowed_and_add_extension(char* out_path, const char* requested) {
    printf("[log] Requested Path : '%s'\n", requested);

    char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE] = {
        "/",
        "/favicon.ico",
        "/error",
        "/ok",
        "/index"
    };

    bool allowed = false;
    int p = 0;
    while (p < NUM_PAGES && allowed != true) {
        if(strncmp(requested, allowed_paths[p], MAX_PAGE_SIZE) == 0) {
            allowed = true;
            break;
        }
        p += 1;
    }

    if (allowed == false) {
        printf("[ERROR] Requested not in list !\n");
        return REQUEST_NOT_IN_ALLOWLIST;
    }

    // TODO : make clearear max_page_size, with or without extension
    char* cp_err; char* cat_err;
    if (p == 0) {
        cp_err = strncpy(out_path, "/index.html", MAX_PAGE_SIZE);
    } else if (p == 1) {
        cp_err = strncpy(out_path, "/favicon.ico", MAX_PAGE_SIZE);
    } else {
        cp_err = strncpy(out_path, allowed_paths[p], MAX_PAGE_SIZE);
        cat_err = strncat(out_path, ".html", MAX_PAGE_SIZE);
    }

    if (cp_err == NULL || cat_err == NULL) { return REQUEST_PROBLEM_WITH_STR; }

    return REQUEST_OK;
}


int server_accept(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_size);

    exit_on_err(client_fd, "accept error");

    printf("[log] New connection from %s\n", inet_ntoa(client_addr.sin_addr));

    char client_sent_buf[BUFF_SIZE] = {0};
    int err = recv(client_fd, client_sent_buf,  BUFF_SIZE, 0);
    exit_on_err(err, "receive error");

    char requested_page[MAX_PAGE_SIZE] = {0};
    int parse_err = parse_http_request(client_sent_buf, requested_page);

    // TODO handle this gracefully
    if(parse_err != 0) {
        printf("[log] Could not parse HTTP request\n"); 
    }

    char file_path[MAX_PAGE_SIZE] = {0};
    int allowed = check_allowed_and_add_extension(file_path, requested_page);

    char file_content[BUFF_SIZE] = {0};
    int read_size = 0;
    if(allowed != 0) {
        read_size = read_file(file_content, "not_found.html");
    } else {
        read_size = read_file(file_content, file_path);
    }

    if(read_size == -1){
        printf("[ERROR] Couldn't read file for some reason, TODO"); //TODO
        send(client_fd, HTTP_ERROR, sizeof(HTTP_ERROR), 0);
        // NOTE : Sending HTTP_OK for now because browser stalls otherwise
        // send(client_fd, HTTP_OK, sizeof(HTTP_OK), 0);
    } else {
        send(client_fd, HTTP_OK, sizeof(HTTP_OK), 0);
    }
    send(client_fd, file_content, BUFF_SIZE, 0);

    err = close(client_fd);
    exit_on_err(close(server_fd), "close");
    return 0;
}

int server_setup() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    exit_on_err(server_fd, "socket");

    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY,
    };

    int err = bind(server_fd,(struct sockaddr *) &server_addr, sizeof(server_addr));
    exit_on_err(err, "bind");

    RETURN_ON_ERROR(listen(server_fd, LISTEN_BACKLOG));
    printf("[log] Listening on port (%d -> 80)\n", ntohs(server_addr.sin_port));

    return server_fd;
}
