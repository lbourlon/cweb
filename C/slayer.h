#ifndef SLAYER_HEADER
#define SLAYER_HEADER

#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* SETTINGS */

#define MSG_BUF_SIZE 500000
#define RECV_BUF_SIZE 2048 
#define RECV_BUF_SINGLE_LINE_SIZE 128

#define MAX_PAGE_SIZE 24
#define NUM_PAGES 7


typedef enum {
    HTTP_NO_ERR,
    HTTP_VERSION_UNSUPORTED,
    HTTP_METHOD_UNSUPORTED,
    HTTP_BAD_PARSE
} cweb_err;

typedef enum {
    HTML,
    IMAGE,
    // TEXT
} content_type;

typedef enum {
    GET,
    HEAD,
} method;

typedef struct http_request {
    method method;
    content_type content_type;
    char url[MAX_PAGE_SIZE];
} http_request;

/* Socket Related */
int server_setup();
void server_close(int server);
int client_close(int server);
int client_interract(int client);
int accept_client(int server);

/* HTTP HTML */
int check_allowed(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], http_request* rq);
int add_extension(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], http_request* rq, int i);
int build_response(char* out_buf, int max_size, const http_request* rq);

int _parse_first_http_line(char* recv_buf, http_request* rq);

/* HTTP STUFF */

#define HEADER "\
Date: Sun, 10 Dec 2077 12:13:37 CET\r\n\
Server: le_server/1.0 (lenux)\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\r\n\
"

#define HTTP_WRONG_METHOD "HTTP/1.1 501 Not Implemented\r\n" HEADER
#define HTTP_WRONG_VERSION "HTTP/1.1 505 HTTP Version Not Supported\r\n" HEADER

#define HTTP_OK "HTTP/1.1 200 OK\r\n" HEADER

#define HTTP_NOT_FOUND_404 "HTTP/1.1 404 Not Found\r\n" HEADER"\
<html><body>\
<title>This is cweb.</title>\
<h1>404 Page Not Found</h1>\
<p>The requested resource was not found on cweb.</p>\
</body></html>"


#define HTTP_IMAGE_HEADER_FSTR "HTTP/1.1 200 OK\r\n\
Date: Sun, 10 Dec 2077 12:13:37 CET\r\n\
Server: le_server/1.0 (lenux)\r\n\
Content-Type: %s\r\n\
Content-Length: %ld\r\n\r\n"


#ifdef DEBUG_PRINT
#define mc_debug_print(...) printf(__VA_ARGS__)
#else
#define mc_debug_print(...) //
#endif

#define exit_on_err(err, msg) if(err==-1) { perror(msg); exit(EXIT_FAILURE); };

#define return_on_err(condition, msg) \
    if(condition) { \
        fprintf(stderr, "[ERROR] n°%d @%s:%d -- %s\n",errno, __FILE__,__LINE__, msg);\
        return -1;}


#define log_msg(msg) \
    fprintf(stderr, "[ERROR] n°%d @%s:%d -- %s\n",errno, __FILE__,__LINE__, msg);\

#define return_and_set_erno(condition, err_no) if(condition) { errno = err_no;  return -1;}

#define log(...) printf(__VA_ARGS__)

#endif
