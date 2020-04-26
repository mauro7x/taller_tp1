// includes
#include "server.h"
#include "socket.h"

#include <string.h>
#include <stdlib.h>

// defines
#define MAX_CLIENTS_IN_QUEUE 10
#define HOSTNAME 0

// --------------------------------------------------------
// definiciones

int server_create(server_t* self, const char* argv[]) {
    self->port = argv[1];
    self->hostname = HOSTNAME;

    // socket aceptador, y socket peer
    socket_t socket, peer_socket;
    socket_create(&socket);
    socket_create(&peer_socket);
    self->acceptor = socket; 
    self->peer = peer_socket;

    return 0;
}

int server_open(server_t* self) {

    if (socket_get_addresses(&(self->acceptor), self->hostname, self->port, true)) {
        return -1;
    }

    if (socket_bind(&(self->acceptor), self->port)) {
        return -1;
    }

    if (socket_listen(&(self->acceptor), MAX_CLIENTS_IN_QUEUE)) {
        return -1;
    }

    return 0;

}

int server_accept(server_t* self) {
    if (socket_accept(&(self->acceptor), &(self->peer))) {
        fprintf(stderr, "Error acceptando conexion entrante.");
        return -1;
    }
    return 0;
}



int server_receive_calls(server_t* self) {

    


    return 0;
}




int server_shutdown(server_t* self) {
    if (socket_shutdown(&(self->acceptor))) {
        fprintf(stderr, "Error in function: socket_shutdown (S)\n");
        return -1;
    }

    if (socket_shutdown(&(self->peer))) {
        fprintf(stderr, "Error in function: socket_shutdown (P)\n");
        return -1;
    }

    return 0;
}

int server_destroy(server_t* self) {
    socket_destroy(&(self->acceptor));
    socket_destroy(&(self->peer));
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

    r = socket_recv(&(self->peer), longitud, 2);
    // printf("%d", r);
    if (r != 2) {
        fprintf(stdout, "NO SE PUDIERON RECIBIR LOS 2 BYTES DE LONG.");
        return -1;
    }

    len = atoi(longitud);
    printf("Se recibieron %d bytes: [%d, %d]\n", r, longitud[0], longitud[1]);
    printf("Ahora deberian venir %i bytes.\n", len);

    if (len == 0) {
        printf("0 bytes. Cerramos\n");
        return 0;
    }

    char* tmp = (char*) malloc(sizeof(char) * (len));
    r = socket_recv(&(self->peer), tmp, (size_t) len);
    printf("Bytes recibidos: %d\n", r);

    printf("Mensaje recibido: %s\n", tmp);

    free(tmp);
    return 0;
}

// --------------------------------------------------------
