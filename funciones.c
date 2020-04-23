// includes
#include "funciones.h"
#include <stdio.h>

// --------------------------------------------------------
// se justifica la existencia de esto?
// --------------------------------------------------------
// definiciones

int uso_incorrecto_cliente(int argc){
    if ((argc < 3) || (argc > 4)) {
        return -1;
    }
    return 0;
}

int uso_incorrecto_server(int argc){
    if (argc != 2) {
        return -1;
    }
    return 0;
}

// --------------------------------------------------------
