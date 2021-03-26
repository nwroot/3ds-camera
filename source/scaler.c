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
/*
extern void inline vga_to_3ds_scale(uint8_t *src, uint8_t *dst, uint8_t pixel_width) {
    for(uint16_t y = 0; y < 240; y++) {
        for(uint16_t x = 0; x < 400; x++) {
            size_t scaled_x = scale_x_map[x];
            size_t scaled_y = scale_y_map[y];

            memcpy(dst + ((y * 400 + x) * pixel_width), src + ((scaled_y * 640 + scaled_x) * pixel_width), pixel_width);
        }
    }
};
*/
