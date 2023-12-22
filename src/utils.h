#include <errno.h>

#ifndef UTILS_HEADER
#define UTILS_HEADER

/* SETTINGS */

#define MAX_PAGE_SIZE 16
#define BUFF_SIZE 1024

/* MACROS */

#define exit_on_err(err, msg) if(err==-1) { perror(msg); exit(EXIT_FAILURE); };
#define RETURN_ON_ERROR(X) {\
    int __val = X;\
    if(__val == -1) { \
    fprintf(stderr, "ERROR (%s:%d) -- %s\n",__FILE__,__LINE__, strerror(errno));\
    return -1;\
    }\
}

#define log(...) printf(__VA_ARGS__)


#define check_error(X) {\
    int __val = X;\
    if(__val == -1) { \
    fprintf(stderr, "ERROR (%s:%d) -- %s\n",__FILE__,__LINE__, strerror(errno));\
    }\
}

/* FUNCTIONS */
int read_file(char* readbuf, const char* filename);

#endif
