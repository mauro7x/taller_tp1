// includes
#include "client.h"
#include "call.h"
#include "dbus_parser.h"
#include "socket.h"
#include "stdin_streamer.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// defines
#define EOF_ERROR 1


// --------------------------------------------------------
// static definitions

static int client_send_parsed_msg(client_t* self, char* msg, uint32_t len) {
    int s;
    s = socket_send(&(self->socket), msg, (size_t) len);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: client_send_parsed_msg. "
                        "Socket was closed before expected.\n");
        return -1;
    }

    return 0;
}

static int client_print_server_reply(client_t* self, call_t* call) {
    
    int s;
    char reply[3] = "";
    s = socket_recv(&(self->socket), &(reply[0]), 3);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: client_print_server_reply. "
                        "Socket was closed before expected.\n");
        return -1;
    }
    
    printf("0x%04x: %s\n", call->id, reply);

    return 0;
}


// --------------------------------------------------------
// public definitions

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

    if (socket_get_addresses(&(self->socket), self->hostname, self->port, false)) {
        return -1;
    }

    if (socket_connect(&(self->socket), self->hostname, self->port)) {
        return -1;
    }   

    return 0;
}

/**
 * CALLBACK del STDIN_STREAMER.
 * Recibe una linea sin parsear, arma la call, y la envia.
*/
int client_send_call(void* context, char* buffer, size_t len) {
    client_t* self = (client_t*) context;
 
    call_t call;
    call_create(&call);
    call_fill(&call, buffer, len, (self->next_msg_id)++);

    dbus_parser_t dbus_parser;
    dbus_parser_create(&dbus_parser, &call);

    /*
    for (int i = 0; i < dbus_parser.total_len; i++) {
        printf("msg[%i]: %d\n", i, dbus_parser.msg[i]);
    }
    */

    // enviar call
    client_send_parsed_msg(self, dbus_parser.msg, dbus_parser.total_len);

    // recibimos e imprimimos respuesta
    client_print_server_reply(self, &call);


    dbus_parser_destroy(&dbus_parser);
    call_destroy(&call);
    return 0;
}

int client_send_calls(client_t* self) {
    stdin_streamer_t streamer;
    stdin_streamer_create(&streamer, &client_send_call);  

    /**
     * stdin_streamer leera una call, llama a client_create_call,
     * quien crea la call y la llena con los datos, luego la parsea
     * llamando a dbus_parser, para finalmente enviarla.
    */
    stdin_streamer_run(&streamer, self);  

    stdin_streamer_destroy(&streamer);
    return 0;
}

int client_shutdown(client_t* self) {
    if (socket_shutdown(&(self->socket))) {
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

// --------------------------------------------------------
