#ifndef __DBUS_CLIENT_H__
#define __DBUS_CLIENT_H__

// ----------------------------------------------------------------------------
#include "call.h"
#include "socket.h"
#include <stdint.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct dbus_client {
    call_t call; // call to be parsed
    socket_t* socket; // socket to send msg to

    char* msg; // parsed msg
    uint32_t total_len;
    uint32_t array_len;
    uint32_t body_len;
} dbus_client_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * Inicializa la estructura de datos según corresponde.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_client_create(dbus_client_t* self, socket_t* socket);


/**
 * Se encarga de llenar la estructura de datos desde una línea recibida.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_client_fill(dbus_client_t* self, char* buffer, size_t len, int id);


/**
 * Envía la call procesada al servidor.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_client_send_call(dbus_client_t* self);


/**
 * Imprime por STDOUT la respuesta del servidor en el formato pedido.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_client_print_server_reply(dbus_client_t* self);


/** DESTRUCTOR
 * Libera los recursos utilizados.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int dbus_client_destroy(dbus_client_t* self);


// ----------------------------------------------------------------------------
#endif // __DBUS_CLIENT_H__
