// ----------------------------------------------------------------------------
#include "dbus_server.h"
#include "call.h"
#include "socket.h"
#include <stdlib.h>
#include <string.h>
// ----------------------------------------------------------------------------
// Constantes propias del protoclo D-Bus

#define DESTINATION_ID 6
#define PATH_ID 1
#define INTERFACE_ID 2
#define METHOD_ID 3
#define DECLARATION_ID 8
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


// De recibo de bytes desde el socket peer:

/**
 * @desc:   recibe n bytes y los descarta.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
static int _discard_n_bytes(dbus_server_t* self, int n) {
    char thrash;
    int s;
    for (int i = 0; i < n; i++) {
        s = socket_recv(self->peer, &thrash, 1);
        if (s == -1) {
            return -1;
        } else if (s == 0) {
            fprintf(stderr, "Error in function: _discard_n_bytes. "
                            "Socket was closed before expected.\n");
            return -1;
        } 
    }

    return 0;
}


/**
 * @desc:   recibe un byte del cliente y lo guarda en un size_t.
 * @param:  puntero al size_t destino.
 * @return: 0 si no hay err_receive_uint32_valueores, -1 CC.
*/
static int _receive_sizet_value(dbus_server_t* self, size_t* dest) {
    char val;
    int s;
    s = socket_recv(self->peer, &val, 1);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: _receive_sizet_value. "
                        "Socket was closed before expected.\n");
        return -1;
    } 
    *dest = val;
    return 0;
}


/**
 * @desc:   recibe un uint32_t del cliente.
 * @param:  puntero al uint32_t destino.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _receive_uint32_value(dbus_server_t* self, uint32_t* dest) {
    int s;
    size_t size = sizeof((*dest));
    char* aux = (char*) malloc(size);

    s = socket_recv(self->peer, aux, size);
    if (s == -1) {
        free(aux);
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: _receive_uint32_value. "
                        "Socket was closed before expected.\n");
        free(aux);
        return -1;
    } 

    memcpy(dest, aux, size);
    free(aux);

    return 0;
}


/**
 * @desc:   recibe un entero de 4 bytes chequeando el endianness, para que
 *          el mensaje enviado siempre sea little endian.
 * @param:  puntero al uint32_t destino.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _receive_uint32_checking_endianness(dbus_server_t* self,
                                                uint32_t* dest) {
    uint32_t int_to_receive;

    if (_receive_uint32_value(self, &int_to_receive)) {
        return -1;
    }

    if (_i_am_big_endian()) {
        int_to_receive = __builtin_bswap32(int_to_receive);
    }

    (*dest) = int_to_receive;

    return 0;
}


// Del llenado de la estructura de datos:

/**
 * @desc:   llena el string de un parametro en la estructura.
 * @param:  recibe un puntero al parametro a recibir, y su longitud, incluyendo
 *          el byte final \0.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_param_data(dbus_server_t* self, param_t* param, uint32_t n) {
    int s;
    char* msg = (char*) malloc(sizeof(char)*(n+1));
    
    s = socket_recv(self->peer, msg, n+1);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: _fill_param_data. "
                        "Socket was closed before expected.\n");
        return -1;
    }

    param->data = msg;
    return 0;
}


/**
 * @desc:   llena la firma en la estructura.
 * @param:  recibe un puntero a la call a llenar.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_declaration(dbus_server_t* self, call_t* call) {
    // Descartamos los 3 bytes que siguen, no nos sirven
    if (_discard_n_bytes(self, 3)) {
        return -1;
    }

    // recibir n_params
    if (_receive_sizet_value(self, &(call->n_params))) {
        return -1;
    }

    // Descartamos los proximos n_params+1(\0)+padding bytes
    int bytes_to_discard = call->n_params + 1;
    if ((bytes_to_discard+5) % 8) {
        bytes_to_discard += 8 - ((bytes_to_discard+5) % 8);
    }

    if (_discard_n_bytes(self, bytes_to_discard)) {
        return -1;
    }

    return 0;
}


/**
 * @desc:   coordina como se llena un parametro de la estructura.
 * @param:  recibe un puntero al parametro a llenar.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_param(dbus_server_t* self, param_t* param) {
    // Descartamos los 3 bytes que siguen, no nos sirven
    if (_discard_n_bytes(self, 3)) {
        return -1;
    }

    if (_receive_uint32_checking_endianness(self, &(param->len))) {
        return -1;
    }

    if (_fill_param_data(self, param, param->len)) {
        return -1;
    }

    // Verificamos si hay que descartar padding
    if ((param->len+1) % 8) {
        int padding_bytes = 8 - ((param->len+1) % 8);
        if (_discard_n_bytes(self, padding_bytes)) {
            return -1;
        }
    }

    return 0;
}


/**
 * @desc:   identifica el tipo de parámetro a recibir según su ID.
 * @param:  un caracter que identifica a cada parametro, un puntero a la
 *          call a llenar.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_specific_param(dbus_server_t* self, call_t* call, char type) {
    switch (type) {
            case DESTINATION_ID:
                if (_fill_param(self, &(call->dest))) {
                    return -1;
                }
                break;

            case PATH_ID:
                if (_fill_param(self, &(call->path))) {
                    return -1;
                }
                break;

            case INTERFACE_ID:
                if (_fill_param(self, &(call->interface))) {
                    return -1;
                }
                break;

            case METHOD_ID:
                if (_fill_param(self, &(call->method))) {
                    return -1;
                }
                break;

            case DECLARATION_ID:
                if (_fill_declaration(self, call)) {
                    return -1;
                }
                break;
        }

    return 0;
}


/**
 * @desc:   coordina el recibimiento de los 4 o 5 parametros de la call.
 * @param:  puntero apuntando la call a llenar.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_call_params(dbus_server_t* self, call_t* call,
                             int has_declaration) {
    /**
     * Los parametros no necesariamente llegan en orden, pero los identificamos
     * por el byte de tipo que nos dice que es. Lo que si sabemos es que como
     * las lineas estan bien formadas nos llegaran 4 parametros, y
     * opcionalmente, una firma del metodo.
     * 
     * Sabemos que se trata de strings, ya que en el enunciado así lo detalla.
    */
    int s;

    size_t params_to_fill;
    if (has_declaration) {
        params_to_fill = 5;
    } else {
        params_to_fill = 4;
        call->n_params = 0;
    }
    
    char type;
    for (int i = 0; i < params_to_fill; i++) {
        s = socket_recv(self->peer, &type, 1);
        if (s == -1) {
            return -1;
        } else if (s == 0) {
            fprintf(stderr, "Error in function: _fill_call_params. "
                            "Socket was closed before expected.\n");
            return -1;
        }
        if (_fill_specific_param(self, call, type)) {
            return -1;
        }
    }
    
    return 0;
}


/**
 * @desc:   coordina el recibimiento de la firma.
 * @param:  un puntero a la call a llenar, un int que indica si tiene firma.
 * @return: 0 si no hay errores, -1 CC.
*/
static int _fill_call_declaration(dbus_server_t* self, call_t* call,
                                  int has_declaration) {
    if (!has_declaration) {
        return 0;
    }

    call->params = (param_t*) malloc(sizeof(param_t)*(call->n_params));
    for (int i = 0; i < call->n_params; i++) {
        if (_receive_uint32_checking_endianness(self, &(call->params[i].len))) {
            return -1;
        }

        if (_fill_param_data(self, &(call->params[i]), call->params[i].len)) {
            return -1;
        }
    }

    return 0;
}


// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int dbus_server_create(dbus_server_t* self, socket_t* peer) {
    self->peer = peer;
    self->has_args = 0;
    call_create(&(self->call));

    return 0;
}


int dbus_server_recv_call(dbus_server_t* self) {
    // Descartamos los proximos 3 bytes, no nos sirven
    if (_discard_n_bytes(self, 3)) {
        return -1;
    }

    uint32_t body_len;
    uint32_t id_call;

    if (_receive_uint32_checking_endianness(self, &body_len)) {
        return -1;
    }

    if (_receive_uint32_checking_endianness(self, &id_call)) {
        return -1;
    }

    self->call.id = id_call; // llenamos el id
    self->has_args = (body_len > 0);

    // descartamos el array ley, no nos interesa
    if (_discard_n_bytes(self, 4)) {
        return -1;
    } 

    if (_fill_call_params(self, &(self->call), self->has_args)) {
        return -1;
    }

    if (_fill_call_declaration(self, &(self->call), self->has_args)) {
        return -1;
    }

    return 0;
}


void dbus_server_print_received_call(dbus_server_t* self) {
    printf("* Id: 0x%08x\n", self->call.id);
    printf("* Destino: %s\n", self->call.dest.data);
    printf("* Ruta: %s\n", self->call.path.data);
    printf("* Interfaz: %s\n", self->call.interface.data);
    printf("* Metodo: %s\n", self->call.method.data);

    if (self->call.n_params) {
        printf("* Parametros:\n");
        for (int i = 0; i < self->call.n_params; i++) {
            printf("    * %s\n", self->call.params[i].data);
        }
    }
    printf("\n");
}


int dbus_server_send_confirmation(dbus_server_t* self) {
    int s;
    char* confirmation = "OK";

    s = socket_send(self->peer, confirmation, 3);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: server_send_recv_confirmation. "
                        "Socket was closed before expected.\n");
        return -1;
    }

    return 0;
}


int dbus_server_destroy(dbus_server_t* self) {
    call_destroy(&(self->call));
    return 0;
}


// ----------------------------------------------------------------------------
