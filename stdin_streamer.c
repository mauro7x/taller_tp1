// includes
#include "stdin_streamer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines


// --------------------------------------------------------
// definiciones

int stdin_streamer_create(stdin_streamer_t *self, callback_t callback) {
    self->callback = callback;
    return 0;
}

/*

static int stdin_streamer_by_word(stdin_streamer_t* self, void* context, char delimiter) {
    char* word;
    size_t total_length = 0;
    int found_delimiter = 0;
    char c = fgetc(stdin);

    if (feof(stdin)) {
        return -1;
    }

    while(!found_delimiter) {

        char static_buffer[32] = "";
        size_t len = 0;

        for (int i = 0; ((i<32) && (!found_delimiter)); i++) {
            if (c != delimiter) {
                static_buffer[i] = c;
                len++;
                c = fgetc(stdin);
            } else {
                found_delimiter = 1;
            }
        }

        if (total_length == 0) {
            word = (char*) malloc(sizeof(char)*(len));
            strncpy(word, static_buffer, len);
            total_length+= len;
        } else {
            char* temp = (char*) malloc(sizeof(char) * total_length);
            strncpy(temp, word, total_length);
            word = realloc(word, sizeof(char)*total_length + sizeof(char)*(len));
            strncpy(word, temp, total_length);
            strncpy(word+total_length, static_buffer, len);
            free(temp);
            total_length+= len;
        }
    }
    
    self->callback(context, word, total_length);
    return 0;
}

int stdin_streamer_run(stdin_streamer_t *self, void *context) {
    // formato de entrada: <dest> <path> <interface> <method>([<arg1, arg2, ... argN>])\n
    // por enunciado SE PUEDE ASUMIR que la linea esta formada correctamente

    if (stdin_streamer_by_word(self, context, ' ')) {
        return -1; //end of file
    }
    
    stdin_streamer_by_word(self, context, ' ');
    stdin_streamer_by_word(self, context, ' ');
    stdin_streamer_by_word(self, context, '(');
    stdin_streamer_by_word(self, context, ')');
    fgetc(stdin); // eliminar \n final
    return 0;
}

*/


int stdin_streamer_run(stdin_streamer_t *self, void *context) {

    /**
     * Tenemos que leer lineas separadas por \n hasta el eof,
     * y mandarlas a callback.
    */





    return 0;
}



int stdin_streamer_destroy(stdin_streamer_t *self) {
    return 0;
}

// --------------------------------------------------------
