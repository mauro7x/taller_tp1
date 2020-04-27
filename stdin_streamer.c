// includes
#include "stdin_streamer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defines
#define CALL_DELIMITER '\n'


// --------------------------------------------------------
// definiciones

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
        } else {
            if (call) {
                free(call);
            }
        }
    }

    return 0;
}

int stdin_streamer_destroy(stdin_streamer_t *self) {
    return 0;
}

// --------------------------------------------------------
