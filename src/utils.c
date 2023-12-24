#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"

#define NUM_PAGES 6

int check_allowed_and_add_extension(char* out_path, const char* requested) {
    printf("[log] Requested Path : '%s'\n", requested);

    char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE] = {
        "/",
        "/favicon.ico",
        "/error",
        "/ok",
        "/index",
        "/image"
    };

    bool allowed = false;
    int p = 0;
    while (p < NUM_PAGES && allowed != true) {
        if(strncmp(requested, allowed_paths[p], MAX_PAGE_SIZE) == 0) {
            allowed = true;
            break;
        }
        p += 1;
    }

    char* cp_err; char* cat_err;
    if (allowed == false) {
        cp_err = strncpy(out_path, "/not_found.html", MAX_PAGE_SIZE);
    } else if (p == 0) {
        cp_err = strncpy(out_path, "/index.html", MAX_PAGE_SIZE);
    } else if (p == 1) {
        cp_err = strncpy(out_path, "/favicon.ico", MAX_PAGE_SIZE);
    } else {
        cp_err = strncpy(out_path, allowed_paths[p], MAX_PAGE_SIZE);
        cat_err = strncat(out_path, ".html", MAX_PAGE_SIZE);
    }
    // TODO : make clearear max_page_size, with or without extension

    if (cp_err == NULL || cat_err == NULL) {
        errno = REQUEST_PROBLEM_WITH_STR;
        return -1;
    }

    return allowed;
}

/* Returns -1 on error, size of read if not*/
int read_file(char* readbuf, const char* filename, const int max_readsize) {
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, filename, MAX_PAGE_SIZE);

    printf("[log] Reading path %s\n", path);
    int fd = open(path, O_RDONLY);
    RETURN_ON_ERROR(fd) ;

    int read_err =  read(fd, readbuf, max_readsize);
    check_error(read_err);

    RETURN_ON_ERROR(close(fd));

    return read_err;
}
