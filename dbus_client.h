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
 * @desc:   inicializa la estructura de datos según corresponde.
 * @param:  socket a través del cual enviar bytes al servidor.
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_client_create(dbus_client_t* self, socket_t* socket);


/**
 * @desc:   se encarga de llenar la estructura de datos.
 * @param:  linea de texto plano que contiene a la call a serializar,
 *          longitud de la misma, id de la call.
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_client_fill(dbus_client_t* self, char* buffer, size_t len, int id);


/**
 * @desc:   envia la call procesada al servidor.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_client_send_call(dbus_client_t* self);


/**
 * @desc:   imprime por stdout la respuesta del servidor tras enviar la call.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_client_print_server_reply(dbus_client_t* self);


/** DESTRUCTOR
 * @desc:   libera los recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_client_destroy(dbus_client_t* self);


// ----------------------------------------------------------------------------
#endif // __DBUS_CLIENT_H__
