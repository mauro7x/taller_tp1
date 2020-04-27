// includes
#include <stdio.h>
#include "client.h"

// defines
#define USAGE_ERROR 1
#define CREATE_ERROR 2
#define INPUT_FILE_ERROR 3
#define CONNECT_ERROR 4
#define SEND_ERROR 5

#define SHUTDOWN_ERROR 10
#define DESTROY_ERROR 11


// --------------------------------------------------------

int main(int argc, const char *argv[]) {
    if ((argc < 3) || (argc > 4)) {
        fprintf(stderr, "Usage error. Usage: ./client <host> <port> [<input_file>]\n");
        return USAGE_ERROR;
    }

    client_t client;

    if (client_create(&client, argv[1], argv[2])) {
        return CREATE_ERROR;
    }

    if (argc == 4) {
        if (client_set_input_file(argv[3])) {
            return INPUT_FILE_ERROR;
        }
    }

    if (client_connect(&client)) {
        client_destroy(&client);
        return CONNECT_ERROR;
    }

    
    if (client_send_calls(&client)) {
        client_shutdown(&client);
        client_destroy(&client);
        return SEND_ERROR;
    }
       
    if (client_shutdown(&client)) {
        client_destroy(&client);
        return SHUTDOWN_ERROR;
    }

    if (client_destroy(&client)) {
        return DESTROY_ERROR;
    }
    

    printf("\nCHAMPIONS\n");
    return 0;
}

// --------------------------------------------------------

