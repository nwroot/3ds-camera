#include "cam.h"

#include <3ds.h>

#define VGA_WIDTH 640
#define VGA_HEIGHT 480

uint16_t cam_init_settings() {
    CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_VGA, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A);
    
    CAMU_SetFrameRate(SELECT_OUT1_OUT2, FRAME_RATE_10);
    
    CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true);
    CAMU_SetAutoExposure(SELECT_OUT1_OUT2, true);
    CAMU_SetAutoWhiteBalance(SELECT_OUT1_OUT2, true);
    
    CAMU_SetTrimming(PORT_CAM1, false);
    CAMU_SetTrimming(PORT_CAM2, false);

    uint32_t bufsize = 0;
    CAMU_GetMaxBytes(&bufsize, VGA_WIDTH, VGA_HEIGHT);
    CAMU_SetTransferBytes(PORT_BOTH, bufsize, VGA_WIDTH, VGA_HEIGHT);
  
    CAMU_ClearBuffer(PORT_BOTH);
    CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
    
    return (uint16_t) bufsize;
}
