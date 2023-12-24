#ifndef UTILS_HEADER
#define UTILS_HEADER

#include <errno.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    REQUEST_OK,
    REQUEST_TOKEN_IS_NULL,
    REQUEST_METHOD_IS_NOT_GET,
    REQUEST_NOT_IN_ALLOWLIST,
    REQUEST_PROBLEM_WITH_STR,
    REQUEST_HTTP_VERSION_UNSUPORTED,
} request_parse_error;


#define MAX_PAGE_SIZE 24

#define check_error(X) {\
    int __val = X;\
    if(__val == -1) { \
    fprintf(stderr, "ERROR (%s:%d) -- %s\n",__FILE__,__LINE__, strerror(errno));\
    }\
}

#define RETURN_ON_ERROR(X) {\
    int __val = X;\
    if(__val == -1) { \
    fprintf(stderr, "ERROR (%s:%d) -- %s\n",__FILE__,__LINE__, strerror(errno));\
    return -1;\
    }\
}

/* FUNCTIONS */
int check_allowed_and_add_extension(char* out_path, const char* requested);
int read_file(char* readbuf, const char* filename, const int max_readsize);

#endif
