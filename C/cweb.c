#include "slayer.h"

int main() {
    
    int server = server_setup();

    while (1) {
        mc_debug_print("\n\nNEW LOOP\n");
        int client = accept_client(server);        
        client_interract(client);
        
        client_close(client);
    }

    server_close(server);
    return 0;
}
