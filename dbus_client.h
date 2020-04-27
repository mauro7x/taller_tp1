#ifndef __DBUS_CLIENT_H__
#define __DBUS_CLIENT_H__

// --------------------------------------------------------
// includes
#include "call.h"
#include "socket.h"

#include <stdint.h>

// --------------------------------------------------------
// estructuras

typedef struct dbus_client {
    call_t call; // call to be parsed
    socket_t* socket; // socket to send msg to

    char* msg; // parsed msg
    uint32_t total_len;
    uint32_t array_len;
    uint32_t body_len;
} dbus_client_t;

// --------------------------------------------------------
// declaraciones

int dbus_client_create(dbus_client_t* self, socket_t* socket);

int dbus_client_fill(dbus_client_t* self, char* buffer, size_t len, int id);

int dbus_client_send_call(dbus_client_t* self);

int dbus_client_print_server_reply(dbus_client_t* self);

int dbus_client_destroy(dbus_client_t* self);

// --------------------------------------------------------
#endif // __DBUS_CLIENT_H__