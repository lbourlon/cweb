#include "slayer.h"
#include <stdio.h>


int main() {
    while (1) {
        printf("\n");
        int server_fd = server_setup();
        server_accept(server_fd);        
    }
    return 0;
}
