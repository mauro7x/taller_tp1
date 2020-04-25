// includes
#include "call.h"
#include "stdin_streamer.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// defines
#define PARAMS_DELIMITER ','

// --------------------------------------------------------
// static definitions for parsing

// parameters parser

static size_t call_parameters_counter(char* buffer, size_t len, char delimiter) {

    size_t n_params = 1;

    for (int i = 0; i < len; i++) { // cuento cuantos hay
        if (buffer[i] == delimiter) {
            n_params++;
        }
    }

    return n_params;
}

static void call_parameters_fill(param_t* params, char* buffer, size_t len, char delimiter) {
    int last_delimiter = 0;
    size_t current_param_len = 0;
    size_t params_added = 0;

    for (int i = 0; i < len; i++) {
        if ((buffer[i] == delimiter) || (i == len-1)) {
            if (i == len-1) { // creamos el ultimo parametro
                current_param_len++;
            }
            char* current_param = (char*) malloc(sizeof(char)*(current_param_len));
            strncpy(current_param, buffer + last_delimiter, current_param_len);
            params[params_added].string = current_param;
            params[params_added].len = (uint32_t) current_param_len;
            current_param_len = 0;
            params_added++;
            last_delimiter = i+1;

        } else {
            current_param_len++;
        }
    }
}

static void call_parameters_parser(call_t* self, char* buffer, size_t len, char delimiter) {

    size_t n_params = call_parameters_counter(buffer, len, delimiter);
    param_t* params = (param_t*) malloc(sizeof(param_t)*n_params);
    call_parameters_fill(params, buffer, len, delimiter);

    self->n_params = n_params;
    self->params = params;
    free(buffer);
}

// --------------------------------------------------------
// public definitions

int call_create(call_t* self, uint32_t id, stdin_streamer_t* streamer) {
    self->already_filled = 0;
    self->id = id;

    if (stdin_streamer_run(streamer, self)) {
        return -1; // eof
    }

    return 0;
}

int call_fill(void* context, char* buffer, size_t len) {
    call_t* self = context;

    switch (self->already_filled) {
    case 0:
        self->dest.len = (uint32_t) len;
        self->dest.string = buffer;
        break;

    case 1:
        self->path.len = (uint32_t) len;
        self->path.string = buffer;
        break;

    case 2:
        self->interface.len = (uint32_t) len;
        self->interface.string = buffer;
        break;

    case 3:
        self->method.len = (uint32_t) len;
        self->method.string = buffer;
        break;

    case 4:
        if (len == 0) { // no hay parametros
            self->n_params = 0;
            self->params = NULL;
            free(buffer);
        } else { // en buffer tenemos un string de los parametros
            call_parameters_parser(self, buffer, len, PARAMS_DELIMITER);
        }
        break;
    
    default: 
        return -1;
        break;
    }

    self->already_filled++;
    return 0;
}

int call_destroy(call_t* self) {
    if (self->dest.string) {
        free(self->dest.string);
    }
    if (self->path.string) {
        free(self->path.string);
    }
    if (self->interface.string) {
        free(self->interface.string);
    }
    if (self->method.string) {
        free(self->method.string);
    }
    
    if (self->params) {
        for (int i = 0; i < (self->n_params); i++) {
            free(self->params[i].string);
        }
        free(self->params);
    }
    
    /*
    if (self->msg) {
        free(self->msg);
    }
    */
    
    return 0;
}

// --------------------------------------------------------

