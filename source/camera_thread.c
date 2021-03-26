#include "camera_thread.h"

#include <3ds/synchronization.h>
#include <3ds/svc.h>
#include <3ds/services/cam.h>

#include <stdlib.h>
#include <memory.h>

#define IMAGE_SIZE (640 * 480 * 2)
#define BUF_SIZE (IMAGE_SIZE * 2)

void camera_thread_function(void *camera_context_ptr) {
    volatile struct camera_context_t *camera_context = camera_context_ptr;
    uint16_t bufsize = camera_context->bufsize;
    
    uint8_t *next_buffer = malloc(BUF_SIZE);
    
    Handle receive_events[2];
    
    int32_t sync_out = 0;
    
    while(1) {
        if(camera_context->exit) {
            break;
        }
    
        CAMU_SetReceiving(&receive_events[0], next_buffer, PORT_CAM1, IMAGE_SIZE, (int16_t) bufsize);
        CAMU_SetReceiving(&receive_events[1], next_buffer + IMAGE_SIZE, PORT_CAM2, IMAGE_SIZE, (int16_t) bufsize);

        svcWaitSynchronizationN(&sync_out, receive_events, 2, true, 1000000000ULL);

        LightLock_Lock(camera_context->buffer_mutex);
        memcpy(camera_context->buffer, next_buffer, BUF_SIZE);
        LightLock_Unlock(camera_context->buffer_mutex);
        
        svcCloseHandle(receive_events[0]);
        svcCloseHandle(receive_events[1]);
    }
}
