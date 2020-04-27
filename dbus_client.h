#ifndef __DBUS_CLIENT_H__
#define __DBUS_CLIENT_H__

// --------------------------------------------------------
// includes
#include "call.h"

#include <stdint.h>

// --------------------------------------------------------
// estructuras

typedef struct dbus_client {
    call_t* call; // call to be parsed

    char* msg; // parsed msg
    uint32_t total_len;
    uint32_t array_len;
    uint32_t body_len;
} dbus_client_t;

// --------------------------------------------------------
// declaraciones

int dbus_client_create(dbus_client_t* self, call_t* call);

int dbus_client_destroy(dbus_client_t* self);

// --------------------------------------------------------
#endif // __DBUS_CLIENT_H__