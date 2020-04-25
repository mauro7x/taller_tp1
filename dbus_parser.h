#ifndef __DBUS_PARSER_H__
#define __DBUS_PARSER_H__

// --------------------------------------------------------
// includes
#include "call.h"

#include <stdint.h>

// --------------------------------------------------------
// estructuras

typedef struct dbus_parser {
    call_t* call; // call to be parsed

    char* msg; // parsed msg
    uint32_t total_len;
    uint32_t array_len;
    uint32_t body_len;
} dbus_parser_t;

// --------------------------------------------------------
// declaraciones

int dbus_parser_create(dbus_parser_t* self, call_t* call);

int dbus_parser_destroy(dbus_parser_t* self);

// --------------------------------------------------------
#endif // __DBUS_PARSER_H__