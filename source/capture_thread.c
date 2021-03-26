#include "capture_thread.h"

#define IMAGE_SIZE (640 * 480 * 2)
#define RGB8_IMAGE_SIZE (640 * 480 * 3)
#define BUF_SIZE (IMAGE_SIZE * 2)

#include <3ds/services/fs.h>
#include <3ds/synchronization.h>
#include <stdio.h>
#include <time.h>


static const uint8_t header[] = {
    'P', '6', ' ', '6', '4', '0', ' ', '4', '8', '0', ' ', '2', '5', '5', ' '
};


static FS_Archive sdmc_archive;

void init_sdmc(void) {
    FSUSER_OpenArchive(&sdmc_archive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, NULL));
}

static void write_full_buffer(Handle file, uint32_t *bytes_written, uint64_t offset, void *buffer, uint32_t size, uint32_t flags) {
    uint32_t total_written = 0;
    while(1) {
        uint32_t written = 0;
        FSFILE_Write(file, &written, offset + total_written, (char *) buffer + total_written, ((size - total_written) > (512 * 1024)) ? (512 * 1024) : (size - total_written), flags);
        total_written += written;
        if(written >= size || written == 0) {
            break;
        }
    }
    *bytes_written = total_written;
    return;
}

void capture_thread_function(void *capture_context_ptr) {
    // volatile just in case
    volatile struct capture_context_t *capture_context = capture_context_ptr;
    
    while(1) {
        LightEvent_Wait(capture_context->capture_thread_event);
        
        if(capture_context->exit) {
            break;
        }
        
        Result ret;
        
        time_t curr_time = 0;
        time(&curr_time);
        char name[128] = { 0 };
        
        snprintf(name, 128, "/camera/%lld-l.ppm", (long long int) curr_time);
        
        Handle file_l;
        Handle file_r;
        
        ret = FSUSER_OpenFile(&file_l, sdmc_archive, fsMakePath(PATH_ASCII, name), FS_OPEN_CREATE | FS_OPEN_WRITE, 0x0);
        if(ret != 0) {
            printf("Failed to open file: %s\n", name);
        } else {
            uint32_t bytes_written = 0;
            FSFILE_Write(file_l, &bytes_written, 0x0, header, sizeof(header), 0x0);
            if(bytes_written != sizeof(header)) {
                printf("Failed to write header for file: %s\n", name);
            }
            write_full_buffer(file_l, &bytes_written, sizeof(header), capture_context->left_image, RGB8_IMAGE_SIZE, 0x0);
            if(bytes_written != RGB8_IMAGE_SIZE) {
                printf("Failed to write image data for file: %s\n", name);
            }
            FSFILE_Close(file_l);
        }
        
        snprintf(name, 128, "/camera/%lld-r.ppm", (long long int) curr_time);
        ret = FSUSER_OpenFile(&file_r, sdmc_archive, fsMakePath(PATH_ASCII, name), FS_OPEN_CREATE | FS_OPEN_WRITE, 0x0);
        if(ret != 0) {
            printf("Failed to open file: %s\n", name);
        } else {
            uint32_t bytes_written = 0;
            FSFILE_Write(file_r, &bytes_written, 0x0, header, sizeof(header), 0x0);
            if(bytes_written != sizeof(header)) {
                printf("Failed to write header for file: %s\n", name);
            }
            write_full_buffer(file_r, &bytes_written, sizeof(header), capture_context->right_image, RGB8_IMAGE_SIZE, 0x0);
            if(bytes_written != RGB8_IMAGE_SIZE) {
                printf("Failed to write image data for file: %s\n", name);
            }
            FSFILE_Close(file_r);
        }
        
        capture_context->busy = 0;
        __dmb();
    }
}
