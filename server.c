// includes
#include "server.h"
#include <string.h>
#include <stdlib.h>

// defines
#define MAX_CLIENTS_IN_QUEUE 10

// --------------------------------------------------------
// definiciones

int server_create(server_t* self, const char* argv[]) {
    self->port = argv[1];

    // OJO, PREGUNTAR SI TIENE QUE ADMITIR VARIAS CONEXIONES SIMULANTEAS.
    // EN ESTE CASO DEBERIAMOS TENER UN SOCKET ACEPTOR, Y N SOCKETS PEER.

    socket_t socket, peer_socket;
    socket_create(&socket);
    socket_create(&peer_socket);
    self->socket = socket; 
    self->peer_socket = peer_socket;

    return 0;
}

int server_open(server_t* self) {

    if (socket_get_addresses(&(self->socket), 0, self->port, true)) {
        return -1;
    }

    if (socket_config_accepter(&(self->socket)
    )) {
        return -1;
    }

    if (socket_bind(&(self->socket), self->port)) {
        return -1;
    }

    if (socket_listen(&(self->socket), MAX_CLIENTS_IN_QUEUE)) {
        return -1;
    }

    return 0;

}

int server_accept(server_t* self) {
    if (socket_accept(&(self->socket), &(self->peer_socket))) {
        fprintf(stderr, "Error acceptando conexion entrante.");
        return -1;
    }
    return 0;
}

// --------------------------------------------------------

int server_testing_action(server_t* self) {
    // queremos recibir un mensaje e imprimirlo por pantalla
    // primero recibimos un numero de dos digitos que indica la longitud
    // luego recibimos el mensaje

    int r;
    unsigned short len;

    char longitud[2];
    memset(longitud, 0, 2);

    r = socket_recv(&(self->peer_socket), longitud, 2);
    // printf("%d", r);
    if (r != 2) {
        fprintf(stdout, "NO SE PUDIERON RECIBIR LOS 2 BYTES DE LONG.");
        return -1;
    }

    len = atoi(longitud);
    printf("Se recibieron %d bytes: [%c, %c]\n", r, longitud[0], longitud[1]);
    printf("Ahora deberian venir %i bytes.\n", len);

    if (len == 0) {
        printf("0 bytes. Cerramos\n");
        return 0;
    }

    char* tmp = (char*) malloc(sizeof(char) * (len));
    r = socket_recv(&(self->peer_socket), tmp, (size_t) len);
    printf("Bytes recibidos: %d\n", r);
    printf("Mensaje recibido: %s\n", tmp);

    free(tmp);
    return 0;
}

// --------------------------------------------------------

int server_shutdown(server_t* self) {
    if (socket_shutdown(&(self->socket))) {
        fprintf(stderr, "Error apagando el socket.");
        return -1;
    }
    return 0;
}

int server_destroy(server_t* self) {
    return 0;
}

// --------------------------------------------------------