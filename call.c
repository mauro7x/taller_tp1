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

// initialize params

static void call_initialize_param(call_t* self, param_t* param) {
    param->data_type = 0;
    param->id = 0;
    param->len = 0;
    param->string = NULL;
}

// line parser

static void call_fill_param_from_line(param_t* param, char* line, int* offset, char delimiter, size_t max_len) {

   uint32_t param_len = 0;
   int delimiter_found = 0;

   for (int i = (*offset); (i < max_len) && (!delimiter_found); i++) {
       if (line[i] != delimiter) {
           param_len++;
       } else {
           param->len = param_len;
           param->string = (char*) malloc(sizeof(char)*param_len);
           strncpy(param->string, line+(*offset), param_len);
           (*offset)+= param_len+1;
           delimiter_found = 1;
       }
   }
}

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
    uint32_t current_param_len = 0;
    size_t params_added = 0;

    for (int i = 0; i < len; i++) {
        if ((buffer[i] == delimiter) || (i == len-1)) {
            if (i == len-1) { // creamos el ultimo parametro
                current_param_len++;
            }
            char* current_param = (char*) malloc(sizeof(char)*(current_param_len));
            strncpy(current_param, buffer + last_delimiter, current_param_len);
            params[params_added].string = current_param;
            params[params_added].len = current_param_len;
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

int call_create(call_t* self) {
    self->id = 0;
    self->endianness = 0;
    call_initialize_param(self, &(self->dest));
    call_initialize_param(self, &(self->path));
    call_initialize_param(self, &(self->interface));
    call_initialize_param(self, &(self->method));
    self->params = NULL;
    
    return 0;
}

int call_fill(call_t* self, char* line, size_t len, uint32_t id) {
    self->id = id;
    /**
     * Sabemos que las lineas vienen bien formadas:
     * <dest> <path> <interface> <method>([<args>])
    */

    int offset = 0;
    param_t args;
   
    call_fill_param_from_line(&(self->dest), line, &offset, ' ', len);
    call_fill_param_from_line(&(self->path), line, &offset, ' ', len);
    call_fill_param_from_line(&(self->interface), line, &offset, ' ', len);
    call_fill_param_from_line(&(self->method), line, &offset, '(', len);
    call_fill_param_from_line(&(args), line, &offset, ')', len);

    if (args.len) {
        call_parameters_parser(self, args.string, args.len, ',');
    } else {
        self->n_params = 0;
        self->params = NULL;
        free(args.string);
    }

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
    
    return 0;
}

// --------------------------------------------------------

