// includes
#include "server.h"
#include "socket.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

static int server_receive_param(server_t* self) {

    char desc[4];
    uint32_t param_len;
    char aux[4];
    char* param;

    char discard;

    socket_recv(&(self->peer), desc, sizeof(desc));

    socket_recv(&(self->peer), aux, sizeof(aux));
    memcpy(&param_len, aux, sizeof(aux));
    printf("param_len: %u\n", param_len);

    param = (char*) malloc(sizeof(char) * (param_len+1));
    param[param_len] = '\0'; 
    socket_recv(&(self->peer), param, param_len);
    puts(param);

    socket_recv(&(self->peer), &discard, sizeof(discard));

    if ((param_len+1) % 8) {
        size_t padding = 8 - ((param_len+1) % 8);
        for (int i = 0; i < padding; i++) {
            socket_recv(&(self->peer), &discard, sizeof(discard));
        }
    }

    free(param);

    return 0;
}


static int server_receive_call(server_t* self) {
    char header_bytes[3];
    uint32_t body_len;
    uint32_t id_msg;
    uint32_t array_len;
    char aux[4];

    socket_recv(&(self->peer), header_bytes, sizeof(header_bytes));

    socket_recv(&(self->peer), aux, sizeof(body_len));
    memcpy(&body_len, aux, sizeof(aux));
    printf("body length: %u\n", body_len);


    socket_recv(&(self->peer), aux, sizeof(id_msg));
    memcpy(&id_msg, aux, sizeof(aux));
    printf("id_msg: %u\n", id_msg);

    socket_recv(&(self->peer), aux, sizeof(array_len));
    memcpy(&array_len, aux, sizeof(aux));
    printf("array_len: %u\n", array_len);

    server_receive_param(self);
    server_receive_param(self);
    server_receive_param(self);
    server_receive_param(self);


    return 0;
}



int server_receive_calls(server_t* self) {

    char endianness[1];
    int s;
    while ((s = socket_recv(&(self->peer), endianness, 1))) {
        if (s == -1) {
            fprintf(stderr, "Error in function: socket_recv.");
            return -1;
        }

        server_receive_call(self);
        
        break;
    }

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
