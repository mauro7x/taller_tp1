#ifndef __FUNCIONES_H__
#define __FUNCIONES_H__

// --------------------------------------------------------
// includes

// --------------------------------------------------------
// estructuras

// --------------------------------------------------------
// declaraciones

/**
 * Chequea argumentos pasados para ver si cumplen
 * con el formato esperado por cliente.
 * 0 == ok; -1 == error.
 */
int uso_incorrecto_cliente(int argc);

/**
 * Chequea argumentos pasados para ver si cumplen
 * con el formato esperado por server.
 * 0 == ok; -1 == error.
 */
int uso_incorrecto_server(int argc);


// --------------------------------------------------------
#endif // __FUNCIONES_H__