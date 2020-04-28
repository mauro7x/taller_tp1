#ifndef __DBUS_SERVER_H__
#define __DBUS_SERVER_H__

// ----------------------------------------------------------------------------
#include "call.h"
#include "socket.h"
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct dbus_server {
    call_t call; // call to be received
    socket_t* peer; // peer to receive from
    int has_args;
} dbus_server_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * Inicializa los atributos según corresponda.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_server_create(dbus_server_t* self, socket_t* peer);


/**
 * Recibe una call del socket peer y la procesa.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_server_recv_call(dbus_server_t* self);


/**
 * Imprime por STDOUT la call recibida y procesada segun lo especificado por
 * el enunciado.
*/
void dbus_server_print_received_call(dbus_server_t* self);


/**
 * Envía la confirmación al cliente de que se recibió la call.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_server_send_confirmation(dbus_server_t* self);


/** DESTRUCTOR
 * Libera los recursos utilizados.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_server_destroy(dbus_server_t* self);


// ----------------------------------------------------------------------------
#endif // __DBUS_SERVER_H__
