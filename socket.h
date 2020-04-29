#ifndef __SOCKET_H__
#define __SOCKET_H__

// ----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct addrinfo addrinfo_t;

typedef struct socket {
	int fd; // file descriptor del socket.
    struct addrinfo* addresses_to_try;
} socket_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * Inicializa los atributos correspondientes.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_create(socket_t* self);


/**
 * Obtiene la lista de direcciones a las que se intentará conectar el socket.
 * Con el argumento passive se indica si se trata de un servidor o cliente.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_get_addresses(socket_t* self, const char* hostname,
                         const char* port, bool passive);


/** Método SERVER-SIDE.
 * Crea el socket con el file descriptor obtenido por getaddrinfo, arregla
 * el problema de TIMEWAIT de existir, y bindea el socket al puerto
 * que corresponda.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_bind(socket_t* self, const char* port);


/** Método SERVER-SIDE.
 * Establece el servidor a la escucha de N clientes (encolados).
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_listen(socket_t* self, const int max_clients_in_queue);


/** Método SERVER-SIDE.
 * Acepta un cliente.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_accept(socket_t* self, socket_t* accepted_socket);


/** Método CLIENT-SIDE.
 * Intenta conectarse a través de las direcciones obtenidas.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_connect(socket_t* self, const char* hostname, const char* port);


/**
 * Envía 'len' bytes desde el buffer recibido a través del socket.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_send(socket_t *self, char *buffer, size_t len);


/**
 * Recibe 'len' bytes desde el buffer recibido a través del socket.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_recv(socket_t *self, char *buffer, size_t len);


/**
 * Apaga el socket, cerrando ambos de sus canales.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_shutdown(socket_t *self, int channel);


/** DESTRUCTOR
 * Libera los recursos utilizados.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int socket_destroy(socket_t *self);


// ----------------------------------------------------------------------------
#endif // __SOCKET_H__
