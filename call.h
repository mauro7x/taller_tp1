#ifndef __CALL_H__
#define __CALL_H__

// --------------------------------------------------------
// includes
#include <stdio.h>
#include <stdint.h>

#include "stdin_streamer.h"

// --------------------------------------------------------
// structs

typedef struct param {
    char* string;
    uint32_t len;
    char id;
    char data_type;
} param_t;

typedef struct call {
    uint32_t id;
    char* msg;
    uint32_t total_len;
    uint32_t array_len;
    uint32_t body_len;

    int already_filled;
    param_t dest;
    param_t path;
    param_t interface;
    param_t method;
    param_t* params;
    size_t n_params; 
} call_t;


// --------------------------------------------------------
// declarations

int call_create(call_t* self, uint32_t id, stdin_streamer_t* streamer);

int call_fill(void* context, char* buffer, size_t len);

int call_destroy(call_t* self);

// --------------------------------------------------------
#endif // __CALL_H__
