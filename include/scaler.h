#pragma once

#include <stdint.h>
#include <stddef.h>

#include <memory.h>

extern uint16_t scale_x_map[400];
extern uint16_t scale_y_map[240];

void init_scaler(void);

//void inline vga_to_3ds_scale(uint8_t *src, uint8_t *dst, uint8_t pixel_width);


void inline __attribute__((always_inline)) vga_to_3ds_scale(uint8_t *src, uint8_t *dst, uint8_t pixel_width) {
    for(uint16_t y = 0; y < 240; y++) {
        for(uint16_t x = 0; x < 400; x++) {
            size_t scaled_x = scale_x_map[x];
            size_t scaled_y = scale_y_map[y];

            memcpy(dst + ((y * 400 + x) * pixel_width), src + ((scaled_y * 640 + scaled_x) * pixel_width), pixel_width);
        }
    }
};

void inline __attribute__((always_inline)) vga_to_3ds_scale_fb(uint8_t *src, uint8_t *dst, uint8_t pixel_width) {
    for(uint16_t y = 0; y < 240; y++) {
        for(uint16_t x = 0; x < 400; x++) {
            size_t scaled_x = scale_x_map[x];
            size_t scaled_y = scale_y_map[y];
            
            size_t draw_x = 239 - y;
            size_t draw_y = x;
            
            size_t draw = (draw_x + draw_y * 240) * pixel_width;
            

            memcpy(dst + draw, src + ((scaled_y * 640 + scaled_x) * pixel_width), pixel_width);
        }
    }
};

//void simple_scale(uint8_t *src, uint8_t *dst, size_t src_width, size_t src_height, size_t dst_width, size_t dst_height, uint8_t pixel_width);
