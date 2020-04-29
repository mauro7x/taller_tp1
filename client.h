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
 * @desc:   se encarga de inicializar el cliente llenando sus atributos.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int client_create(client_t* self, const char* hostname, const char* port);


/**
 * @desc:   se encarga de abrir el archivo recibido y de setearlo como stdin
 *          para el resto de la ejecución.
 * @param:  ruta al archivo.
 * @return: 0 si no hay errores, -1 CC.
*/
int client_set_input_file(const char* path_to_file);


/**
 * @desc:   intenta conectarse al servidor, primero obteniendo las direcciones
 *          posibles y luego probando con cada una de ellas.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int client_connect(client_t* self);


/**
 * @desc:   se encarga de coordinar el procesamiento de las calls recibidas
 *          por la entrada, para luego enviarlas al servidor y recibir la
 *          respuesta del mismo por cada call enviada.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int client_send_calls(client_t* self);


/** CALLBACK del STDIN_STREAMER
 * @desc:   se encarga del envio de una call en particular.
 * @param:  linea de texto plano con la call a procesar y enviar, longitud
 *          de la misma
 * @return: 0 si no hay errores, -1 CC.
*/
int client_send_call(void* context, char* buffer, size_t len);


/**
 * @desc:   apaga el cliente, cerrando los canales del socket.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int client_shutdown(client_t* self);


/** DESTRUCTOR
 * @desc:   libera los recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int client_destroy(client_t* self);


// ----------------------------------------------------------------------------
#endif // __CLIENT_H__
