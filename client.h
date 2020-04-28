#ifndef __CLIENT_H__
#define __CLIENT_H__

// ----------------------------------------------------------------------------
#include "socket.h"
#include <stdint.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct client {
    uint32_t next_msg_id; 
    const char* port;
    const char* hostname;
    socket_t socket;
} client_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * Se encarga de inicializar el cliente llenando sus atributos.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_create(client_t* self, const char* hostname, const char* port);


/**
 * Se la llama cuando se proporciona un archivo de entrada, se encarga
 * de abrirlo y setearlo como stdin para el resto de la ejecución.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_set_input_file(const char* path_to_file);


/**
 * Intenta conectarse al servidor, primero obteniendo las direcciones
 * posibles y luego probando con cada una de ellas.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_connect(client_t* self);


/**
 * Se encarga de coordinar el procesamiento de las calls recibidas
 * por la entrada, para luego enviarlas al servidor y recibir la
 * respuesta del mismo por cada call enviada.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_send_calls(client_t* self);


/** CALLBACK del STDIN_STREAMER.
 * Se encarga del envio de una call en particular, recibiendo por
 * argumentos una linea con texto plano de la misma.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_send_call(void* context, char* buffer, size_t len);


/**
 * Apaga el cliente, cerrando los canales del socket y luego cerrando
 * al mismo socket.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_shutdown(client_t* self);


/** DESTRUCTOR
 * Libera los recursos utilizados.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int client_destroy(client_t* self);


// ----------------------------------------------------------------------------
#endif // __CLIENT_H__
