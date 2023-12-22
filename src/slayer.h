#ifndef SLAYER_HEADER
#define SLAYER_HEADER



#define HEADER "\
Date: Sun, 10 Dec 2077 12:13:37 CET\r\n\
Server: le_server/1.0 (lenux)\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\r\n\
"

#define HTTP_ERROR "HTTP/1.1 404 Not Found\r\n" HEADER
#define HTTP_OK "HTTP/1.1 200 OK\r\n" HEADER

int server_setup();
int server_accept(int server_fd);
int read_html_file(const char* filename, char* readbuf, const int buf_size);

#endif
