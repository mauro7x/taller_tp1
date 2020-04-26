// includes
#include "stdin_streamer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines
#define CALL_DELIMITER '\n'


// --------------------------------------------------------
// definiciones

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

*/


int stdin_streamer_create(stdin_streamer_t *self, callback_t callback) {
    self->callback = callback;
    return 0;
}

int stdin_streamer_run(stdin_streamer_t *self, void *context) {  
    char buffer[32];
    char* call = NULL;
    size_t total_call_len = 0;
    size_t partial_call_len = 0;
    size_t copy_from = 0;

    while(!feof(stdin)) {
        size_t read_bytes = fread(buffer, sizeof(char), sizeof(buffer), stdin);
        partial_call_len = 0;
        copy_from = 0;
        
        for (int i = 0; i < read_bytes; i++) {
            if (buffer[i] != CALL_DELIMITER) {
                partial_call_len++;
            } else {
                call = (char*) realloc(call, sizeof(char)*(total_call_len+partial_call_len));
                strncpy(call+total_call_len, buffer+copy_from, partial_call_len);
                total_call_len += partial_call_len;

                self->callback(context, call, total_call_len); // enviamos la call

                free(call);
                call = NULL;
                copy_from = i+1;
                total_call_len = 0;
                partial_call_len = 0;
            }
        }

        if (!feof(stdin)) {
            call = (char*) realloc(call, sizeof(char)*(total_call_len + partial_call_len));
            strncpy(call+total_call_len, buffer+copy_from, partial_call_len);
            total_call_len += partial_call_len;
        }
    }

    return 0;
}

int stdin_streamer_destroy(stdin_streamer_t *self) {
    return 0;
}

// --------------------------------------------------------
