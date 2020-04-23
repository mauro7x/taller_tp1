#ifndef __CLIENT_H__
#define __CLIENT_H__

// --------------------------------------------------------
// includes
#include "socket.h"

// --------------------------------------------------------
// estructuras
typedef struct client {
    const char* hostname;
    const char* port;
    FILE* input;
    socket_t socket;
} client_t;

// --------------------------------------------------------
// declaraciones

int client_create(client_t* self, int argc, const char* argv[]);

FILE* client_get_input(int argc, const char* argv[]);

int client_connect(client_t* self);

int client_shutdown(client_t* self);

int client_destroy(client_t* self);

// --------------------------------------------------------

int server_testing_action(client_t* self);

// --------------------------------------------------------
#endif // __CLIENT_H__