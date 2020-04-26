// includes
#include "dbus_parser.h"
#include "call.h"

#include <stdlib.h>
#include <string.h>


// D-Bus Protocol Constants ----------------------------------------------

// header constants
#define ENDIANNESS 'l'
#define BYTE_FOR_METHOD_CALLS 0x01
#define HEADER_FLAGS 0x00
#define PROTOCOL_VERSION 0x01

// byte-counting constants
#define HEADER_DESC_BYTES 16
#define PARAM_DESC_BYTES 8   // bytes que se usan para la descripcion
                                    // de cada parametro
#define DECLARATION_DESC_BYTES 5
#define BODY_ARG_LENGTH_BYTES 4
#define END_BYTE 1

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

// -----------------------------------------------------------------------


// --------------------------------------------------------
// static definitions for parsing

// setting the data types of the params

static void dbus_parser_set_dtypes(dbus_parser_t* self) {
    call_t* call = self->call;
    
    call->endianness = ENDIANNESS;

    call->dest.id = DESTINATION_ID;
    call->dest.data_type = DESTINATION_DATA_TYPE;
    call->path.id = PATH_ID;
    call->path.data_type = PATH_DATA_TYPE;
    call->interface.id = INTERFACE_ID;
    call->interface.data_type = INTERFACE_DATA_TYPE;
    call->method.id = METHOD_ID;
    call->method.data_type = METHOD_DATA_TYPE;
}


// setting array, body, and total len of the msg

static uint32_t dbus_parser_set_declaration_len(size_t n_params, uint32_t* last_padding) {
    uint32_t len = 0;
    if (n_params) {
        len = n_params + DECLARATION_DESC_BYTES + END_BYTE;
        if (len % 8) {
            (*last_padding) = 8 - (len % 8);
            len += (*last_padding);
        }
    }

    return len;
}

static uint32_t dbus_parser_set_param_len(param_t* param, uint32_t* last_padding) {
    uint32_t len = param->len + PARAM_DESC_BYTES + END_BYTE;
    if (len % 8) {
        (*last_padding) = 8 - (len % 8);
        len += (*last_padding);
    }

    return len;
}

static void dbus_parser_set_array_len(dbus_parser_t* self) {
    call_t* call = self->call;
    uint32_t array_len = 0;
    uint32_t last_padding = 0;

    array_len += dbus_parser_set_param_len(&(call->dest), &last_padding);
    array_len += dbus_parser_set_param_len(&(call->path), &last_padding);
    array_len += dbus_parser_set_param_len(&(call->interface), &last_padding);
    array_len += dbus_parser_set_param_len(&(call->method), &last_padding);
    array_len += dbus_parser_set_declaration_len(call->n_params, &last_padding);
    
    array_len -= last_padding; // sacamos el ultimo padding
    self->array_len = array_len;
}

static void dbus_parser_set_body_len(dbus_parser_t* self) {
    uint32_t body_len = 0;
    call_t* call = self->call;

    for (int i = 0; i < call->n_params; i++) {
        body_len += BODY_ARG_LENGTH_BYTES;
        body_len += call->params[i].len + END_BYTE;
    }

    self->body_len = body_len;
}

static void dbus_parser_set_total_length(dbus_parser_t* self) {
    dbus_parser_set_body_len(self);
    dbus_parser_set_array_len(self);

    uint32_t header_length = self->array_len + HEADER_DESC_BYTES;

    if (header_length % 8) {
        header_length += 8 - (header_length % 8); // padding
    }

    self->total_len = header_length + self->body_len;
}


// creating the parsed call msg

static void dbus_parser_copy_to_msg(char* dest, int* offset, void* src, size_t len) {
    memcpy(dest + *offset, src, len);
    (*offset) += len;
}

static void dbus_parser_copy_c_to_msg(char* dest, int* offset, char c, size_t len) {
    memset(dest + *offset, c, len);
    (*offset) += len;
}

static void dbus_parser_copy_param_to_msg(param_t param, char* msg, int* offset) {
    // FORMATO:
    // id,1,datatype,0(padding),len_param(uint32),param(len_param bytes),
    // \0,[padding%8]

    dbus_parser_copy_to_msg(msg, offset, &(param.id), sizeof(param.id));
    dbus_parser_copy_c_to_msg(msg, offset, 1, 1);
    dbus_parser_copy_to_msg(msg, offset, &(param.data_type), sizeof(param.data_type));
    dbus_parser_copy_c_to_msg(msg, offset, '\0', 1);
    
    dbus_parser_copy_to_msg(msg, offset, &(param.len), sizeof(param.len));
    dbus_parser_copy_to_msg(msg, offset, param.string, param.len);
    dbus_parser_copy_c_to_msg(msg, offset, '\0', 1);

    if ((param.len + 1) % 8) {
        size_t bytes_of_padding = 8 - ((param.len + 1) % 8);
        dbus_parser_copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }
}

static void dbus_parser_copy_header_desc_to_msg(dbus_parser_t* self, char* msg, int* offset) {
    // FORMATO:
    // endianness,function,flags,protocol_version,body_length(uint32),
    // id(uint32),array_length(uint32);

    call_t* call = self->call;

    dbus_parser_copy_c_to_msg(msg, offset, ENDIANNESS, 1);
    dbus_parser_copy_c_to_msg(msg, offset, BYTE_FOR_METHOD_CALLS, 1);
    dbus_parser_copy_c_to_msg(msg, offset, HEADER_FLAGS, 1);
    dbus_parser_copy_c_to_msg(msg, offset, PROTOCOL_VERSION, 1);

    dbus_parser_copy_to_msg(msg, offset, &(self->body_len), sizeof(self->body_len));
    dbus_parser_copy_to_msg(msg, offset, &(call->id), sizeof(call->id));
    dbus_parser_copy_to_msg(msg, offset, &(self->array_len), sizeof(self->array_len));
}

static void dbus_parser_copy_declaration_to_msg(dbus_parser_t* self, param_t* param, char* msg, int* offset) {
    // FORMATO:
    // id,1,datatype,0(padding),n_params,'s', ... (n_params times),
    // \0,[padding%8]

    call_t* call = self->call;

    dbus_parser_copy_c_to_msg(msg, offset, DECLARATION_ID, 1);
    dbus_parser_copy_c_to_msg(msg, offset, 1, 1);
    dbus_parser_copy_c_to_msg(msg, offset, DECLARATION_DATA_TYPE, 1);
    dbus_parser_copy_c_to_msg(msg, offset, '\0', 1);

    dbus_parser_copy_c_to_msg(msg, offset, call->n_params, 1);
    for (int i = 0; i < call->n_params; i++) {
        dbus_parser_copy_c_to_msg(msg, offset, 's', 1);
    }
    dbus_parser_copy_c_to_msg(msg, offset, '\0', 1);

    if ((*offset) % 8) {
        size_t bytes_of_padding = 8 - ((*offset) % 8);
        dbus_parser_copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }    

}

static void dbus_parser_copy_header_to_msg(dbus_parser_t* self, char* msg, int* offset) {
    // copiamos los bytes iniciales
    dbus_parser_copy_header_desc_to_msg(self, msg, offset);

    // array de parametros
    call_t* call = self->call;
    dbus_parser_copy_param_to_msg(call->dest, msg, offset);
    dbus_parser_copy_param_to_msg(call->path, msg, offset);
    dbus_parser_copy_param_to_msg(call->interface, msg, offset);
    dbus_parser_copy_param_to_msg(call->method, msg, offset);

    // firma (si hay)
    if (call->n_params) {
        dbus_parser_copy_declaration_to_msg(self, call->params, msg, offset);
    }
}

static void dbus_parser_copy_body_to_msg(dbus_parser_t* self, char* msg, int* offset) {
    call_t* call = self->call;

    for (int i = 0; i < (call->n_params); i++) {
        dbus_parser_copy_to_msg(msg, offset, &(call->params[i].len), sizeof(call->params[i].len));
        dbus_parser_copy_to_msg(msg, offset, call->params[i].string, call->params[i].len);
        dbus_parser_copy_c_to_msg(msg, offset, '\0', 1);
    }
}

static int dbus_parser_create_msg(dbus_parser_t* self) {
    // falta manejar errores
    
    char* msg = (char*) malloc(self->total_len);
    memset(msg, '/', self->total_len);

    int i = 0;
    dbus_parser_copy_header_to_msg(self, msg, &i);
    dbus_parser_copy_body_to_msg(self, msg, &i);

    self->msg = msg;
    return 0;
}


// --------------------------------------------------------
// public definitions

int dbus_parser_create(dbus_parser_t* self, call_t* call) {
    self->call = call;
    dbus_parser_set_dtypes(self);
    dbus_parser_set_total_length(self);
    dbus_parser_create_msg(self);

    return 0;
}

int dbus_parser_destroy(dbus_parser_t* self) {
    if (self->msg) {
        free(self->msg);
    }
    return 0;
}

// --------------------------------------------------------