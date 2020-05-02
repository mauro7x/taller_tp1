// ----------------------------------------------------------------------------
#include "client.h"
#include "dbus_client.h"
#include "socket.h"
#include "stdin_streamer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define EOF_ERROR 1
#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int client_create(client_t* self, const char* hostname, const char* port) {
    self->hostname = hostname;
    self->port = port;
    self->next_msg_id = 1;

    socket_t socket;
    if (socket_create(&socket)) {
        fprintf(stderr, "Error in function: socket_create.\n");
        return -1;
    }
    self->socket = socket;
    return 0;
}


int client_set_input_file(const char* path_to_file) {
    FILE *fp;
    if ((fp = fopen(path_to_file, "r")) == NULL) {
        fprintf(stderr, "Error in function: client_set_stdin. "
                        "Error: no se puede abrir el archivo.");
        return -1;
    }
    stdin = fp;

    return 0;
}


int client_connect(client_t* self) {
    if (socket_get_addresses(&(self->socket), self->hostname, self->port,
                             false)) {
        return -1;
    }

    if (socket_connect(&(self->socket))) {
        return -1;
    }   

    return 0;
}


int client_send_call(void* context, char* buffer, size_t len) {
    client_t* self = (client_t*) context;
    dbus_client_t dbus_client;
    dbus_client_create(&dbus_client, &(self->socket));
    dbus_client_fill(&dbus_client, buffer, len, (self->next_msg_id)++);

    // Enviar la call
    if (dbus_client_send_call(&dbus_client)) {
        return -1;
    }
    
    // Imprimir respuesta del servidor
    if (dbus_client_print_server_reply(&dbus_client)) {
        return -1;
    }
    
    dbus_client_destroy(&dbus_client);
    return 0;
}


int client_send_calls(client_t* self) {
    stdin_streamer_t streamer;
    stdin_streamer_create(&streamer, &client_send_call);  
    stdin_streamer_run(&streamer, self);
    stdin_streamer_destroy(&streamer);
    return 0;
}


int client_shutdown(client_t* self) {
    if (socket_shutdown(&(self->socket), SHUT_RDWR)) {
        fprintf(stderr, "Error apagando el socket.\n");
        return -1;
    }
    return 0;
}


int client_destroy(client_t* self) {
    if (socket_destroy(&(self->socket))) { 
        return -1;
    }

    if (fclose(stdin)) {
        return -1;
    }
    return 0;
}


// ----------------------------------------------------------------------------
