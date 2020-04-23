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

int socket_get_addresses(socket_t *self, const char* hostname, const char* port, bool server) {
    int s; // variable para el manejo de errores
    struct addrinfo hints; // filtros

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
    if (server) {
        hints.ai_flags = AI_PASSIVE;        // Server mode
    } else {
        hints.ai_flags = 0;                 // Client mode
    }
    

    s = getaddrinfo(hostname, port, &hints, &(self->addresses_to_try));
    if (s != 0) { // verificamos que no hayan errores
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    return 0;
}


// --------------------------------------------------------
// server-side functions

int socket_fix_timeout(socket_t* self) {
    int val = 1;
    if (setsockopt(self->state, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))) {
        fprintf(stderr, "Error arreglando el TIMEWAIT.");
        close(self->state);
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }
    return 0;
}

int socket_config_accepter(socket_t* self) {
    self->state = socket((self->addresses_to_try)->ai_family, (self->addresses_to_try)->ai_socktype, (self->addresses_to_try)->ai_protocol);

    if (self->state == -1) {    // verificamos que no hayan errores en el aceptador
        printf("Error: %s\n", strerror(errno));
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    if (socket_fix_timeout(self)) { // arreglamos TIMEWAIT de existir
        return -1;
    }
    return 0;
}

int socket_bind(socket_t* self, const char* port) {
    if (bind(self->state, (self->addresses_to_try)->ai_addr, (self->addresses_to_try)->ai_addrlen)) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        close(self->state);
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    freeaddrinfo(self->addresses_to_try);
    return 0;
}

int socket_listen(socket_t* self, const int max_clients_in_queue) {
    if (listen(self->state, max_clients_in_queue)) {
        fprintf(stderr, "Error en la configuracion del listen del socket.");
        return -1;
    }

    return 0;
}

int socket_accept(socket_t* self, socket_t* accepted_socket) {
    
    accepted_socket->state = accept(self->state, NULL, NULL);

    if (accepted_socket->state == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }

    self->connected = true;
    accepted_socket->connected = true;

    fprintf(stdout, "Se ha conectado un cliente!\n");
    return 0;
}


// --------------------------------------------------------
// client-side functions

int socket_connect(socket_t *self, const char* hostname, const char* port) {

    struct addrinfo* ptr;
    for (ptr = self->addresses_to_try; ptr != NULL && self->connected == false; ptr = ptr->ai_next) {
        self->state = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (self->state == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        } else {
            if (connect(self->state, ptr->ai_addr, ptr->ai_addrlen)) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
                close(self->state);
            } else {
                self->connected = true;
            }
        }
    }

    freeaddrinfo(self->addresses_to_try); // liberamos la memoria de addresses
    return 0;
}

// --------------------------------------------------------

int socket_send(socket_t *self, char *buffer, size_t len) {
    int sent = 0;
    int s = 0;
    bool socket_valid = true;

    while (sent < len && socket_valid) {
        s = send(self->state, &buffer[sent], len - sent, MSG_NOSIGNAL);

        if (s == 0) {
            socket_valid = false;
        } else if (s == -1) {
            socket_valid = false;
        } else {
            sent += s;
        }
    }

    if (socket_valid) {
        return sent;
    }
    else {
        return -1;
    }
}

int socket_recv(socket_t *self, char *buffer, size_t len) {
    int received = 0;
    int r = 0;
    bool socket_valid = true;

    while (received < len && socket_valid) {
        r = recv(self->state, &buffer[received], len - received, 0);

        if (r == 0) {
            socket_valid = false;
        } else if (r == -1) {
            socket_valid = false;
        } else {
            received += r;
        }
    }

    if (socket_valid) {
        return received;
    }
    else {
        return -1;
    }
}

int socket_shutdown(socket_t *self) {
    if (shutdown(self->state, SHUT_RDWR)) {
        return -1;
    }
    close(self->state);
    return 0;
}

int socket_destroy(socket_t *self) {
    return 0;
}

// --------------------------------------------------------