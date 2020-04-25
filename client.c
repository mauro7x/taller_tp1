// includes
#include "client.h"
#include "call.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// defines
#define EOF_ERROR 1


// --------------------------------------------------------
// definiciones

static int client_set_stdin(int argc, const char* argv[]) {
    if (argc==4) {
        FILE *fp;
        if ((fp = fopen(argv[3], "r")) == NULL) {
            return -1;
        }
        stdin = fp;
    }

    return 0;
}


int client_create(client_t* self, int argc, const char* argv[]) {
    self->hostname = argv[1];
    self->port = argv[2];

    socket_t socket;
    socket_create(&socket);
    self->socket = socket;

    if (client_set_stdin(argc, argv)) {
        fprintf(stderr, "Error in function: client_set_stdin. Error: no se puede abrir el archivo.");
        return -1;
    }

    return 0;
}


int client_connect(client_t* self) {

    if (socket_get_addresses(&(self->socket), self->hostname, self->port, false)) {
        return -1;
    }

    if (socket_connect(&(self->socket), self->hostname, self->port)) {
        return -1;
    }   

    return 0;
}



// --------------------------------------------------------

static int client_send_call(client_t* self, uint32_t id) {
    call_t call;
    
    int s = call_create(&call, id);
    if (s == -1) {
        fprintf(stderr, "Error in function: call_create.\n");
        return -1;
    } else if (s == EOF_ERROR) {
        return EOF_ERROR;
    }

    /* PARA PRINTEAR LA CALL BYTE POR BYTE

    for (int i = 0; i < call.total_len; i++) {
        printf("msg[%i]: %d\n", i, call.msg[i]);
    }

    */

    // enviar
    

    call_destroy(&call);
    return 0;
}


int client_send_calls(client_t* self) {
    uint32_t next_id = 1;

    while(!feof(stdin)) {
        client_send_call(self, next_id++);
    }
    
    return 0;
}


// --------------------------------------------------------

int server_testing_action(client_t* self) {

    int sent;

    char longitud[2];


    char* msg = "Mensaje random, aguante bokita";
    unsigned short len = strlen(msg) + 1;


    sprintf(longitud, "%d", len);

    // primero mandamos dos digitos que representan la longitud del proximo mensaje
    sent = socket_send(&(self->socket), longitud, 2);
    if (sent == -1) {
        fprintf(stderr, "Error en el envio del mensaje.");
        return -1;
    } else {
        fprintf(stdout, "Se enviaron %d bytes.\n", sent);
    }

    // ahora mandamos el mensaje
    sent = socket_send(&(self->socket), msg, (size_t) len);
    if (sent == -1) {
        fprintf(stderr, "Error en el envio del mensaje.");
        return -1;
    } else {
        fprintf(stdout, "Se enviaron %d bytes.\n", sent);
    }

    return 0;
}

// --------------------------------------------------------


int client_shutdown(client_t* self) {
    if (socket_shutdown(&(self->socket))) {
        fprintf(stderr, "Error apagando el socket.");
        return -1;
    }
    return 0;
}


int client_destroy(client_t* self) {
    socket_destroy(&self->socket);
    fclose(stdin);
    return 0;
}

// --------------------------------------------------------

