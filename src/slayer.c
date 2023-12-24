#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <stdbool.h>

// file reads
#include <sys/stat.h>
#include <fcntl.h>

#include <assert.h>

#include "utils.h"
#include "slayer.h"
#include "hlayer.h"

#define LISTEN_BACKLOG 3

int client_interract(int client_fd) {
    char client_sent_buf[MSG_BUF_SIZE] = {0};

    int err = recv(client_fd, client_sent_buf,  MSG_BUF_SIZE, 0);
    return_on_err_condition(err == -1);

    char requested_page[MAX_PAGE_SIZE] = {0};
    err = parse_http_request(client_sent_buf, requested_page);
    return_on_err_condition(err == -1);

    char file_path[MAX_PAGE_SIZE] = {0};
    int allowed = check_allowed_and_add_extension(file_path, requested_page);
    return_on_err_condition(allowed == -1);

    char file_content[MSG_BUF_SIZE] = {0};
    if(allowed) {
        send(client_fd, HTTP_ERROR, sizeof(HTTP_ERROR), 0);
    } else {
        send(client_fd, HTTP_OK, sizeof(HTTP_OK), 0);
    }

    int read_size = read_file(file_content, file_path, MSG_BUF_SIZE);
    return_on_err_condition(read_size > MSG_BUF_SIZE);

    printf("\n\n FILE : \n%s\n", file_content);
    send(client_fd, file_content, read_size, 0);
    return 0;
}

int accept_client(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_size);
    return_on_err_condition(client_fd == -1);

    printf("[log] New connection from %s\n", inet_ntoa(client_addr.sin_addr));

    return client_fd;
}

void client_close(int client_fd) {
    exit_on_err(close(client_fd), "close");
}

void server_close(int server_fd) {
    exit_on_err(close(server_fd), "close");
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

    err = listen(server_fd, LISTEN_BACKLOG);
    exit_on_err(err, "listen");

    printf("[log] Listening on port (%d -> 80)\n", ntohs(server_addr.sin_port));

    return server_fd;
}
