#ifndef __CALL_H__
#define __CALL_H__

// --------------------------------------------------------
// includes
#include <stdio.h>

// --------------------------------------------------------
// structs

typedef struct param {
    char* param;
    size_t param_len;
} param_t;

typedef struct call {
    int already_filled;

    char* dest;
    size_t dest_len;
    char* path;
    size_t path_len;
    char* interface;
    size_t interface_len;
    char* method;
    size_t method_len;
    char* params_string;
    size_t params_string_len;

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
