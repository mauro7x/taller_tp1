#ifndef __CALL_H__
#define __CALL_H__

// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef struct param {
    char* data;
    uint32_t len;
    char id;
    char data_type;
} param_t;

typedef struct call {
    char endianness;
    uint32_t id;
    param_t dest;
    param_t path;
    param_t interface;
    param_t method;
    param_t* params;
    size_t n_params; 
} call_t;


// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * Inicializa los parámetros del TDA call en 0/NULL, según corresponda.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int call_create(call_t* self);


/** MÉTODO CLIENT-SIDE
 * Recibe un buffer que contiene una call en texto plano, y se encarga
 * de parsearla para llenar la estructura de los datos.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int call_fill(call_t* self, char* line, size_t len, uint32_t id);


/** DESTRUCTOR
 * Libera la memoria dinámica reservada para la estructura de datos.
 * Devuelve 0 si no hay errores, -1 CC.
*/
int call_destroy(call_t* self);


// ----------------------------------------------------------------------------
#endif // __CALL_H__
