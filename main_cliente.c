// includes
#include <stdio.h>
// #include <string.h>

#include "funciones.h"
#include "client.h"
#include "socket.h"

// defines
#define USAGE_ERROR 1           // problemas con el uso de la aplicacion
#define CREATE_ERROR 2          // problemas con la apertura del archivo
#define CONNECT_ERROR 3         // problemas con la conexion del cliente


// --------------------------------------------------------

int main(int argc, const char *argv[]) {
    /*
    PSEUDO CODIGO IDEA:

    0. Chequear que la entrada ingresada sea correcta.
    
    1. Establecer conexion con el server.
        1.1. Parsear <hostname> y <port>.
        1.2.

    2. Parsear entrada, generar calls de a una.
        2.1. 
        
    3. Enviar call generada al server.

    4. Imprimir respuesta server, si hay mas calls volver a 2.

    5. Cerrar conexion con el server.

    6. Terminar ejecucion.
    */


    // Formato de entrada:
    // ./client <host> <port> [<input_file>]

    if (uso_incorrecto_cliente(argc, argv)) {
        fprintf(stderr, "Error en el uso. Uso: ./client <host> <port> [<input_file>]\n");
        return USAGE_ERROR;
    }

    client_t client;

    if (client_create(&client, argc, argv)) {
        return CREATE_ERROR;
    }    

    if (client_connect(&client)) {
        return CONNECT_ERROR;
    }

 
    



    client_destroy(&client);
    
    

    


    


/*
    const char* hostname = argv[1];
    const char* port = argv[2];


    socket_t socket;

    if (socket_create(&socket)) {
        fprintf(stderr, "Error en socket_create: referencia invalida.\n");
        return ERROR_SOCKET_CREATE;
    }

    socket_get_addresses(&socket, hostname, port);
    socket_connect(&socket, hostname, port);

    // intentamos hacer un saludin
    int sent;
    char* buff = "Estoy enviando un msj random.\n";

    sent = socket_send(&socket, buff, strlen(buff));
    printf("Se enviaron %d bytes.\n", sent);

    socket_shutdown(&socket);

    if (socket_destroy(&socket)) {
        fprintf(stderr, "Error en socket_destroy.\n");
        return ERROR_SOCKET_DESTROY;
    }

*/
    return 0;
}
