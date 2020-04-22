#ifndef __SOCKET_H__
#define __SOCKET_H__

// --------------------------------------------------------
// includes
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
// #include <unistd.h> que hace esta?

// defines
#define _POSIX_C_SOURCE 200112L

// --------------------------------------------------------
// structs

typedef struct socket {
	int state;
    bool connected;
    // flags de estado, etc
} socket_t;

// --------------------------------------------------------
// functions

/*  PRE: direccion de variable de tipo socket_t existe
    POST: 0 == el socket se inicializa correctamente; -1 == error
*/
int socket_create(socket_t *self);

/*  PRE: socket previamente inicializado, hostname al que se desea
    conectar, puerto o servicio que se desea utilizar.
    POST: 0 == se realiza la conexion, -1 == error
*/
int socket_connect(socket_t *self, const char* hostname, const char* port);

/*  PRE:
    POST:
*/
int socket_send(socket_t *self, char *buffer, size_t len);

/*  PRE:
    POST:
*/
int socket_recv(socket_t *self, char *buffer, size_t len);

/*  PRE: socket inicializado
    POST: socket destruido, recursos liberados
*/
int socket_destroy(socket_t *self);



// --------------------------------------------------------
#endif // __SOCKET_H__
