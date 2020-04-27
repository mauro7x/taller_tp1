#ifndef __SERVER_H__
#define __SERVER_H__

// --------------------------------------------------------
// includes
#include "socket.h"

// --------------------------------------------------------
// estructuras
typedef struct server {
    const char* port;
    const char* hostname;
    socket_t acceptor;        // socket aceptador (S)
    socket_t peer;   // socket correspondiente al cliente (P)
} server_t;

// --------------------------------------------------------
// declaraciones


int server_create(server_t* self, const char* port);

int server_open(server_t* self);

int server_accept(server_t* self);

int server_receive_calls(server_t* self);

int server_shutdown(server_t* self);

int server_destroy(server_t* self);

#endif // __SERVER_H__