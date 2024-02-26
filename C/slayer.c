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

int check_allowed(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], http_request* rq) {
    mc_debug_print("[log] Requested Path : '%s'\n", rq->url);

    for (int i = 0; i < NUM_PAGES; i++) {
        if(strncmp(rq->url, allowed_paths[i], MAX_PAGE_SIZE) == 0) {
            return i;
        }
    }
    return -1;
}

int _parse_first_http_line(char* recv_buf, http_request* rq) {
    // mc_debug_print("full line : \n\n%s\n", recv_buf);

    char* token = strtok(recv_buf, " ");
    return_on_err(token == NULL, "Couldn't tokenize");

    int cmp = strncmp(token, "GET", 3);
    return_on_err(cmp != 0, "Request method whas not GET");
    rq->method = GET;

    token = strtok(NULL, " ");
    return_on_err(token == NULL, "Couldn't tokenize");

    memcpy(rq->url, token, MAX_PAGE_SIZE);

    rq->content_type = strncmp(rq->url, "/images/", 8) ? HTML : IMAGE;
    return 0;
}

int client_interract(int client) {
    char client_sent_buf[RECV_BUF_SIZE] = {0};

    int err = recv(client, client_sent_buf,  RECV_BUF_SIZE, 0);
    return_on_err(err == -1, "Socket receive failed");

    http_request rq = {0};
    _parse_first_http_line(client_sent_buf, &rq);

    int allowed_index = check_allowed(allowed_paths, &rq);
    if (allowed_index == -1) {
        log_msg("Path not allowed");
        send(client, HTTP_NOT_FOUND_404, sizeof(HTTP_NOT_FOUND_404), 0);
        return 0;
    }

    char file_content[MSG_BUF_SIZE] = {0};
    err = add_extension(allowed_paths, &rq, allowed_index);
    if (err == -1) {
        log_msg("Couldn't add extension");
        send(client, HTTP_NOT_FOUND_404, sizeof(HTTP_NOT_FOUND_404), 0);
        return 0;
    }
    
    err = build_response(file_content, MSG_BUF_SIZE, &rq);
    if (err == -1) {
        log_msg("Couldn't build response");
        send(client, HTTP_NOT_FOUND_404, sizeof(HTTP_NOT_FOUND_404), 0);
        return 0;
    }

    send(client, file_content, sizeof(file_content), 0);
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
    };

    int err = bind(server,(struct sockaddr *) &server_addr, sizeof(server_addr));
    exit_on_err(err, "bind");

    err = listen(server, LISTEN_BACKLOG);
    exit_on_err(err, "listen");

    printf("Listening on port %d\n", ntohs(server_addr.sin_port));

    return server;
}

int add_extension(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], http_request* rq, int i) {

    // TODO do this witout assuming position 1 of allowed_path is "/"
    char* cp_err = (i == 0) ?
        strncpy(rq->url, "/index", MAX_PAGE_SIZE)
        : strncpy(rq->url, allowed_paths[i], MAX_PAGE_SIZE); 


    if(rq->content_type == HTML) {
        char* cat_err = strncat(rq->url, ".html", MAX_PAGE_SIZE);
        return_on_err(cat_err == NULL, "Error on strfunc");
    }

    printf("url <%s>, index: %d, cp_err %s\n", rq->url, i, cp_err);

    return_on_err(cp_err == NULL, "Error on strfunc");
    return 0;
}

int read_file(char* out_buf, const char* path, const int max_size)
{
    FILE* file = fopen(path, "r");
    return_on_err(file == NULL, "Couldn't open file");

    int err = fread(out_buf, sizeof(unsigned char), max_size, file);
    return_on_err(err == -1, "Couldn't read file");
    mc_debug_print("Actual read : %d", err);

    return_on_err(fclose(file) == -1, "Couldn't Close file");

    return err;
}

int build_response(char* out_buf, int max_size, const http_request* rq)
{
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, rq->url, MAX_PAGE_SIZE);
    return_on_err(path == NULL, "Couldn't append to path");

    struct stat st;
    int err = stat(path, &st);
    return_on_err(err == -1, "Couldn't stat file");

    int header_size, new_start;
    if(rq->content_type == IMAGE)
    {
        const char* image_type;

        if(strstr(rq->url, ".webp") != NULL){
            image_type = "image/webp";
        }
        else if(strstr(rq->url, ".png") != NULL) {
            image_type = "image/png";
        }
        else if(strstr(rq->url, ".svg") != NULL) { 
            image_type = "image/svg+xml";
        } else {
            return_on_err(err == -1, "Unknown image type");
        }

        header_size = sizeof(HTTP_IMAGE_HEADER_FSTR) - 5 + strlen(image_type) + strlen("9999999");
        new_start = snprintf(out_buf, header_size, HTTP_IMAGE_HEADER_FSTR, image_type, st.st_size);
    }
    else
    {
        header_size = sizeof(HTTP_OK);
        new_start = snprintf(out_buf, header_size, HTTP_OK);
    }
    return_on_err(new_start == -1, "Couldn't snprintf file");

    int remaining_space = max_size - header_size;
    return_on_err(new_start + st.st_size > max_size, "file wouldn't fit in buffer");

    if(rq->content_type != IMAGE)
    {
        int bytes_writen = read_file(&out_buf[new_start], "./files/nav.html", remaining_space);
        return_on_err(bytes_writen == -1, "Coulnd't write nav");

        remaining_space -= bytes_writen;
        new_start += bytes_writen;
    }

    err = read_file(&out_buf[new_start], path, remaining_space);

    return err;
}

