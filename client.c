// includes
#include "client.h"

#include <string.h>
#include <stdlib.h>
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


int client_connect(client_t* self) {

    if (socket_get_addresses(&(self->socket), self->hostname, self->port, false)) {
        return -1;
    }

    if (socket_connect(&(self->socket), self->hostname, self->port)) {
        return -1;
    }   

    return 0;
}


// --------------------------------------------------------

int server_testing_action(client_t* self) {

    int sent;

    char longitud[2];


    char* msg = "Mensaje random, aguante bokita";
    unsigned short len = strlen(msg) + 1;


    sprintf(longitud, "%d", len);

    // primero mandamos dos digitos que representan la longitud del proximo mensaje
    sent = socket_send(&(self->socket), longitud, 2);
    if (sent == -1) {
        fprintf(stderr, "Error en el envio del mensaje.");
        return -1;
    } else {
        fprintf(stdout, "Se enviaron %d bytes.\n", sent);
    }

    // ahora mandamos el mensaje
    sent = socket_send(&(self->socket), msg, (size_t) len);
    if (sent == -1) {
        fprintf(stderr, "Error en el envio del mensaje.");
        return -1;
    } else {
        fprintf(stdout, "Se enviaron %d bytes.\n", sent);
    }

    return 0;
}

// --------------------------------------------------------


int client_shutdown(client_t* self) {
    if (socket_shutdown(&(self->socket))) {
        fprintf(stderr, "Error apagando el socket.");
        return -1;
    }
    return 0;
}


int client_destroy(client_t* self) {
    fclose(self->input);
    return 0;
}

// --------------------------------------------------------

