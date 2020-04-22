/*
Para avisarle al compilador que vamos a usar las
extensiones POSIX, que no estan incluidas en el
std=c99
*/
#define _POSIX_C_SOURCE 200112L

// includes
#include <stdio.h>
#include <stdbool.h>

#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "socket.h"

// --------------------------------------------------------
// definiciones

int socket_create(socket_t *self) {
    if (!self) {
        return -1;
    }
    
    self->state = 0;
    self->connected = false;
    return 0;
}

int socket_get_addresses(socket_t *self, const char* hostname, const char* port) {
    int s; // variable para el manejo de errores
    struct addrinfo hints; // filtros

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_flags = 0;                 // None

    s = getaddrinfo(hostname, port, &hints, &(self->addresses_to_try));
    if (s != 0) { // verificamos que no hayan errores
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(s));
        return -1;
    }

    return 0;
}

int socket_connect(socket_t *self, const char* hostname, const char* port) {

    struct addrinfo* ptr;
    for (ptr = self->addresses_to_try; ptr != NULL && self->connected == false; ptr = ptr->ai_next) {
        self->state = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (self->state == -1) {
            printf("Error: %s\n", strerror(errno));
        } else {
            if (connect(self->state, ptr->ai_addr, ptr->ai_addrlen)) {
                printf("Error: %s\n", strerror(errno));
                close(self->state);
            } else {
                self->connected = true;
            }
        }
    }

    freeaddrinfo(self->addresses_to_try); // liberamos la memoria de addresses
    return 0;
}


int socket_send(socket_t *self, char *buffer, size_t len) {
    
    return 0;
}

int socket_recv(socket_t *self, char *buffer, size_t len) {
    
    return 0;
}

int socket_shutdown(socket_t *self) {
    shutdown(self->state, SHUT_RDWR);
    close(self->state);
    return 0;
}

int socket_destroy(socket_t *self) {
    return 0;
}

// --------------------------------------------------------