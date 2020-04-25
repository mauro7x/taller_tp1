// includes
#include "call.h"
#include "stdin_streamer.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// defines
#define PARAMS_DELIMITER ','
#define EOF_ERROR 1

// D-BUS CONSTANTS
// header constants
#define ENDIANNESS 'l'
#define BYTE_FOR_METHOD_CALLS 0x01
#define HEADER_FLAGS 0x00
#define PROTOCOL_VERSION 0x01

// byte-counting constants
#define PARAM_DESC_BYTES 8   // bytes que se usan para la descripcion
                                    // de cada parametro
#define DECLARATION_DESC_BYTES 5
#define END_BYTE 1
#define BODY_ARG_LENGTH_BYTES 4

// protocol data types and identifiers
#define DESTINATION_ID 6
#define DESTINATION_DATA_TYPE 's'
#define PATH_ID 1
#define PATH_DATA_TYPE 'o'
#define INTERFACE_ID 2
#define INTERFACE_DATA_TYPE 's'
#define METHOD_ID 3
#define METHOD_DATA_TYPE 's'
#define DECLARATION_ID 8
#define DECLARATION_DATA_TYPE 'g'


// --------------------------------------------------------
// static definitions

static void call_set_types(call_t* self) {
    // acorde al protocolo D-Bus
    self->dest.id = DESTINATION_ID;
    self->dest.data_type = DESTINATION_DATA_TYPE;
    self->path.id = PATH_ID;
    self->path.data_type = PATH_DATA_TYPE;
    self->interface.id = INTERFACE_ID;
    self->interface.data_type = INTERFACE_DATA_TYPE;
    self->method.id = METHOD_ID;
    self->method.data_type = METHOD_DATA_TYPE;
}

// --------------------------------------------------------
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
// call process


static void call_set_array_len(call_t* self) {
    size_t dest_length = self->dest.len + PARAM_DESC_BYTES + END_BYTE;
    if (dest_length % 8) {
        dest_length += 8 - (dest_length % 8); // padding
    }

    size_t path_length = self->path.len + PARAM_DESC_BYTES + END_BYTE;
    if (path_length % 8) {
        path_length += 8 - (path_length % 8); // padding
    }
 
    size_t interface_length = self->interface.len + PARAM_DESC_BYTES + END_BYTE;
    if (interface_length % 8) {
        interface_length += 8 - (interface_length % 8); // padding
    }
   
    size_t last_padding = 0;

    size_t method_length = self->method.len + PARAM_DESC_BYTES + END_BYTE;
    if (method_length % 8) {
        last_padding = 8 - (method_length % 8); // padding
        method_length += last_padding;
    }
  
    size_t params_length = 0;
    if (self->n_params) {
        params_length = self->n_params + DECLARATION_DESC_BYTES + END_BYTE;
        if (params_length % 8) {
            last_padding = 8 - (params_length % 8);
            params_length += last_padding;
        }
    }

    self->array_len = (uint32_t) (dest_length + path_length + interface_length + 
                        method_length + params_length - last_padding);
}

static void call_set_body_len(call_t* self) {
    /**
     * ESTO ES UNA PLENA DEDUCCION PERSONAL, EN EL ENUNCIADO NO DICE NADA
     * El formato del body es:
     * por cada arg se agrega: UINT32 (longitud del arg), ARG + \0 + padding a %4
     * en el ultimo argumento no se pone el padding.
     */

    size_t body_length = 0;
    size_t last_padding = 0;
    for (int i = 0; i < self->n_params; i++) {
        body_length += BODY_ARG_LENGTH_BYTES;
        body_length += self->params[i].len + END_BYTE;
        if (body_length % 4) {
            last_padding = 4 - (body_length % 4);
            body_length += last_padding;
        }
    }
    body_length -= last_padding;

    self->body_len = (uint32_t) body_length;
}

static void call_set_total_length(call_t* self) {
    call_set_array_len(self);
    call_set_body_len(self);

    size_t header_length = self->array_len + 16;

    if (header_length % 8) {
        header_length += 8 - (header_length % 8); // padding
    }

    self->total_len = (uint32_t) header_length + self->body_len;
}

static void call_copy_to_msg(char* dest, int* offset, void* src, size_t len) {
    memcpy(dest + *offset, src, len);
    (*offset) += len;
}

static void call_copy_c_to_msg(char* dest, int* offset, char c, size_t len) {
    memset(dest + *offset, c, len);
    (*offset) += len;
}

static void call_copy_param_to_msg(call_t* self, param_t param, char* msg, int* offset) {
    /** 
     * FORMATO:
     * id,1,datatype,0(padding),len_param(uint32),param(len_param bytes),
     * \0,[padding%8]
    */

    call_copy_to_msg(msg, offset, &(param.id), sizeof(param.id));
    call_copy_c_to_msg(msg, offset, '1', 1);
    call_copy_to_msg(msg, offset, &(param.data_type), sizeof(param.data_type));
    call_copy_c_to_msg(msg, offset, '\0', 1);

    call_copy_to_msg(msg, offset, &(param.len), sizeof(param.len));
    call_copy_to_msg(msg, offset, param.string, param.len);
    call_copy_c_to_msg(msg, offset, '\0', 1);

    if ((param.len + 1) % 8) {
        size_t bytes_of_padding = 8 - ((param.len + 1) % 8);
        call_copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }
}

static void call_copy_header_desc_to_msg(call_t* self, char* msg, int* offset) {
    /** 
     * FORMATO:
     * endianness,function,flags,protocol_version,body_length(uint32),
     * id(uint32),array_length(uint32);
    */

    call_copy_c_to_msg(msg, offset, ENDIANNESS, 1);
    call_copy_c_to_msg(msg, offset, BYTE_FOR_METHOD_CALLS, 1);
    call_copy_c_to_msg(msg, offset, HEADER_FLAGS, 1);
    call_copy_c_to_msg(msg, offset, PROTOCOL_VERSION, 1);

    call_copy_to_msg(msg, offset, &(self->body_len), sizeof(self->body_len));
    call_copy_to_msg(msg, offset, &(self->id), sizeof(self->id));
    call_copy_to_msg(msg, offset, &(self->array_len), sizeof(self->array_len));
}

static void call_copy_declaration_to_msg(call_t* self, param_t* param, char* msg, int* offset) {
    /** 
     * FORMATO:
     * id,1,datatype,0(padding),n_params,'s', ... (n_params times),
     * \0,[padding%8]
    */

    call_copy_c_to_msg(msg, offset, DECLARATION_ID, 1);
    call_copy_c_to_msg(msg, offset, '1', 1);
    call_copy_c_to_msg(msg, offset, DECLARATION_DATA_TYPE, 1);
    call_copy_c_to_msg(msg, offset, '\0', 1);

    call_copy_c_to_msg(msg, offset, self->n_params, 1);
    for (int i = 0; i < self->n_params; i++) {
        call_copy_c_to_msg(msg, offset, 's', 1);
    }
    call_copy_c_to_msg(msg, offset, '\0', 1);

    if ((*offset) % 8) {
        size_t bytes_of_padding = 8 - ((*offset) % 8);
        call_copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }    

}

static void call_copy_header_to_msg(call_t* self, char* msg, int* offset) {
    // copiamos los bytes iniciales
    call_copy_header_desc_to_msg(self, msg, offset);

    // array de parametros
    call_copy_param_to_msg(self, self->dest, msg, offset);
    call_copy_param_to_msg(self, self->path, msg, offset);
    call_copy_param_to_msg(self, self->interface, msg, offset);
    call_copy_param_to_msg(self, self->method, msg, offset);

    // firma (si hay)
    if (self->n_params) {
        call_copy_declaration_to_msg(self, self->params, msg, offset);
    }
    
}

static void call_copy_body_to_msg(call_t* self, char* msg, int* offset) {
    // al ultimo argumento no se le pone padding
    
    for (int i = 0; i < (self->n_params); i++) {
        call_copy_to_msg(msg, offset, &(self->params[i].len), sizeof(self->params[i].len));
        call_copy_to_msg(msg, offset, self->params[i].string, self->params[i].len);
        call_copy_c_to_msg(msg, offset, '\0', 1);

        if (((self->params[i].len + 1) % 4) && (i != ((self->n_params) - 1))) {
            size_t bytes_of_padding = 4 - ((self->params[i].len + 1) % 4);
            call_copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
        }
    }

}

static int call_process(call_t* self) {
    char* msg = (char*) malloc(self->total_len);
    
    memset(msg, '/', self->total_len);

    int i = 0;
    call_copy_header_to_msg(self, msg, &i);
    call_copy_body_to_msg(self, msg, &i);

    self->msg = msg;
    return 0;
}



// --------------------------------------------------------
// public api


int call_create(call_t* self, uint32_t id, stdin_streamer_t* streamer) {
    self->already_filled = 0;
    self->id = id;

    call_set_types(self);
    
    if (stdin_streamer_run(streamer, self)) {
        return EOF_ERROR; // eof
    }

    call_set_total_length(self);

    if (call_process(self)) {
        return -1;
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
    
    if (self->msg) {
        free(self->msg);
    }
    

    return 0;
}


// --------------------------------------------------------

