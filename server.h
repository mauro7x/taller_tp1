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
 * Inicializa los atributos correspondientes.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_create(server_t* self, const char* port);


/**
 * Abre el servidor en el puerto especificado.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_open(server_t* self);


/**
 * Acepta la conexión de un cliente.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_accept(server_t* self);


/**
 * Coordina el recibimiento de las calls del cliente, y su procesamiento.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_receive_calls(server_t* self);


/**
 * Apaga el servidor, cerrando el socket aceptador así como el peer.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_shutdown(server_t* self);


/** DESTRUCTOR
 * Libera los recursos utilizados.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int server_destroy(server_t* self);


// ----------------------------------------------------------------------------
#endif // __SERVER_H__
