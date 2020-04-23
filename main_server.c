// includes
#include <stdio.h>

#include "funciones.h"
#include "server.h"

// defines
#define USAGE_ERROR 1           // problemas con el uso de la aplicacion
#define OPEN_ERROR 2            // problemas con la abertura del servidor
#define ACCEPT_ERROR 3          // problemas aceptando cliente
#define TEST_ERROR 4            // problemas con las acciones en test

#define SHUTDOWN_ERROR 10       // problemas con el shutdown
#define DESTROY_ERROR 11        // problemas al destruir el server

// --------------------------------------------------------

int main(int argc, const char *argv[]) {
    /*
    PSEUDO CODIGO IDEA:

    1. Iniciar servidor, a la escucha de conexiones.
        1.1. Abrir servidor en el puerto especificado.
        1.2.

    2. Procesar cada call recibida.
        2.1. Procesamiento de call.
        ...
        2.n-1. Enviar OK al cliente en caso de ejecucion correcta.
        2.n. Imprimir por stdout la salida pedida.

    3. Cerrar conexion.

    4. Terminar ejecucion.
    
    */

    // Formato de entrada:
    // ./server <port>

    if (uso_incorrecto_server(argc)) {
        fprintf(stderr, "Error en el uso. Uso: ./server <port>\n");
        return USAGE_ERROR;
    }


    server_t server;

    server_create(&server, argv); 


    if (server_open(&server)) {
        return OPEN_ERROR;
    }

    if (server_accept(&server)) {
        return ACCEPT_ERROR;
    }

    
    // --------------------------------------------------------
    // para este punto, tenemos que estar conectados
    // hacemos algo y despues sigue el curso
    // vamos a recibir un mensaje

    if (server_testing_action(&server)) {
        return TEST_ERROR;
    }

    // --------------------------------------------------------


    if (server_shutdown(&server)) {
        return SHUTDOWN_ERROR;
    }


    if (server_destroy(&server)) {
        return DESTROY_ERROR;
    }

    return 0;

}

// --------------------------------------------------------
