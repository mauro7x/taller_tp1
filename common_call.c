// ----------------------------------------------------------------------------
#include "call.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define PARAMS_DELIMITER ','
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// "Métodos" privados
// ----------------------------------------------------------------------------

// De inicialización:

/**
 * @desc:   llena los atributos de la estructura param con nulos/0, según
 *          corresponda.
 * @param:  puntero a param_t a inicializar.
 * @return: -
*/
static void _initialize_param(call_t* self, param_t* param) {
    param->data_type = 0;
    param->id = 0;
    param->len = 0;
    param->data = NULL;
}

// De parseo:

/**
 * @desc:   parsea la línea recibida que contiene el siguiente formato por
 *          enunciado: <dest> <path> <interface> <method>([<args>]).
 * @param:  el string que contiene la linea a parsear, un offset que indica
 *          desde donde hay que parsearla, un char que será el que corte
 *          nuestro split, y un size_t que indica hasta cuanto leer.
 * @return: -
*/
static void _fill_param_from_line(param_t* param, char* line, int* offset,
                                  char delimiter, size_t max_len) {
   uint32_t param_len = 0;
   int delimiter_found = 0;

   for (int i = (*offset); (i < max_len) && (!delimiter_found); i++) {
       if (line[i] != delimiter) {
           param_len++;
       } else {
           param->len = param_len;
           param->data = (char*) malloc(sizeof(char)*param_len);
           strncpy(param->data, line+(*offset), param_len);
           (*offset)+= param_len+1;
           delimiter_found = 1;
       }
   }
}


// De parseo de los argumentos de la firma (de existir):

/**
 * @desc:   cuenta la cantidad de parámetros que se van a separar en la
 *          línea recibida.
 * @param:  linea de parametros, size_t que indica la longitud de la misma,
 *          char que delimita nuestro split.
 * @return: size_t indicando la cantidad de parámetros en nuestra linea.
*/
static size_t _count_params(char* buffer, size_t len, char delimiter) {
    size_t n_params = 1;

    for (int i = 0; i < len; i++) {
        if (buffer[i] == delimiter) {
            n_params++;
        }
    }

    return n_params;
}


/**
 * @desc:   recibe una línea de parámetros separados por un delimiter, y los
 *          separa llenando la estructura de datos correspondiente.
 * @param:  puntero a la lista de parametros a llenar, linea de parametros,
 *          un size_t que indica la longitud de la linea, y un char que
 *          delimita nuestro split.
 * @return: -
*/
static void _fill_params(param_t* params, char* buffer, size_t len,
                         char delimiter) {
    int last_delimiter = 0;
    uint32_t current_param_len = 0;
    size_t params_added = 0;

    for (int i = 0; i < len; i++) {
        if ((buffer[i] == delimiter) || (i == len-1)) {
            if (i == len-1) { // creamos el ultimo parametro
                current_param_len++;
            }
            char* current_param = (char*) malloc(sizeof(char)*
                                  (current_param_len));
            strncpy(current_param, buffer + last_delimiter, current_param_len);
            params[params_added].data = current_param;
            params[params_added].len = current_param_len;
            current_param_len = 0;
            params_added++;
            last_delimiter = i+1;

        } else {
            current_param_len++;
        }
    }
}


/**
 * @desc:   se encarga de llamar a las funciones necesarias para parsear
 *          los argumentos de la firma (si existiese)
 * @param:  linea de la firma a parsear, longitud, char delimitador de split.
 * @return: -
*/
static void _parameters_parser(call_t* self, char* buffer, size_t len,
                               char delimiter) {
    size_t n_params = _count_params(buffer, len, delimiter);
    param_t* params = (param_t*) malloc(sizeof(param_t)*n_params);
    _fill_params(params, buffer, len, delimiter);

    self->n_params = n_params;
    self->params = params;
    free(buffer);
}

// ----------------------------------------------------------------------------
// "Métodos" públicos
// ----------------------------------------------------------------------------

int call_create(call_t* self) {
    self->id = 0;
    self->endianness = 0;
    self->dest.data = NULL;
    _initialize_param(self, &(self->dest));
    _initialize_param(self, &(self->path));
    _initialize_param(self, &(self->interface));
    _initialize_param(self, &(self->method));
    self->params = NULL;
    
    return 0;
}


int call_fill(call_t* self, char* line, size_t len, uint32_t id) {
    self->id = id;

    // Por enunciado, las líneas vienen bien formadas.
    int offset = 0;
    param_t args;  
    _fill_param_from_line(&(self->dest), line, &offset, ' ', len);
    _fill_param_from_line(&(self->path), line, &offset, ' ', len);
    _fill_param_from_line(&(self->interface), line, &offset, ' ', len);
    _fill_param_from_line(&(self->method), line, &offset, '(', len);
    _fill_param_from_line(&(args), line, &offset, ')', len);

    if (args.len) {
        _parameters_parser(self, args.data, args.len, ',');
    } else {
        self->n_params = 0;
        self->params = NULL;
        free(args.data);
    }

    return 0;
}


int call_destroy(call_t* self) {
    if (self->dest.data) {
        free(self->dest.data);
    }

    if (self->path.data) {
        free(self->path.data);
    }
    
    if (self->interface.data) {
        free(self->interface.data);
    }
    if (self->method.data) {
        free(self->method.data);
    }
    
    if (self->params) {
        for (int i = 0; i < (self->n_params); i++) {
            free(self->params[i].data);
        }
        free(self->params);
    }
    
    return 0;
}

// ----------------------------------------------------------------------------
