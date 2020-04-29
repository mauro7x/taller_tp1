/** TP1 - Taller de Programación - SERVIDOR
 * 
 * Alumno: Parafati, Mauro.
 * Padrón: 102749.
 * Año y cuatrimestre: 1C 2020.
 * Repositorio: https://github.com/mauro7x/taller_tp1
*/

// ----------------------------------------------------------------------------
#include "server.h"
#include <stdio.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Constantes para el manejo de errores

#define USAGE_ERROR 1
#define CREATE_ERROR 2
#define OPEN_ERROR 3
#define ACCEPT_ERROR 4
#define RECEIVE_ERROR 5
#define SHUTDOWN_ERROR 6
#define DESTROY_ERROR 7
// ----------------------------------------------------------------------------

/**
 * Entrada al flujo principal del SERVIDOR.
*/
int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage error. Usage: ./server <port>\n");
        return USAGE_ERROR;
    }

    server_t server;

    if (server_create(&server, argv[1])) {
        return CREATE_ERROR;
    }

    if (server_open(&server)) {
        server_destroy(&server);
        return OPEN_ERROR;
    }

    if (server_accept(&server)) {
        server_destroy(&server);
        return ACCEPT_ERROR;
    }

    if (server_receive_calls(&server)) {
        server_shutdown(&server);
        server_destroy(&server);
        return RECEIVE_ERROR;
    }
    
    if (server_shutdown(&server)) {
        server_destroy(&server);
        return SHUTDOWN_ERROR;
    }

    if (server_destroy(&server)) {
        return DESTROY_ERROR;
    }

    return 0;
}

// ----------------------------------------------------------------------------
