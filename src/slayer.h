#ifndef SLAYER_HEADER
#define SLAYER_HEADER


/* SETTINGS */

#define MSG_BUF_SIZE 500000
#define RECV_BUF_SIZE 2048 

#define MAX_PAGE_SIZE 24
#define NUM_PAGES 5


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
    // HEAD,
} method;

typedef struct http_request {
    method method;
    content_type content_type;
    char page[MAX_PAGE_SIZE];
} http_request;


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

// Content-Length: 330531\r\n\r\n"




int server_setup();
void server_close(int server);
int client_close(int server);

int client_interract(int client);
int accept_client(int server);
int read_html_file(const char* filename, char* readbuf, const int buf_size);

#endif
