#pragma once

#include <3ds/synchronization.h>

struct camera_context_t {
    LightLock *buffer_mutex;
    uint16_t bufsize;
    uint8_t *buffer;
    uint8_t exit;
};

void camera_thread_function(void *camera_context_ptr);
