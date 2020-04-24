// includes
#include "call.h"
#include "stdin_streamer.h"

#include <stdlib.h>
#include <string.h>


// --------------------------------------------------------
// definitions

int call_create(call_t* self) {
    self->already_filled = 0;

    stdin_streamer_t stdin_streamer;
    stdin_streamer_create(&stdin_streamer, &call_fill);
    
    if (stdin_streamer_run(&stdin_streamer, self)) {
        return -1; // eof
    }

    stdin_streamer_destroy(&stdin_streamer);
    return 0;
}


int call_fill(void* context, char* buffer, size_t len) {
    call_t* self = context;

    switch (self->already_filled) {
    case 0:
        self->dest = buffer;
        self->dest_len = len;
        break;

    case 1:
        self->path = buffer;
        self->path_len = len;
        break;

    case 2:
        self->interface = buffer;
        self->interface_len = len;
        break;

    case 3:
        self->method = buffer;
        self->method_len = len;
        break;

    case 4:
        self->params = buffer;
        self->params_len = len;
        break;
    
    default: 
        return -1;
        break;
    }

    self->already_filled++;
    return 0;
}


int call_destroy(call_t* self) {
    if (self->dest != NULL) {
        free(self->dest);
    }
    if (self->path != NULL) {
        free(self->path);
    }
    if (self->interface != NULL) {
        free(self->interface);
    }
    if (self->method != NULL) {
        free(self->method);
    }
    if (self->params != NULL) {
        free(self->params);
    }

    return 0;
}

// --------------------------------------------------------

