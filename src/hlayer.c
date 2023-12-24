#include <stdio.h>
#include <string.h>

#include "hlayer.h"
#include "utils.h"

#define LOCATION_SIZE 16
#define VERSION_SIZE 10

#define BODY_SIZE MSG_BUF_SIZE - 4 - LOCATION_SIZE - VERSION_SIZE

// TODO : change to parse in full and return result
// this way it shouldn't call check_a
/* Error on return != 0 */
int parse_http_request(char* buf, char* out_path) {
    printf("full request : \n\n%s\n", buf);

    char* saveptr_lines;
    char* saveptr_spaces;

    char* lines = strtok_r(buf, "\r\n", &saveptr_lines);

    //spaces -> [GET] /index HTTP/1.1
    char* spaces = strtok_r(lines, " ", &saveptr_spaces);
    if(spaces == NULL) { return REQUEST_TOKEN_IS_NULL; }

    printf("[log] Requested Method : '%s'\n", spaces);
    if(strncmp(spaces, "GET", 3) != 0) { return REQUEST_METHOD_IS_NOT_GET; }

    //spaces -> GET [/index] HTTP/1.1
    spaces = strtok_r(NULL, " ", &saveptr_spaces);
    if(spaces == NULL) { return REQUEST_TOKEN_IS_NULL; }
    
    //TODO : add check
    strncpy(out_path, spaces, MAX_PAGE_SIZE);

    //spaces -> GET /index [HTTP/1.1]
    spaces = strtok_r(NULL, " ", &saveptr_spaces);
    if(spaces == NULL || strncmp(spaces, "HTTP", 4) != 0) {
        return REQUEST_HTTP_VERSION_UNSUPORTED;
    }
    printf("[log] Version : %s\n", spaces);
    #if 0

    // Interates through the next few lines of header, although it's not useful
    int i = 0;
    while (lines != NULL && i < 10) {
        lines = strtok_r(NULL, "\r\n", &saveptr_lines);
        printf("[%2d] %s\n", i, lines);
        i += 1;
    }
    #endif

    return REQUEST_OK;
}
