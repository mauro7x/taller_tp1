#ifndef __CLIENT_H__
#define __CLIENT_H__

// --------------------------------------------------------
// includes
#include "socket.h"
#include <stdint.h>

// --------------------------------------------------------
// estructuras
typedef struct client {
    uint32_t next_msg_id; 
    const char* port;
    const char* hostname;
    socket_t socket;
} client_t;

// --------------------------------------------------------
// declaraciones

int client_create(client_t* self, int argc, const char* argv[]);

int client_connect(client_t* self);

int client_send_calls(client_t* self);

int client_send_call(void* context, char* buffer, size_t len);

int client_shutdown(client_t* self);

int client_destroy(client_t* self);

#endif // __CLIENT_H__