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
    "/index",
    "/rick",
    "/ok",
    "/perf",
    "/images/rick.webp",
    "/images/perf.svg"
};

const char* index_html =
    HTTP_OK
    base_start 
    "<h1>does this work in c?</h1>"
    "<p>This is cweb.</p>"
    base_end;

int parse_http_header(char* in_buffer, http_request* rq) {
    mc_debug_print("full request : \n\n%s\n", in_buffer);

    char* saveptr_lines;
    char* saveptr_spaces;

    char* lines = strtok_r(in_buffer, "\r\n", &saveptr_lines);
    return_and_set_erno(lines == NULL, HTTP_BAD_PARSE);

    /* TODO, this should cause a change in response for unsuported method */
    char* spaces = strtok_r(lines, " ", &saveptr_spaces);
    return_and_set_erno(spaces == NULL, HTTP_BAD_PARSE);

    int cmp = strncmp(spaces, "GET", 3);
    return_and_set_erno(cmp != 0, HTTP_METHOD_UNSUPORTED);
    rq->method = GET;

    spaces = strtok_r(NULL, " ", &saveptr_spaces);
    return_and_set_erno(spaces == NULL, HTTP_BAD_PARSE);

    printf("'%c' == '%c'\n", spaces[0], '/');
    return_and_set_erno(spaces == NULL, HTTP_BAD_PARSE);

    cmp = strncmp(spaces, "/images/", 8);
    if(cmp == 0) {
        rq->content_type = IMAGE;
    } else {
        rq->content_type = HTML;
    }

    memcpy(rq->page, spaces, MAX_PAGE_SIZE);

    spaces = strtok_r(NULL, " ", &saveptr_spaces);
    return_and_set_erno(spaces == NULL, HTTP_BAD_PARSE);

    cmp = strncmp(spaces, "HTTP", 4);
    return_and_set_erno(cmp != 0, HTTP_VERSION_UNSUPORTED);
    
    mc_debug_print("Version : %s\n", spaces);

    #if 0 // iterates through rest of lines
    int i = 0;
    while (lines != NULL && i < 10) {
        lines = strtok_r(NULL, "\r\n", &saveptr_lines);

        return_and_set_erno(lines == NULL, HTTP_BAD_PARSE);
        mc_debug_print("[%2d] %s\n", i, lines);
        i += 1;
    }
    #endif

    return HTTP_NO_ERR;
}

int client_interract(int client) {
    char client_sent_buf[RECV_BUF_SIZE] = {0};
    int err = recv(client, client_sent_buf,  RECV_BUF_SIZE, 0);
    return_on_err(err == -1, "Socket receive failed");

    http_request rq = {
        .method = GET,
        .content_type = HTML
    };
    memset(rq.page, 0, sizeof(rq.page));

    err = parse_http_header(client_sent_buf, &rq);
    if(err == -1) {
        switch (errno) {
            case HTTP_VERSION_UNSUPORTED:
                send(client, HTTP_WRONG_VERSION, sizeof(HTTP_WRONG_VERSION), 0);
                return_on_err(err == -1, "HTTP version Unsuported !");
                break;
            case HTTP_METHOD_UNSUPORTED:
                send(client, HTTP_WRONG_METHOD, sizeof(HTTP_WRONG_METHOD), 0);
                return_on_err(err == -1, "HTTP method Unsuported !");
                break;
            case HTTP_BAD_PARSE:
                return_on_err(err == -1, "Problem with HTTP parse !");
                break;
            default:
                return_on_err(err == -1, "Killing connection by default !");
        }
    }
    
    int allowed_index = check_allowed(allowed_paths, rq.page);

    if(allowed_index == -1) {
        send(client, HTTP_NOT_FOUND_404, sizeof(HTTP_NOT_FOUND_404), 0);
        return 0;
    }

    // if(allowed_index == 0) {
    //     send(client, index_html, strlen(index_html) * sizeof(index_html[0]), 0);
    //     return 0;
    // }

    char file_content[MSG_BUF_SIZE] = {0};
    add_extension(allowed_paths, rq.page, allowed_index);

    mc_debug_print("[log] building response\n");
    err = build_response(file_content, MSG_BUF_SIZE, &rq);
    return_on_err(err == -1, "Couldn't build response");

    send(client, file_content, sizeof(file_content), 0);
    // mc_debug_print("content : %s\n\n", file_content);
    return 0;
}

int accept_client(int server) {
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    int client = accept(server, (struct sockaddr *) &client_addr, &client_size);
    return_on_err(client == -1, "Could not accept client");

    mc_debug_print("New connection from %s\n", inet_ntoa(client_addr.sin_addr));

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

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = INADDR_ANY,
    };

    int err = bind(server,(struct sockaddr *) &server_addr, sizeof(server_addr));
    exit_on_err(err, "bind");

    err = listen(server, LISTEN_BACKLOG);
    exit_on_err(err, "listen");

    printf("Listening on port %d\n", ntohs(server_addr.sin_port));

    return server;
}
