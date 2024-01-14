#include "utils.h"
#include "slayer.h"
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>


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
        if(strncmp(out_path, "/images",7) != 0) {
            cat_err = strncat(out_path, ".html", MAX_PAGE_SIZE);
        }
    }

    return_on_err(cp_err == NULL || cat_err == NULL, "Error on strfunc");
    return 0;
}

int better_read_file(char* out_buf, const char* path, const int max_size)
{
    FILE* file = fopen(path, "r");
    return_on_err(file == NULL, "Couldn't open file");

    int err = fread(out_buf, sizeof(unsigned char), max_size, file);
    return_on_err(err == -1, "Couldn't read file");
    mc_debug_print("Actual read : %d", err);

    return_on_err(fclose(file) == -1, "Couldn't Close file");
    return 0;
}

int build_response(char* out_buf, int max_size, const http_request* rq)
{
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, rq->page, MAX_PAGE_SIZE);
    return_on_err(path == NULL, "Couldn't append to path");

    struct stat st;
    int err = stat(path, &st);
    return_on_err(err == -1, "Couldn't stat file");

    int header_size, new_start;
    if(rq->content_type == IMAGE)
    {
        const char* image_type = "image/webp";
        header_size = sizeof(HTTP_IMAGE_HEADER_FSTR) - 5 + strlen(image_type) + strlen("9999999");

        new_start = snprintf(out_buf, header_size, HTTP_IMAGE_HEADER_FSTR, image_type, st.st_size);
    }
    else
    {
        header_size = sizeof(HTTP_OK);
        new_start = snprintf(out_buf, header_size, HTTP_OK);
    }

    int remaining_space = max_size - header_size;
    mc_debug_print("Initial write : %d\n", initial_write);
    return_on_err(new_start == -1, "Couldn't snprintf file");
    return_on_err(new_start + st.st_size > max_size, "file wouldn't fit in buffer");

    err = better_read_file(&out_buf[new_start], path, remaining_space);

    return err;
}

/* Returns -1 on error, size of read if not*/
int read_file(char* readbuf, const char* file_path, const int max_readsize) {
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, file_path, MAX_PAGE_SIZE);

    mc_debug_print("Reading path %s\n", path);
    int fd = open(path, O_RDONLY);
    return_on_err(fd == -1, "Couldn't open file");

    int read_err = read(fd, readbuf, max_readsize);
    return_on_err(read_err == -1, "Couldn't read file");
    return_on_err(close(fd) == -1, "Couldn't Close file");

    return read_err;
}
