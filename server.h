#ifndef __SERVER_H__
#define __SERVER_H__

// ----------------------------------------------------------------------------
#include "socket.h"
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct server {
    const char* port;
    const char* hostname;
    socket_t acceptor;        // socket aceptador (S)
    socket_t peer;   // socket correspondiente al cliente (P)
} server_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * @desc:   inicializa los atributos correspondientes.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_create(server_t* self, const char* port);


/**
 * @desc:   abre el servidor en el puerto especificado.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_open(server_t* self);


/**
 * @desc:   acepta la conexión de un cliente.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_accept(server_t* self);


/**
 * @desc:   coordina el recibimiento de las calls del cliente,
 *          y su procesamiento.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_receive_calls(server_t* self);


/**
 * @desc:   apaga el servidor, invocando los métodos de shutdown tanto del
 *          socket aceptador como del socket peer.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_shutdown(server_t* self);


/** DESTRUCTOR
 * @desc:   libera los recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int server_destroy(server_t* self);


// ----------------------------------------------------------------------------
#endif // __SERVER_H__
