#ifndef __DBUS_SERVER_H__
#define __DBUS_SERVER_H__

// --------------------------------------------------------
#include "call.h"
#include "socket.h"

// --------------------------------------------------------
// Structs

typedef struct dbus_server {
    call_t call; // call to be received
    socket_t* peer; // peer to receive from
    int has_args;

} dbus_server_t;

// --------------------------------------------------------
// Public API

int dbus_server_create(dbus_server_t* self, socket_t* peer);

int dbus_server_recv_call(dbus_server_t* self);

void dbus_server_print_received_call(dbus_server_t* self);

int dbus_server_send_confirmation(dbus_server_t* self);

int dbus_server_destroy(dbus_server_t* self);

// --------------------------------------------------------

#endif // __DBUS_SERVER_H__
