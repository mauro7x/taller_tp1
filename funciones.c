// includes
#include "funciones.h"
#include <stdio.h>

// --------------------------------------------------------
// definiciones

int uso_incorrecto_cliente(int argc, const char *argv[]){
    if ((argc < 3) || (argc > 4)) {
        return -1;
    }
    return 0;
}

// --------------------------------------------------------
