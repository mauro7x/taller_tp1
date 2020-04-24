#ifndef __CALL_H__
#define __CALL_H__

// --------------------------------------------------------
// includes
#include <stdio.h>

// --------------------------------------------------------
// structs

typedef struct param {
    char* string;
    size_t len;
} param_t;

typedef struct call {
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

int call_create(call_t* self);

int call_fill(void* context, char* buffer, size_t len);

int call_destroy(call_t* self);

// --------------------------------------------------------
#endif // __CALL_H__
