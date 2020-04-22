// includes
#include "client.h"

#include <stdio.h>

// --------------------------------------------------------
// definiciones

FILE* client_get_input(int argc, const char* argv[]) {
    if (argc == 3) {        // significa que no tenemos input_file
        return stdin;
    } else {                // significa que tenemos input_file
        return fopen(argv[3], "r");
    }
}

// --------------------------------------------------------

int client_create(client_t* self, int argc, const char* argv[]) {
    self->hostname = argv[1];
    self->port = argv[2];

    // creamos el socket
    socket_t socket;
    socket_create(&socket);
    self->socket = socket;

    // establecemos la entrada
    FILE* input;
    if ((input = client_get_input(argc, argv))) {
        self->input = input;
    } else {
        fprintf(stderr, "Error: no se pudo abrir el archivo.\n");
        return -1;
    }

    return 0;
}

// --------------------------------------------------------

int client_connect(client_t* self) {

    if (socket_get_addresses(&(self->socket), self->hostname, self->port)) {
        return -1;
    }

    if (socket_connect(&(self->socket), self->hostname, self->port)) {
        return -1;
    }

    if (!(self->socket.connected)) {
        fprintf(stderr, "Fallo en la conexion al servidor.\n");
        return -1;
    };
    

    return 0;
}

// --------------------------------------------------------



// --------------------------------------------------------

int client_destroy(client_t* self) {

    return 0;
}

// --------------------------------------------------------
