// includes
#include "server.h"
#include "socket.h"
#include "call.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>


// defines
#define MAX_CLIENTS_IN_QUEUE 10
#define HOSTNAME 0
#define SOCKET_HAS_BEEN_CLOSED 1

// protocol identifiers
#define DESTINATION_ID 6
#define PATH_ID 1
#define INTERFACE_ID 2
#define METHOD_ID 3
#define DECLARATION_ID 8


// --------------------------------------------------------
// static definitions

// for receiving bytes

static int server_discard_n_bytes(server_t* self, int n) {
    char thrash;
    int s;
    for (int i = 0; i < n; i++) {
        s = socket_recv(&(self->peer), &thrash, 1);
        if (s == -1) {
            return -1;
        } else if (s == 0) {
            fprintf(stderr, "Error in function: server_discard_n_bytes_from_peer. "
                            "Socket was closed before expected.");
            return -1;
        } 
    }

    return 0;
}

static int server_receive_numeric_byte(server_t* self, size_t* dest) {
    char val;
    int s;
    s = socket_recv(&(self->peer), &val, 1);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: server_discard_n_bytes_from_peer. "
                        "Socket was closed before expected.");
        return -1;
    } 
    *dest = val;
    return 0;
}

static int server_receive_uint32_value(server_t* self, uint32_t* dest) {
    int s;
    size_t size = sizeof((*dest));
    char* aux = (char*) malloc(size);

    s = socket_recv(&(self->peer), aux, size);
    if (s == -1) {
        free(aux);
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: server_receive_uint32_value. "
                        "Socket was closed before expected.");
        free(aux);
        return -1;
    } 

    memcpy(dest, aux, size);
    free(aux);

    return 0;
}


// for filling params

// ojo, malloc. agrega el \0
static int server_fill_param_string(server_t* self, param_t* param, uint32_t n) {
    int s;
    char* msg = (char*) malloc(sizeof(char)*(n+1));
    
    s = socket_recv(&(self->peer), msg, n+1);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: server_receive_uint32_value. "
                        "Socket was closed before expected.");
        return -1;
    }

    param->string = msg;
    return 0;
}


static int server_fill_declaration(server_t* self, call_t* call) {
    // Descartamos los 3 bytes que siguen, no nos sirven
    if (server_discard_n_bytes(self, 3)) {
        return -1;
    }

    // recibir n_params
    if (server_receive_numeric_byte(self, &(call->n_params))) {
        return -1;
    }

    // Descartamos los proximos n_params+1(\0)+padding bytes
    int bytes_to_discard = call->n_params + 1;
    if ((bytes_to_discard+5) % 8) {
        bytes_to_discard += 8 - ((bytes_to_discard+5) % 8);
    }

    if (server_discard_n_bytes(self, bytes_to_discard)) {
        return -1;
    }

    return 0;
}


static int server_fill_param(server_t* self, param_t* param) {
    // Descartamos los 3 bytes que siguen, no nos sirven
    if (server_discard_n_bytes(self, 3)) {
        return -1;
    }

    if (server_receive_uint32_value(self, &(param->len))) {
        return -1;
    }

    if (server_fill_param_string(self, param, param->len)) {
        return -1;
    }

    // Verificamos si hay que descartar padding
    if ((param->len+1) % 8) {
        int padding_bytes = 8 - ((param->len+1) % 8);
        if (server_discard_n_bytes(self, padding_bytes)) {
            return -1;
        }
    }

    return 0;
}


static int server_fill_specific_param(server_t* self, call_t* call, char type) {
    switch (type) {
            case DESTINATION_ID:
                if (server_fill_param(self, &(call->dest))) {
                    return -1;
                }
                break;

            case PATH_ID:
                if (server_fill_param(self, &(call->path))) {
                    return -1;
                }
                break;

            case INTERFACE_ID:
                if (server_fill_param(self, &(call->interface))) {
                    return -1;
                }
                break;

            case METHOD_ID:
                if (server_fill_param(self, &(call->method))) {
                    return -1;
                }
                break;

            case DECLARATION_ID:
                if (server_fill_declaration(self, call)) {
                    return -1;
                }
                break;
        }

    return 0;
}


static int server_fill_call_params(server_t* self, call_t* call, int has_declaration) {
    /**
     * Los parametros no necesariamente llegan en orden, pero los identificamos
     * por el byte de tipo que nos dice que es. Lo que si sabemos es que, como
     * las lineas estan bien formadas, nos llegaran 4 parametros, y opcionalmente
     * una firma del metodo.
     * 
     * Tambien sabemos que se trata de strings, ya que en el enunciado asi lo detalla.
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
        s = socket_recv(&(self->peer), &type, 1);
        if (s == -1) {
            return -1;
        } else if (s == 0) {
            fprintf(stderr, "Error in function: server_receive_uint32_value. "
                            "Socket was closed before expected.");
            return -1;
        }
        if (server_fill_specific_param(self, call, type)) {
            return -1;
        }
    }
    
    return 0;
}


static int server_fill_call_declaration(server_t* self, call_t* call, int has_declaration) {
    if (!has_declaration) {
        return 0;
    }

    call->params = (param_t*) malloc(sizeof(param_t)*(call->n_params));
    for (int i = 0; i < call->n_params; i++) {

        if (server_receive_uint32_value(self, &(call->params[i].len))) {
            return -1;
        }

        if (server_fill_param_string(self, &(call->params[i]), call->params[i].len)) {
            return -1;
        }
    }

    return 0;
}


static int server_fill_call_from_peer(server_t* self, call_t* call) {
    // Descartamos los proximos 3 bytes, no nos sirven para el TP
    if (server_discard_n_bytes(self, 3)) {
        return -1;
    }

    uint32_t body_len;
    uint32_t array_len;
    uint32_t id_call;

    server_receive_uint32_value(self, &body_len);
    server_receive_uint32_value(self, &id_call);
    server_receive_uint32_value(self, &array_len);
    call->id = id_call; // llenamos el id

    if (server_fill_call_params(self, call, (body_len > 0))) {
        return -1;
    }

    if (server_fill_call_declaration(self, call, (body_len > 0))) {
        return -1;
    }
    
    return 0;
}


// for printing the received call

static void server_print_received_call(call_t* received_call) {
    printf("* Id: 0x%04x\n", received_call->id);
    printf("* Destino: %s\n", received_call->dest.string);
    printf("* Path: %s\n", received_call->path.string);
    printf("* Interface: %s\n", received_call->interface.string);
    printf("* Método: %s\n", received_call->method.string);

    if (received_call->n_params) {
        printf("* Parámetros:\n");
        for (int i = 0; i < received_call->n_params; i++) {
            printf("    * %s\n", received_call->params[i].string);
        }
    }

    printf("\n");

}

// for sending answer back
static int server_send_recv_confirmation(server_t* self) {

    int s;
    char* confirmation = "OK";
    s = socket_send(&(self->peer), confirmation, 3);
    if (s == -1) {
        return -1;
    } else if (s == 0) {
        fprintf(stderr, "Error in function: server_send_recv_confirmation. "
                        "Socket was closed before expected.\n");
        return -1;
    }

    return 0;
}


// receiving one call

static int server_receive_and_process_call(server_t* self) {
    // Leemos el primer byte para verificar que se enviara otra call
    int s;
    char r;
    s = socket_recv(&(self->peer), &r, sizeof(r));

    if (s == 0) {
        return SOCKET_HAS_BEEN_CLOSED;
    } else if (s == -1) {
        return -1;
    }

    call_t call;
    call_create(&call);
    
    if (server_fill_call_from_peer(self, &call)) {
        call_destroy(&call);
        return -1;
    }

    // aca tenemos que responder e imprimir.
    server_print_received_call(&call);
    server_send_recv_confirmation(self);
    
    call_destroy(&call);
    return 0;
}


// --------------------------------------------------------
// public definitions

int server_create(server_t* self, const char* port) {
    self->port = port;
    self->hostname = HOSTNAME;

    // socket aceptador, y socket peer
    socket_t socket, peer_socket;
    if (socket_create(&socket)) {
        fprintf(stderr, "Error in function: socket_create.\n");
        return -1;
    }
    if (socket_create(&peer_socket)) {
        fprintf(stderr, "Error in function: socket_create.\n");
        return -1;
    }
    self->acceptor = socket; 
    self->peer = peer_socket;

    return 0;
}

int server_open(server_t* self) {

    if (socket_get_addresses(&(self->acceptor), self->hostname, self->port, true)) {
        return -1;
    }

    if (socket_bind(&(self->acceptor), self->port)) {
        return -1;
    }

    if (socket_listen(&(self->acceptor), MAX_CLIENTS_IN_QUEUE)) {
        return -1;
    }

    return 0;

}

int server_accept(server_t* self) {
    if (socket_accept(&(self->acceptor), &(self->peer))) {
        fprintf(stderr, "Error acceptando conexion entrante.");
        return -1;
    }
    return 0;
}

int server_receive_calls(server_t* self) {
    int s;

    do {
        s = server_receive_and_process_call(self);
    } while (s == 0);

    if (s != SOCKET_HAS_BEEN_CLOSED) {
        fprintf(stderr, "Error in function: server_receive_call.\n");
        return -1;
    }

    return 0;
}

int server_shutdown(server_t* self) {
    if (socket_shutdown(&(self->acceptor))) {
        fprintf(stderr, "Error in function: socket_shutdown (S)\n");
        return -1;
    }

    if (socket_shutdown(&(self->peer))) {
        fprintf(stderr, "Error in function: socket_shutdown (P)\n");
        return -1;
    }

    return 0;
}

int server_destroy(server_t* self) {
    socket_destroy(&(self->acceptor));
    socket_destroy(&(self->peer));
    return 0;
}

// --------------------------------------------------------
