#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <memory.h>

#include <3ds.h>
//#include <libswscale/swscale.h>


#include "scaler.h"
#include "fb.h"
#include "color_convert.h"
#include "ppm.h"
#include "cam.h"
#include "capture_thread.h"
#include "camera_thread.h"

#define WAIT_TIMEOUT 1000000000ULL

#define KERNEL_SYNC_TIMEOUT 0x09401BFE

/*
#define WIDTH 640
#define HEIGHT 480
*/

#define IMAGE_SIZE (640 * 480 * 2)
#define RGB8_IMAGE_SIZE (640 * 480 * 3)
#define BUF_SIZE (IMAGE_SIZE * 2)

int main(int argc, char *argv[]) {
    uint8_t *buffer = NULL;
    uint8_t *next_buffer = NULL;
    
    volatile float *config_3d_slider = (volatile float *) 0x1FF81080;
    
    gfxInit(GSP_RGB565_OES, GSP_BGR8_OES, false);
    consoleInit(GFX_BOTTOM, NULL);
    
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_TOP, false);
    
    fsInit();

    
    camInit();
    uint16_t bufsize = cam_init_settings();
    
    buffer = malloc(BUF_SIZE);
    next_buffer = malloc(BUF_SIZE);
    
    if(buffer == NULL) {
        printf("Failed to allocate memory %d bytes for buffer\n", BUF_SIZE);
        while(1);
    }
    if(next_buffer == NULL) {
        printf("Failed to allocate memory %d bytes for next buffer\n", BUF_SIZE);
        while(1);
    }

    LightLock buffer_mutex;
    LightLock_Init(&buffer_mutex);
    
    LightEvent capture_thread_event;
    LightEvent_Init(&capture_thread_event, RESET_ONESHOT);
    
    init_scaler();
    init_sdmc();

    int32_t main_thread_prio = 0x30;
    svcGetThreadPriority(&main_thread_prio, CUR_THREAD_HANDLE);

    volatile struct capture_context_t *capture_thread_ctx = malloc(sizeof(struct capture_context_t));
    volatile struct camera_context_t *camera_thread_ctx = malloc(sizeof(struct camera_context_t));
    /*
    *capture_thread_ctx = (struct capture_context_t) { 
        &capture_thread_event,
        buffer + IMAGE_SIZE,
        buffer,
        0x0,
        0x0
    };
    */
    capture_thread_ctx->capture_thread_event = &capture_thread_event;
    capture_thread_ctx->left_image = malloc(RGB8_IMAGE_SIZE);
    capture_thread_ctx->right_image = malloc(RGB8_IMAGE_SIZE);
    capture_thread_ctx->busy = 0;
    capture_thread_ctx->exit = 0;
    
    camera_thread_ctx->buffer_mutex = &buffer_mutex;
    camera_thread_ctx->bufsize = bufsize;
    camera_thread_ctx->buffer = buffer;
    
    //printf("buf: %p\n", (void *) camera_thread_ctx->buffer);
    
    /*
    *camera_thread_ctx = (struct camera_context_t) {
        &buffer_mutex,
        bufsize,
        buffer
        //next_buffer
    };
    */
    printf("Activating cameras...\n");
    
    CAMU_Activate(SELECT_OUT1_OUT2);
    CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
    CAMU_StartCapture(PORT_BOTH);
    
    

    Thread capture_thread = threadCreate(capture_thread_function, (void *) capture_thread_ctx, 0x1000, main_thread_prio + 1, -2, false);
    Thread camera_thread = threadCreate(camera_thread_function, (void *) camera_thread_ctx, 0x1000, main_thread_prio + 1, -2, false);

    /*
    gfxFlushBuffers();
    gfxSwapBuffers();
    gspWaitForVBlank();
    */
    //memset(preview_buffer, 0, 400 * 240 * 2);
    memset(buffer, 0, BUF_SIZE);
    //bool camera_enabled = false;
    uint32_t keys_down;
    while(aptMainLoop()) {
        uint64_t start_time = osGetTime();
        
        hidScanInput();
        
        keys_down = hidKeysDown();
        
        
        if(keys_down & KEY_START) {
            camera_thread_ctx->exit = 1;
            capture_thread_ctx->exit = 1;
            __dmb();
            LightEvent_Signal(&capture_thread_event);
            break;
        }
        
        if((keys_down & KEY_L) || (keys_down & KEY_R)) {
            if(!capture_thread_ctx->busy) {
                //capture_context.busy = true;
                
                capture_thread_ctx->busy = 1;
                
                printf("Capture\n");
            
                LightLock_Lock(&buffer_mutex);
                rgb565_to_rgb888(camera_thread_ctx->buffer + IMAGE_SIZE, capture_thread_ctx->left_image, 640, 480);
                rgb565_to_rgb888(camera_thread_ctx->buffer, capture_thread_ctx->right_image, 640, 480);
                
                //memcpy(capture_thread_ctx->left_image, camera_thread_ctx->buffer, IMAGE_SIZE);
                //memcpy(capture_thread_ctx->right_image, camera_thread_ctx->buffer + IMAGE_SIZE, IMAGE_SIZE);
                LightLock_Unlock(&buffer_mutex);
            
                LightEvent_Signal(&capture_thread_event);
            } else {
                //printf("Busy   \n");
            }
        }

        
        // if((svcWaitSynchronization(cam1_receive_event, 0) != KERNEL_SYNC_TIMEOUT) && (svcWaitSynchronization(cam2_receive_event, 0) != KERNEL_SYNC_TIMEOUT)) { 

        if(*config_3d_slider > 0.0f) {
            /*
            gfxSet3D(true);
            uint64_t start_time = osGetTime();
            
            vga_to_3ds_scale(current_buffer, scale_buffer, 2);
            write_to_fb(scale_buffer, gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 2);

            vga_to_3ds_scale(current_buffer + IMAGE_SIZE, scale_buffer, 2);
            write_to_fb(scale_buffer, gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL), 2);
            
            uint64_t end_time = osGetTime();
            printf("Convert 3D (c: %d, r: %d)\r", (int) (end_time - start_time), (int) (receive_time_end - receive_time_start));
            */
        } else {
            gfxSet3D(false);
            uint64_t scale_time = osGetTime();
            
            //printf("lock()\n");
            LightLock_Lock(&buffer_mutex);
            
            vga_to_3ds_scale_fb(camera_thread_ctx->buffer, gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), 2);
            
            //printf("unlock()\n");
            LightLock_Unlock(&buffer_mutex);

            uint64_t end_time = osGetTime();
            printf("Convert (c: %d, s: %d, w: %d, %s)\r", (int) (end_time - start_time), (int) (scale_time - start_time), (int) (end_time - scale_time), (capture_thread_ctx->busy) ? "busy" : "ready");
            
        }
           
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        
        //svcSleepThread(100000000);
    }
    
    printf("Closing threads...\n");
    printf("Waiting for camera thread...\n");
    
    threadJoin(camera_thread, INT64_MAX);
    threadFree(camera_thread);
    
    printf("Waiting for capture thread...\n");
    
    threadJoin(capture_thread, INT64_MAX);
    threadFree(capture_thread);

    CAMU_StopCapture(PORT_BOTH);
    CAMU_Activate(SELECT_NONE);
    
    free(buffer);
    //free(next_buffer);
    
    return 0;
}

