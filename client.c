// includes
#include "client.h"
#include "call.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// --------------------------------------------------------
// definiciones

static int client_set_stdin(int argc, const char* argv[]) {
    if (argc==4) {
        FILE *fp;
        if ((fp = fopen(argv[3], "r")) == NULL) {
            return -1;
        }
        stdin = fp;
    }

    return 0;
}


int client_create(client_t* self, int argc, const char* argv[]) {
    self->hostname = argv[1];
    self->port = argv[2];

    socket_t socket;
    socket_create(&socket);
    self->socket = socket;

    if (client_set_stdin(argc, argv)) {
        fprintf(stderr, "Error in function: client_set_stdin. Error: no se puede abrir el archivo.");
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

static int client_send_call(client_t* self) {
    call_t call;

    if (call_create(&call)) {
        return -1; //eof
    }

    call.dest[call.dest_len] = '\0';
    call.path[call.path_len] = '\0';
    call.interface[call.interface_len] = '\0';
    call.method[call.method_len] = '\0';
    call.params[call.params_len] = '\0';

    printf("* dest: %s\n", call.dest);
    printf("* path: %s\n", call.path);
    printf("* interface: %s\n", call.interface);
    printf("* method: %s\n", call.method);
    printf("* params: %s\n", call.params);

    call_destroy(&call);

    return 0;
}


int client_send_calls(client_t* self) {
    
    while(!feof(stdin)) {
        client_send_call(self);
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
    socket_destroy(&self->socket);
    fclose(stdin);
    return 0;
}

// --------------------------------------------------------

