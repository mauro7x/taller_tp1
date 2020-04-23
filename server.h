#ifndef __SERVER_H__
#define __SERVER_H__

// --------------------------------------------------------
// includes
#include "socket.h"

// --------------------------------------------------------
// estructuras
typedef struct server {
    const char* port;
    socket_t socket;        // socket aceptador (S)
    socket_t peer_socket;   // socket correspondiente al cliente (P)
} server_t;

// --------------------------------------------------------
// declaraciones


int server_create(server_t* self, const char* argv[]);

int server_open(server_t* self);

int server_accept(server_t* self);

int server_shutdown(server_t* self);

int server_destroy(server_t* self);

// --------------------------------------------------------

int server_testing_action(server_t* server);

// --------------------------------------------------------
#endif // __SERVER_H__