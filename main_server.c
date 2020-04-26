// includes
#include <stdio.h>
#include "server.h"

// defines
#define USAGE_ERROR 1           // problemas con el uso de la aplicacion
#define OPEN_ERROR 2            // problemas con la abertura del servidor
#define ACCEPT_ERROR 3          // problemas aceptando cliente
#define RECEIVE_ERROR 4         // problemas recibiendo las calls

#define SHUTDOWN_ERROR 10       // problemas con el shutdown
#define DESTROY_ERROR 11        // problemas al destruir el server

// --------------------------------------------------------

int main(int argc, const char *argv[]) {
    /*
    PSEUDO CODIGO IDEA:

    2. Procesar cada call recibida.
        2.1. Procesamiento de call.
        ...
        2.n-1. Enviar OK al cliente en caso de ejecucion correcta.
        2.n. Imprimir por stdout la salida pedida.

    */

    if (argc != 2) {
        fprintf(stderr, "Usage error. Usage: ./server <port>\n");
        return USAGE_ERROR;
    }

    server_t server;

    server_create(&server, argv); 

    if (server_open(&server)) {
        server_destroy(&server);
        return OPEN_ERROR;
    }

    if (server_accept(&server)) {
        server_destroy(&server);
        return ACCEPT_ERROR;
    }

    
    if (server_receive_calls(&server)) {
        server_destroy(&server);
        return RECEIVE_ERROR;
    }
    

    
    // --------------------------------------------------------
    // para este punto, tenemos que estar conectados
    // hacemos algo y despues sigue el curso
    // vamos a recibir un mensaje

    /*
    if (server_testing_action(&server)) {
        server_destroy(&server);
        return TEST_ERROR;
    }
    */

    // --------------------------------------------------------

    if (server_shutdown(&server)) {
        server_destroy(&server);
        return SHUTDOWN_ERROR;
    }

    if (server_destroy(&server)) {
        return DESTROY_ERROR;
    }


    printf("\n\nCHAMPIONS\n\n");
    return 0;
}

// --------------------------------------------------------
