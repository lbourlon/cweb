#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"


/* Returns -1 on error, size of read if not*/
int read_file(char* readbuf, const char* filename) {
    char path[7 + MAX_PAGE_SIZE] =  "./files/";
    strncat(path, filename, MAX_PAGE_SIZE);

    printf("[log] Reading path %s\n", path);
    int fd = open(path, O_RDONLY);
    RETURN_ON_ERROR(fd) ;

    int read_err =  read(fd, readbuf, BUFF_SIZE);
    check_error(read_err);

    RETURN_ON_ERROR(close(fd));

    return read_err;
}
