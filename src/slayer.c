#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdbool.h>

// file reads
#include <sys/stat.h>
#include <fcntl.h>

// Parse http_rq
#include <stdio.h>
#include <string.h>

#include "html.h"
#include "utils.h"
#include "slayer.h"

#define LISTEN_BACKLOG 3

#define LOCATION_SIZE 16
#define VERSION_SIZE 10

#define BODY_SIZE MSG_BUF_SIZE - 4 - LOCATION_SIZE - VERSION_SIZE

const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE] = {
        "/",
        "/favicon.ico",
        "/error",
        "/ok",
        "/index",
        "/image"
    };

/*
 * TODO change error return into erno
 */
int parse_http_header (char* buf, char* out_path) {
    // printf("full request : \n\n%s\n", buf);

    char* saveptr_lines;
    char* saveptr_spaces;

    char* lines = strtok_r(buf, "\r\n", &saveptr_lines);

    /* TODO, this should cause a change in response for unsuported method */
    char* spaces = strtok_r(lines, " ", &saveptr_spaces);
    int cmp = strncmp(spaces, "GET", 3);
    return_and_set_erno(cmp != 0, HTTP_METHOD_UNSUPORTED);

    spaces = strtok_r(NULL, " ", &saveptr_spaces);
    cmp = strncmp(spaces, "/", 1);
    return_on_err(cmp != 0, "Request page does not start with '/'");
    strncpy(out_path, spaces, MAX_PAGE_SIZE);

    spaces = strtok_r(NULL, " ", &saveptr_spaces);

    cmp = strncmp(spaces, "HTTP", 4);
    return_and_set_erno(cmp != 0, HTTP_VERSION_UNSUPORTED);
    
    printf("[log] Version : %s\n", spaces);
    #if 0 // iterates through rest of lines
    int i = 0;
    while (lines != NULL && i < 10) {
        lines = strtok_r(NULL, "\r\n", &saveptr_lines);
        printf("[%2d] %s\n", i, lines);
        i += 1;
    }
    #endif

    return HTTP_NO_ERR;
}

int client_interract(int client) {
    char client_sent_buf[MSG_BUF_SIZE] = {0};

    int err = recv(client, client_sent_buf,  MSG_BUF_SIZE, 0);
    return_on_err(err == -1, "Socket receive failed");

    char requested_page[MAX_PAGE_SIZE] = {0};
    err = parse_http_header(client_sent_buf, requested_page);
    if(err == -1) {
        switch (errno) {
            case HTTP_VERSION_UNSUPORTED:
                send(client, HTTP_WRONG_VERSION, sizeof(HTTP_WRONG_VERSION), 0);
                return_on_err(err == -1, "HTTP version Unsuported");
                break;
            case HTTP_METHOD_UNSUPORTED:
                send(client, HTTP_WRONG_METHOD, sizeof(HTTP_WRONG_METHOD), 0);
                return_on_err(err == -1, "HTTP method Unsuported");
                break;
        }
    }
    
    char file_path[MAX_PAGE_SIZE] = {0};
    char file_content[MSG_BUF_SIZE] = {0};
    int allowed_index = check_allowed(allowed_paths, requested_page);

    if(allowed_index == -1) {
        send(client, HTTP_NOT_FOUND, sizeof(HTTP_NOT_FOUND), 0);
        send(client, not_found, sizeof(not_found), 0);
        return 0;
    }
    add_extension(allowed_paths, file_path, allowed_index);
    send(client, HTTP_OK, sizeof(HTTP_OK), 0);

    int read_size = read_file(file_content, file_path, MSG_BUF_SIZE);
    if(read_size == -1) {return -1;};

    err = send(client, file_content, read_size, 0);

    return_on_err(err == -1, "Couldn't send file");
    return 0;
}

int accept_client(int server) {
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    int client = accept(server, (struct sockaddr *)&client_addr, &client_size);
    return_on_err(client == -1, "Could not accept client");

    printf("[log] New connection from %s\n", inet_ntoa(client_addr.sin_addr));

    return client;
}

int client_close(int client) {
    int err = close(client);
    return_on_err(err == -1, "Cloudn't close client");
    return err;
}

void server_close(int server) {
    exit_on_err(close(server), "Cloudn't close server");
}

int server_setup() {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    exit_on_err(server, "socket");

    int option = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY,
    };

    int err = bind(server,(struct sockaddr *) &server_addr, sizeof(server_addr));
    exit_on_err(err, "bind");

    err = listen(server, LISTEN_BACKLOG);
    exit_on_err(err, "listen");

    printf("[log] Listening on port (%d -> 80)\n", ntohs(server_addr.sin_port));

    return server;
}
