#ifndef UTILS_HEADER
#define UTILS_HEADER

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "slayer.h"


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

#define return_and_set_erno(condition, err_no) if(condition) { errno = err_no;  return -1;}

#define log(...) printf(__VA_ARGS__)

/* FUNCTIONS */
int check_allowed(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], const char* requested);

int add_extension(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], char* out_path, int i);

int read_file(char* readbuf, const char* filename, const int max_readsize);

int build_response(char* out_buf, int max_size, const http_request* rq);
#endif