#ifndef __STDIN_STREAMER_H__
#define __STDIN_STREAMER_H__

// --------------------------------------------------------
// includes
#include <stdio.h>


// --------------------------------------------------------
// structs

typedef int (*callback_t)(void* context, char* buffer, size_t len);

typedef struct stdin_streamer {
	callback_t callback;
} stdin_streamer_t;

// --------------------------------------------------------
// declarations

int stdin_streamer_create(stdin_streamer_t *self, callback_t callback);

int stdin_streamer_run(stdin_streamer_t *self, void *context);

int stdin_streamer_destroy(stdin_streamer_t *self);

// --------------------------------------------------------
#endif // __STDIN_STREAMER_H__
