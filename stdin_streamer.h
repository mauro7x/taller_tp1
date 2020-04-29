#ifndef __STDIN_STREAMER_H__
#define __STDIN_STREAMER_H__

// ----------------------------------------------------------------------------
#include <stdio.h>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Definición de estructuras
// ----------------------------------------------------------------------------

typedef int (*callback_t)(void* context, char* buffer, size_t len);

typedef struct stdin_streamer {
	callback_t callback;
} stdin_streamer_t;

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

/** CONSTRUCTOR
 * @desc:   inicializa el streamer, estableciendo la función de callback.
 * @param:  función de callback.
 * @return: 0 si no hay errores, -1 CC.
*/
int stdin_streamer_create(stdin_streamer_t *self, callback_t callback);


/**
 * @desc:   coordina la iteración sobre la entrada, obteniendo mediante un
 * 			buffer estático, las lineas de texto plano que definen las calls,
 * 			separadas mediante el caracter '\n', para posteriormente invocar
 * 			la función callback con esta línea, disparando su creación y
 * 			procesamiento.
 * @param:  puntero a contexto.
 * @return: 0 si no hay errores, -1 CC.
*/
int stdin_streamer_run(stdin_streamer_t *self, void *context);


/** DESTRUCTOR
 * @desc:   libera recursos utilizados.
 * @param:  -
 * @return: 0 si no hay errores, -1 CC.
*/
int stdin_streamer_destroy(stdin_streamer_t *self);


// ----------------------------------------------------------------------------
#endif // __STDIN_STREAMER_H__
