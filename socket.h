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
 * @desc:   inicializa los atributos correspondientes.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_create(socket_t* self);


/**
 * @desc:   obtiene la lista de direcciones a las que se intentará conectar.
 * @param:  hostname y puerto del que se quieren las direcciones, booleano que
 *          indica si se trata de un socket pasivo (server) o no.
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_get_addresses(socket_t* self, const char* hostname,
                         const char* port, bool passive);


/** Método SERVER-SIDE
 * @desc:   crea el socket con el file descriptor obtenido por getaddrinfo,
 *          arregla el problema de TIMEWAIT de existir, y bindea el socket al
 *          puerto que corresponda.
 * @param:  puerto al que se quiere bindear.
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_bind(socket_t* self, const char* port);


/** Método SERVER-SIDE
 * @desc:   establece el servidor a la escucha de clientes.
 * @param:  número maximo de clientes encolados esperando ser aceptados.
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_listen(socket_t* self, const int max_clients_in_queue);


/** Método SERVER-SIDE
 * @desc:   acepta una conexión entrante.
 * @param:  puntero al socket aceptado.
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_accept(socket_t* self, socket_t* accepted_socket);


/** Método CLIENT-SIDE
 * @desc:   intenta conectarse a las direcciones obtenidas en la funcion
 *          socket_get_addresses.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_connect(socket_t* self);


/**
 * @desc:   envia 'len' bytes del buffer a través del socket.
 * @param:  buffer del cual enviar, longitud de bytes a enviar.
 * @return: n>0 indicando cuantos bytes se enviaron, 0 si se cerró el socket,
 *          -1 en caso de error inesperado.
*/
int socket_send(socket_t *self, char *buffer, size_t len);


/**
 * @desc:   recibe 'len' bytes del buffer a través del socket.
 * @param:  buffer del cual recibir, longitud de bytes a recibir.
 * @return: n>0 indicando cuantos bytes se recibieron, 0 si se cerró el socket,
 *          -1 en caso de error inesperado.
*/
int socket_recv(socket_t *self, char *buffer, size_t len);


/**
 * @desc:   apaga el canal solicitado del socket.
 * @param:  int representando el canal a apagar de acuerdo a:
 *              0 == READ CHANNEL, 1 == WRITE CHANNEL, 2 BOTH
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_shutdown(socket_t *self, int channel);


/** DESTRUCTOR
 * @desc:   libera los recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int socket_destroy(socket_t *self);


// ----------------------------------------------------------------------------
#endif // __SOCKET_H__
