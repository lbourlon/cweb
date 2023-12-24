#ifndef SLAYER_HEADER
#define SLAYER_HEADER


/* SETTINGS */

#define MSG_BUF_SIZE 1024

/* HTTP STUFF */

#define HEADER "\
Date: Sun, 10 Dec 2077 12:13:37 CET\r\n\
Server: le_server/1.0 (lenux)\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\r\n\
"

#define HTTP_ERROR "HTTP/1.1 404 Not Found\r\n" HEADER
#define HTTP_OK "HTTP/1.1 200 OK\r\n" HEADER

/* MACROS */

#define exit_on_err(err, msg) if(err==-1) { perror(msg); exit(EXIT_FAILURE); };

#define log(...) printf(__VA_ARGS__)




int server_setup();
void server_close(int server_fd);
void client_close(int server_fd);

int client_interract(int client);
int accept_client(int server_fd);
int read_html_file(const char* filename, char* readbuf, const int buf_size);

#endif
