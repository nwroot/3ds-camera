#pragma once

#include <3ds/synchronization.h>

struct capture_context_t {
    LightEvent *capture_thread_event;
    uint8_t *left_image;
    uint8_t *right_image;
    uint8_t busy;
    uint8_t exit;
};

void init_sdmc(void);

void capture_thread_function(void *capture_context_ptr);
