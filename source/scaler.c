#include "scaler.h"

uint16_t scale_x_map[400];
uint16_t scale_y_map[240];

void init_scaler(void) {
    float x_scale = 640.0 / 400.0;
    float y_scale = 480.0 / 240.0;
    for(uint16_t i = 0; i < 400; i++) {
        scale_x_map[i] = (uint16_t) i * x_scale;
    }
    for(uint16_t i = 0; i < 240; i++) {
        scale_y_map[i] = (uint16_t) i * y_scale;
    }
}

extern void vga_to_3ds_scale(uint8_t *src, uint8_t *dst, uint8_t pixel_width);
extern void vga_to_3ds_scale_fb(uint8_t *src, uint8_t *dst, uint8_t pixel_width);

