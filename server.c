// ----------------------------------------------------------------------------
#include "server.h"
#include "socket.h"
#include "dbus_server.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_CLIENTS_IN_QUEUE 1 // solo se conectara un cliente para este TP
#define HOSTNAME 0
#define SOCKET_HAS_BEEN_CLOSED 1
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// "Métodos" privados
// ----------------------------------------------------------------------------

/**
 * Recibe y procesa una call. Se encarga de instanciar al parser dbus_server,
 * y de coordinar el procesamiento de la misma para posteriormente enviar
 * respuesta al cliente.
*/
static int _receive_and_process_call(server_t* self) {
    // Verificamos si viene otra call, leyendo el primer byte
    int s;
    char r;
    s = socket_recv(&(self->peer), &r, sizeof(r));

    if (s == 0) {
        return SOCKET_HAS_BEEN_CLOSED;
    } else if (s == -1) {
        return -1;
    }

    // Viene otra call, delegamos su procesamiento a dbus_server
    dbus_server_t dbus_server;
    dbus_server_create(&dbus_server, &(self->peer));

    if (dbus_server_recv_call(&dbus_server)) {
        return -1;
    }

    // Imprimimos y mandamos confirmacion al cliente
    dbus_server_print_received_call(&dbus_server);

    if (dbus_server_send_confirmation(&dbus_server)) {
        return -1;
    }

    dbus_server_destroy(&dbus_server);
    return 0;
}


// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int server_create(server_t* self, const char* port) {
    self->port = port;
    self->hostname = HOSTNAME;

    // socket aceptador, y socket peer
    socket_t socket, peer_socket;
    if (socket_create(&socket)) {
        fprintf(stderr, "Error in function: socket_create.\n");
        return -1;
    }
    if (socket_create(&peer_socket)) {
        fprintf(stderr, "Error in function: socket_create.\n");
        return -1;
    }
    self->acceptor = socket; 
    self->peer = peer_socket;

    return 0;
}


int server_open(server_t* self) {
    if (socket_get_addresses(&(self->acceptor), self->hostname, self->port,
                             true)) {
        return -1;
    }

    if (socket_bind(&(self->acceptor), self->port)) {
        return -1;
    }

    if (socket_listen(&(self->acceptor), MAX_CLIENTS_IN_QUEUE)) {
        return -1;
    }

    return 0;
}


int server_accept(server_t* self) {
    if (socket_accept(&(self->acceptor), &(self->peer))) {
        fprintf(stderr, "Error acceptando conexion entrante.");
        return -1;
    }

    return 0;
}


int server_receive_calls(server_t* self) {
    int s;

    do {
        s = _receive_and_process_call(self);
    } while (s == 0);

    if (s != SOCKET_HAS_BEEN_CLOSED) {
        fprintf(stderr, "Error in function: server_receive_call.\n");
        return -1;
    }

    return 0;
}


int server_shutdown(server_t* self) {
    if (socket_shutdown(&(self->acceptor))) {
        fprintf(stderr, "Error in function: socket_shutdown (S)\n");
        return -1;
    }

    if (socket_shutdown(&(self->peer))) {
        fprintf(stderr, "Error in function: socket_shutdown (P)\n");
        return -1;
    }

    return 0;
}


int server_destroy(server_t* self) {
    socket_destroy(&(self->acceptor));
    socket_destroy(&(self->peer));
    
    return 0;
}


// ----------------------------------------------------------------------------
