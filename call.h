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
    char endianness;
    uint32_t id;
    
    param_t dest;
    param_t path;
    param_t interface;
    param_t method;
    param_t* params;
    size_t n_params; 

} call_t;


// --------------------------------------------------------
// declarations

int call_create(call_t* self);

int call_fill(call_t* self, char* line, size_t len, uint32_t id);

int call_destroy(call_t* self);

// --------------------------------------------------------
#endif // __CALL_H__
