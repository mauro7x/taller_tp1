#ifndef __CLIENT_H__
#define __CLIENT_H__

// --------------------------------------------------------
// includes
#include "socket.h"
#include "stdin_streamer.h"

// --------------------------------------------------------
// estructuras
typedef struct client {
    const char* port;
    const char* hostname;
    socket_t socket;
    stdin_streamer_t streamer;
} client_t;

// --------------------------------------------------------
// declaraciones

int client_create(client_t* self, int argc, const char* argv[]);

int client_connect(client_t* self);

int client_send_calls(client_t* self);

int client_shutdown(client_t* self);

int client_destroy(client_t* self);

// --------------------------------------------------------

int server_testing_action(client_t* self);

// --------------------------------------------------------
#endif // __CLIENT_H__