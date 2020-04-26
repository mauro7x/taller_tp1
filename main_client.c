// includes
#include <stdio.h>
#include "client.h"

// defines
#define USAGE_ERROR 1           // problemas con el uso de la aplicacion
#define CREATE_ERROR 2          // problemas con la apertura del archivo
#define CONNECT_ERROR 3         // problemas con la conexion del cliente
#define SEND_ERROR 4            // problemas con el envio de los calls

#define SHUTDOWN_ERROR 10       // problemas con el shutdown
#define DESTROY_ERROR 11        // problemas al destruir el cliente


// --------------------------------------------------------

int main(int argc, const char *argv[]) {
    /*
    PSEUDO CODIGO IDEA:

    2. Parsear entrada, generar calls de a una.
        2.1. 
        
    3. Enviar call generada al server.

    4. Imprimir respuesta server, si hay mas calls volver a 2.

    */


    if ((argc < 3) || (argc > 4)) {
        fprintf(stderr, "Usage error. Usage: ./client <host> <port> [<input_file>]\n");
        return USAGE_ERROR;
    }

    client_t client;

    if (client_create(&client, argc, argv)) {
        return CREATE_ERROR;
    }    

    if (client_connect(&client)) {
        client_destroy(&client);
        return CONNECT_ERROR;
    }

    
    if (client_send_calls(&client)) {
        client_destroy(&client);
        return SEND_ERROR;
    }
    

    // --------------------------------------------------------
    // para este punto, tenemos que estar conectados
    // hacemos algo y despues sigue el curso
    // vamos a enviar un mensaje

    
    /*
    if (server_testing_action(&client)) {
        client_destroy(&client);
        return SEND_ERROR;
    }
    */
    
    

    // --------------------------------------------------------
    
    if (client_shutdown(&client)) {
        client_destroy(&client);
        return SHUTDOWN_ERROR;
    }

    if (client_destroy(&client)) {
        return DESTROY_ERROR;
    }
    
    return 0;
}

// --------------------------------------------------------

