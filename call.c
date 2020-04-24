// includes
#include "call.h"
#include "stdin_streamer.h"

#include <stdlib.h>
#include <string.h>

// defines
#define PARAMS_DELIMITER ','


// --------------------------------------------------------
// definitions


static void call_parameters_parser(call_t* self, char delimiter) {
 
    char* string = self->params_string;
    size_t len = self->params_string_len;

    size_t n_params = 1;
    for (int i = 0; i < len; i++) { // cuento cuantos hay
        if (string[i] == delimiter) {
            n_params++;
        }
    }

    param_t* params = (param_t*) malloc(sizeof(param_t)*n_params);

    char* current_param;
    int last_delimiter = 0;
    size_t current_param_len = 0;
    size_t params_added = 0;

    for (int i = 0; i < len; i++) {
        if ((string[i] == delimiter) || (i == len-1)) {
            if (i == len-1) { // creamos el ultimo parametro
                current_param_len++;
            }
            current_param = (char*) malloc(sizeof(char)*(current_param_len));
            strncpy(current_param, string + last_delimiter, current_param_len);
            params[params_added].param = current_param;
            params[params_added].param_len = current_param_len;
            current_param_len = 0;
            params_added++;
            last_delimiter = i+1;

        } else {
            current_param_len++;
        }
    }

    self->n_params = n_params;
    self->params = params;
    free(string);
}


int call_create(call_t* self) {
    self->already_filled = 0;

    stdin_streamer_t stdin_streamer;
    stdin_streamer_create(&stdin_streamer, &call_fill);
    
    if (stdin_streamer_run(&stdin_streamer, self)) {
        return -1; // eof
    }
    
    if (self->params_string_len == 0) {
        self->n_params = 0;
        self->params = NULL;
    } else {
        call_parameters_parser(self, PARAMS_DELIMITER);
    }

    stdin_streamer_destroy(&stdin_streamer);
    return 0;
}


int call_fill(void* context, char* buffer, size_t len) {
    call_t* self = context;

    switch (self->already_filled) {
    case 0:
        self->dest = buffer;
        self->dest_len = len;
        break;

    case 1:
        self->path = buffer;
        self->path_len = len;
        break;

    case 2:
        self->interface = buffer;
        self->interface_len = len;
        break;

    case 3:
        self->method = buffer;
        self->method_len = len;
        break;

    case 4:
        self->params_string = buffer;
        self->params_string_len = len;
        break;
    
    default: 
        return -1;
        break;
    }

    self->already_filled++;
    return 0;
}


int call_destroy(call_t* self) {
    if (self->dest) {
        free(self->dest);
    }
    if (self->path) {
        free(self->path);
    }
    if (self->interface) {
        free(self->interface);
    }
    if (self->method) {
        free(self->method);
    }
    
    if (self->params) {
        for (int i = 0; i < (self->n_params); i++) {
            free(self->params[i].param);
        }
        free(self->params);
    }

    
    return 0;
}


// --------------------------------------------------------

