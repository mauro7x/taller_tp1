// ----------------------------------------------------------------------------
#include "dbus_client.h"
#include "call.h"
#include "socket.h"
#include <stdlib.h>
#include <string.h>
// ----------------------------------------------------------------------------
// Constantes propias del protoclo D-Bus

// Constantes para el armado del header
#define ENDIANNESS 'l'
#define BYTE_FOR_METHOD_CALLS 0x01
#define HEADER_FLAGS 0x00
#define PROTOCOL_VERSION 0x01

// Constantes para el conteo de bytes
#define HEADER_DESC_BYTES 16
#define PARAM_DESC_BYTES 8   // bytes que se usan para la descripcion
                                    // de cada parametro
#define DECLARATION_DESC_BYTES 5
#define BODY_ARG_LENGTH_BYTES 4
#define END_BYTE 1

// Identificadores y tipos de datos del protocolo
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
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// "Métodos" privados
// ----------------------------------------------------------------------------

// De chequeo del endianness:

/**
 * Para que nuestro programa funcione de igual manera en cualquier
 * arquitectura, debemos chequear que se envien las variables numéricas
 * de más de un byte en little-endian, como se pide por enunciado. Para
 * esto, utilizo una serie de funciones que se encargan de determinar el
 * endianness de la arquitectura para posteriormente invertir (o no) los
 * bytes de las variables en cuestión.
*/

/**
 * @desc:   chequea el endianness de la arquitectura actual.
 * @param:  -
 * @return: 0 en caso de ser little endian, 1 en caso de ser big endian.
*/
static int _i_am_big_endian() {
    /**
     * Esta función compara el primer byte de un entero definido como 1,
     * para ver si este primer byte es 1 o no. Si es 0, estamos en little
     * endian. Si es 1, big endian.
     * 
     * Idea sacada de StackOverflow.
    */
   
    int n = 1;
    if (*(char *)&n == 1) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Para invertir los bytes en caso de estar en una arquitectura big-endian,
 * vamos a utilizar la función built-in de GCC, __builtin_bswap32.
*/


// De seteo de tipos de datos:

/**
 * @desc:   settea las constantes propias del protocolo.
 * @param:  -
 * @return: -
*/
static void _set_dtypes(dbus_client_t* self) {
    call_t* call = &(self->call);
    
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


// De cálculo de longitudes:

/**
 * @desc:   calcula el largo en bytes de la firma (de existir).
 * @param:  recibe un puntero a un entero de 32 bytes para retornar cuanto
 *          padding se agregó en la firma.
 * @return: uint32_t con el largo en bytes de la firma.
*/
static uint32_t _set_declaration_len(size_t n_params, uint32_t* last_padding) {
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


/**
 * @desc:   calcula el largo en bytes de un parámetro en particular, como
 *          podría ser por ejemplo <dest>.
 * @param:  recibe un puntero a un entero de 32 bytes para retornar cuanto
 *          padding se agregó en la firma.
 * @return: uint32_t con el largo en bytes de un parametro.
*/
static uint32_t _set_param_len(param_t* param, uint32_t* last_padding) {
    uint32_t len = param->len + PARAM_DESC_BYTES + END_BYTE;
    if (len % 8) {
        (*last_padding) = 8 - (len % 8);
        len += (*last_padding);
    }

    return len;
}


/**
 * @desc:   calcula y establece el largo del array del header en bytes.
 * @param:  -
 * @return: -
*/
static void _set_array_len(dbus_client_t* self) {
    call_t* call = &(self->call);
    uint32_t array_len = 0;
    uint32_t last_padding = 0;

    array_len += _set_param_len(&(call->dest), &last_padding);
    array_len += _set_param_len(&(call->path), &last_padding);
    array_len += _set_param_len(&(call->interface), &last_padding);
    array_len += _set_param_len(&(call->method), &last_padding);
    array_len += _set_declaration_len(call->n_params, &last_padding);
    
    array_len -= last_padding; // sacamos el ultimo padding
    self->array_len = array_len;
}


/**
 * @desc:   calcula y establece el largo del body en bytes.
 * @param:  -
 * @return: -
*/
static void _set_body_len(dbus_client_t* self) {
    uint32_t body_len = 0;
    call_t* call = &(self->call);

    for (int i = 0; i < call->n_params; i++) {
        body_len += BODY_ARG_LENGTH_BYTES;
        body_len += call->params[i].len + END_BYTE;
    }

    self->body_len = body_len;
}


/**
 * @desc:   calcula y establece la longitud total del mensaje.
 * @param:  -
 * @return: -
*/
static void _set_total_length(dbus_client_t* self) {
    _set_body_len(self);
    _set_array_len(self);

    uint32_t header_length = self->array_len + HEADER_DESC_BYTES;

    if (header_length % 8) {
        header_length += 8 - (header_length % 8); // padding
    }

    self->total_len = header_length + self->body_len;
}


// Del procesamiento del mensaje (parseo):

/**
 * @desc:   copia desde un string src al mensaje procesado.
 * @param:  un string a donde copiar, un puntero que apunta desde donde copiar,
 *          un offset que indica desde donde copiar, y una longitud que indica
 *          cuanto copiar.
 * @return: -
*/
static void _copy_to_msg(char* dest, int* offset, void* src, size_t len) {
    memcpy(dest + *offset, src, len);
    (*offset) += len;
}


/**
 * @desc:   copia un caracter específico al mensaje.
 * @param:  un string a donde copiar, un offset que indica desde donde copiar,
 *          el caracter a copiar, y una longitud que indica cuanto copiar.
 * @return: -
*/
static void _copy_c_to_msg(char* dest, int* offset, char c, size_t len) {
    memset(dest + *offset, c, len);
    (*offset) += len;
}


/**
 * @desc:   copia un entero de 4 bytes chequeando el endianness, para que
 *          el mensaje enviado siempre sea little endian.
 * @param:  un string a donde copiar, un puntero al entero a copiar, un offset
 *          que indica desde donde copiar, y una longitud que indica cuanto
 *          copiar.
 * @return: -
*/
static void _copy_uint32_checking_endianness(char* dest, int* offset,
                                             uint32_t* src, size_t len) {
    uint32_t int_to_copy = *src;

    if (_i_am_big_endian()) {
        int_to_copy = __builtin_bswap32(int_to_copy);
    }

    _copy_to_msg(dest, offset, &int_to_copy, len);                                       
}


/**
 * @desc:   copia un parametro al mensaje.
 * @param:  recibe un puntero al parametro a copiar, un string donde copiar,
 *          y un offset que indica desde donde copiar.
 * @return: -
*/
static void _copy_param_to_msg(param_t* param, char* msg, int* offset) {
    // FORMATO:
    // id,1,datatype,0(padding),len_param(uint32),param(len_param bytes),
    // \0,[padding%8]

    _copy_to_msg(msg, offset, &(param->id), sizeof(param->id));
    _copy_c_to_msg(msg, offset, 1, 1);
    _copy_to_msg(msg, offset, &(param->data_type), sizeof(param->data_type));
    _copy_c_to_msg(msg, offset, '\0', 1);
    
    _copy_uint32_checking_endianness(msg, offset, &(param->len),
                                     sizeof(param->len));
    _copy_to_msg(msg, offset, param->data, param->len);
    _copy_c_to_msg(msg, offset, '\0', 1);

    if ((param->len + 1) % 8) {
        size_t bytes_of_padding = 8 - ((param->len + 1) % 8);
        _copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }
}


/**
 * @desc:   copia la firma al mensaje.
 * @param:  string a donde copiar, offset que indica desde donde copiar.
 * @return: -
*/
static void _copy_declaration_to_msg(dbus_client_t* self, char* msg,
                                     int* offset) {
    // FORMATO:
    // id,1,datatype,0(padding),n_params,'s', ... (n_params times),
    // \0,[padding%8]

    call_t* call = &(self->call);

    _copy_c_to_msg(msg, offset, DECLARATION_ID, 1);
    _copy_c_to_msg(msg, offset, 1, 1);
    _copy_c_to_msg(msg, offset, DECLARATION_DATA_TYPE, 1);
    _copy_c_to_msg(msg, offset, '\0', 1);

    _copy_c_to_msg(msg, offset, call->n_params, 1);
    for (int i = 0; i < call->n_params; i++) {
        _copy_c_to_msg(msg, offset, 's', 1);
    }
    _copy_c_to_msg(msg, offset, '\0', 1);

    if ((*offset) % 8) {
        size_t bytes_of_padding = 8 - ((*offset) % 8);
        _copy_c_to_msg(msg, offset, '\0', bytes_of_padding);
    }    
}


/**
 * @desc:   copia la descripción del header al mensaje.
 * @param:  string a donde copiar, offset que indica desde donde copiar.
 * @return: -
*/
static void _copy_header_desc_to_msg(dbus_client_t* self, char* msg,
                                     int* offset) {
    // FORMATO:
    // endianness,function,flags,protocol_version,body_length(uint32),
    // id(uint32),array_length(uint32);

    call_t* call = &(self->call);

    _copy_c_to_msg(msg, offset, ENDIANNESS, 1);
    _copy_c_to_msg(msg, offset, BYTE_FOR_METHOD_CALLS, 1);
    _copy_c_to_msg(msg, offset, HEADER_FLAGS, 1);
    _copy_c_to_msg(msg, offset, PROTOCOL_VERSION, 1);

    _copy_uint32_checking_endianness(msg, offset, &(self->body_len), sizeof(self->body_len));
    _copy_uint32_checking_endianness(msg, offset, &(call->id), sizeof(call->id));
    _copy_uint32_checking_endianness(msg, offset, &(self->array_len), sizeof(self->array_len));
}


/**
 * @desc:   copia el header al mensaje.
 * @param:  string a donde copiar, offset que indica desde donde copiar.
 * @return: -
*/
static void _copy_header_to_msg(dbus_client_t* self, char* msg, int* offset) {
    // copiamos los bytes iniciales
    _copy_header_desc_to_msg(self, msg, offset);

    // array de parametros
    call_t* call = &(self->call);
    _copy_param_to_msg(&(call->dest), msg, offset);
    _copy_param_to_msg(&(call->path), msg, offset);
    _copy_param_to_msg(&(call->interface), msg, offset);
    _copy_param_to_msg(&(call->method), msg, offset);

    // firma (si hay)
    if (call->n_params) {
        _copy_declaration_to_msg(self, msg, offset);
    }
}


/**
 * @desc:   copia el body al mensaje.
 * @param:  string a donde copiar, offset que indica desde donde copiar.
 * @return: -
*/
static void _copy_body_to_msg(dbus_client_t* self, char* msg, int* offset) {
    call_t* call = &(self->call);

    for (int i = 0; i < (call->n_params); i++) {
        _copy_to_msg(msg, offset, &(call->params[i].len),
                     sizeof(call->params[i].len));
        _copy_to_msg(msg, offset, call->params[i].data, call->params[i].len);
        _copy_c_to_msg(msg, offset, '\0', 1);
    }
}


/**
 * @desc:   coordina la creacion del mensaje procesado listo para enviar.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
static int _create_msg(dbus_client_t* self) {
    char* msg = (char*) malloc(self->total_len);
    if (msg == NULL) {
        fprintf(stderr, "Error in function: _create_msg. Error: malloc.\n");
        return -1;
    }
 
    int i = 0;
    _copy_header_to_msg(self, msg, &i);
    _copy_body_to_msg(self, msg, &i);
    self->msg = msg;

    return 0;
}


// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int dbus_client_create(dbus_client_t* self, socket_t* socket) {
    call_create(&(self->call));
    self->socket = socket;
    self->msg = NULL;
    self->total_len = 0;
    self->array_len = 0;
    self->body_len = 0;

    return 0;
}


int dbus_client_fill(dbus_client_t* self, char* buffer, size_t len, int id) {
    if (call_fill(&(self->call), buffer, len, id)) {
        return -1;
    }

    _set_dtypes(self);
    _set_total_length(self);
    
    if (_create_msg(self)) {
        return -1;
    }

    return 0;
}


int dbus_client_send_call(dbus_client_t* self) {
    int s;

    s = socket_send(self->socket, self->msg, (size_t) self->total_len);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: dbus_client_send_call. "
                        "Socket was closed before expected.\n");
        return -1;
    }

    return 0;
}


int dbus_client_print_server_reply(dbus_client_t* self) {
    int s;
    char reply[3] = "";
    s = socket_recv(self->socket, &(reply[0]), 3);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: dbus_client_print_server_reply. "
                        "Socket was closed before expected.\n");
        return -1;
    }
    
    printf("0x%04x: %s\n", self->call.id, reply);

    return 0;
}


int dbus_client_destroy(dbus_client_t* self) {
    call_destroy(&(self->call));

    if (self->msg) {
        free(self->msg);
    }
    return 0;
}

// ----------------------------------------------------------------------------
