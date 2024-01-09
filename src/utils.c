#include "utils.h"
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>


int check_allowed(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], const char* requested) {
    mc_debug_print("[log] Requested Path : '%s'\n", requested);

    for (int i = 0; i < NUM_PAGES; i++) {
        if(strncmp(requested, allowed_paths[i], MAX_PAGE_SIZE) == 0) {
            return i;
        }
    }
    return -1;
}

int add_extension(const char allowed_paths[NUM_PAGES][MAX_PAGE_SIZE], char* out_path, int i) {
    char* cp_err; char* cat_err;
    if (i == 0) {
        cp_err = strncpy(out_path, "/index.html", MAX_PAGE_SIZE);
    } else {
        cp_err = strncpy(out_path, allowed_paths[i], MAX_PAGE_SIZE);
        cat_err = strncat(out_path, ".html", MAX_PAGE_SIZE);
    }

    return_on_err(cp_err == NULL || cat_err == NULL, "Error on strfunc");
    return 0;
}

/* Returns -1 on error, size of read if not*/
int read_file(char* readbuf, const char* filename, const int max_readsize) {
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, filename, MAX_PAGE_SIZE);

    mc_debug_print("[log] Reading path %s\n", path);
    int fd = open(path, O_RDONLY);
    return_on_err(fd == -1, "Couldn't open file");

    int read_err = read(fd, readbuf, max_readsize);
    return_on_err(read_err == -1, "Couldn't read file");

    return_on_err(close(fd) == -1, "Couldn't Close file");

    return read_err;
}
