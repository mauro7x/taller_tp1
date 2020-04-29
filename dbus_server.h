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
 * @desc:   inicializa los atributos según corresponda.
 * @param:  socket peer de donde se reciben los datos de la call.
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_server_create(dbus_server_t* self, socket_t* peer);


/**
 * @desc:   recibe una call del socket peer y la procesa.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_server_recv_call(dbus_server_t* self);


/**
 * @desc:   imprime por stdout la call recibida y procesada respetando el
 *          formato solicitado.
 * @param:  -
 * @return: -
*/
void dbus_server_print_received_call(dbus_server_t* self);


/**
 * @desc:   envia confirmacion al cliente de que se recibió la call.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_server_send_confirmation(dbus_server_t* self);


/** DESTRUCTOR
 * @desc:   libera los recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int dbus_server_destroy(dbus_server_t* self);


// ----------------------------------------------------------------------------
#endif // __DBUS_SERVER_H__
