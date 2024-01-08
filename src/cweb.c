#include "slayer.h"
#include <stdio.h>
#include "html.h"


int main() {
    
    int server = server_setup();

    while (1) {
        printf("\n\nNEW LOOP\n");
        int client = accept_client(server);        
        client_interract(client);
        
        client_close(client);
    }

    server_close(server);
    return 0;
}
