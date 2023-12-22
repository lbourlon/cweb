#include "utils.h"

typedef enum {
    REQUEST_OK,
    REQUEST_TOKEN_IS_NULL,
    REQUEST_METHOD_IS_NOT_GET,
    REQUEST_NOT_IN_ALLOWLIST,
    REQUEST_PROBLEM_WITH_STR,
    REQUEST_HTTP_VERSION_UNSUPORTED,
} request_parse_error;

#ifndef HLAYER_HEADER
#define HLAYER_HEADER

int parse_http_request(char buf[BUFF_SIZE], char* out_path);

#endif
