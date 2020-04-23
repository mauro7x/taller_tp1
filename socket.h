#ifndef __SOCKET_H__
#define __SOCKET_H__

// --------------------------------------------------------
// includes
#include <stdbool.h>
#include <stdio.h>

// --------------------------------------------------------
// structs

typedef struct addrinfo addrinfo_t;

typedef struct socket {
	int fd; // file descriptor del socket.
    struct addrinfo* addresses_to_try;
    // flags?
} socket_t;

// --------------------------------------------------------
// declarations

int socket_create(socket_t* self);

int socket_get_addresses(socket_t* self, const char* hostname, const char* port, bool server);

// --------------------------------------------------------
// server-side

int socket_config_accepter(socket_t* self);

int socket_bind(socket_t* self, const char* port);

int socket_listen(socket_t* self, const int max_clients_in_queue);

int socket_accept(socket_t* self, socket_t* accepted_socket);

// --------------------------------------------------------
// client-side

int socket_connect(socket_t* self, const char* hostname, const char* port);

// --------------------------------------------------------

int socket_send(socket_t *self, char *buffer, size_t len);

int socket_recv(socket_t *self, char *buffer, size_t len);

int socket_shutdown(socket_t *self);

int socket_destroy(socket_t *self);

// --------------------------------------------------------
#endif // __SOCKET_H__
