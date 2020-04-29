/**
 * Para avisarle al compilador que vamos a usar las extensiones POSIX que no
 * estan incluidas en el std=c99.
 */

#define _POSIX_C_SOURCE 200112L

// ----------------------------------------------------------------------------
#include "socket.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// "Métodos" privados
// ----------------------------------------------------------------------------

/**
 * @desc:   arregla el problema del TIMEWAIT.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fix_timewait(socket_t* self) {
    int val = 1;
    if (setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))) {
        fprintf(stderr, "Error in function: _fix_timewait.\n");
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    return 0;
}


/**
 * @desc:   establece el file descriptor del socket a address.
 * @param:  puntero a addrinfo_t, address a establecer como file descriptor.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _set_fd(socket_t* self, addrinfo_t* address) {
    self->fd = socket(address->ai_family, address->ai_socktype,
                      address->ai_protocol);

    if (self->fd == -1) { 
        fprintf(stderr, "Error in function: _set_fd. Error: %s\n",
                strerror(errno));
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    return 0;
}


// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int socket_create(socket_t *self) {
    if (!self) {
        return -1;
    }
    self->fd = 0;
    return 0;
}


int socket_get_addresses(socket_t *self, const char* hostname,
                         const char* port, bool passive) {
    int s; // variable para el manejo de errores
    struct addrinfo hints; // filtros

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;          // IPv4
    hints.ai_socktype = SOCK_STREAM;    // TCP
    if (passive) {
        hints.ai_flags = AI_PASSIVE;        // Server mode
    } else {
        hints.ai_flags = 0;                 // Client mode
    }
    
    s = getaddrinfo(hostname, port, &hints, &(self->addresses_to_try));
    if (s != 0) { // verificamos que no hayan errores
        fprintf(stderr, "Error in function: getaddrinfo. Error: %s\n",
                gai_strerror(s));
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    return 0;
}


int socket_bind(socket_t* self, const char* port) {
    if (_set_fd(self, self->addresses_to_try)) {
        return -1;
    }

    if (_fix_timewait(self)) {
        return -1;
    }

    if (bind(self->fd, (self->addresses_to_try)->ai_addr,
       (self->addresses_to_try)->ai_addrlen)) {
        fprintf(stderr, "Error in function: socket_bind. Error: %s\n",
                strerror(errno));
        freeaddrinfo(self->addresses_to_try);
        return -1;
    }

    freeaddrinfo(self->addresses_to_try);
    return 0;
}


int socket_listen(socket_t* self, const int max_clients_in_queue) {
    if (listen(self->fd, max_clients_in_queue)) {
        fprintf(stderr, "Error in function: socket_listen.\n");
        return -1;
    }

    return 0;
}


int socket_accept(socket_t* self, socket_t* accepted_socket) {
    accepted_socket->fd = accept(self->fd, NULL, NULL);

    if (accepted_socket->fd == -1) {
        fprintf(stderr, "Error in function: socket_accept. Error: %s\n",
                strerror(errno));
        return -1;
    }
    
    return 0;
}


int socket_connect(socket_t *self) {
    bool connected = false;
    struct addrinfo* ptr;

    for (ptr = self->addresses_to_try; ptr != NULL && connected == false;
        ptr = ptr->ai_next) {
        if (_set_fd(self, ptr)) {
            return -1;
        }

        if (connect(self->fd, ptr->ai_addr, ptr->ai_addrlen)) {
            fprintf(stderr, "Error in socket_connect. Error: %s\n",
                    strerror(errno));
            close(self->fd);
        } else {
            connected = true;
        }
    }

    freeaddrinfo(self->addresses_to_try); // liberamos la memoria de addresses
    return (!connected); // 0 si se conecto, 1 si no
}


int socket_send(socket_t *self, char *buffer, size_t len) {
    int total_sent = 0;
    int last_sent = 0;

    while (total_sent < len) {
        last_sent = send(self->fd, &buffer[total_sent], len - total_sent,
                         MSG_NOSIGNAL);

        if (last_sent == -1) {
            fprintf(stderr, "Error in function: socket_send.\n");
            return -1;
        } else if (last_sent == 0) {
            return 0;
        } else { // se enviaron last_sent bytes.
            total_sent += last_sent;
        }
    }

    return total_sent;
}


int socket_recv(socket_t *self, char *buffer, size_t len) {
    int total_received = 0;
    int last_received = 0;
    
    while (total_received < len) {
        last_received = recv(self->fd, &buffer[total_received],
                             len - total_received, 0);

        if (last_received == -1) {
            fprintf(stderr, "Error in function: socket_recv.\n");
            return -1;
        } else if (last_received == 0) { // socket cerrado
            return 0;
        } else {
            total_received += last_received;
        }
    }

    return total_received;
}


int socket_shutdown(socket_t *self, int channel) {
    if (shutdown(self->fd, channel)) {
        return -1;
    }

    return 0;
}


int socket_destroy(socket_t *self) {
    if (close(self->fd)) {
        return -1;
    }
    return 0;
}


// ----------------------------------------------------------------------------
