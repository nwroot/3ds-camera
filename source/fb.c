#include "fb.h"

#include <stddef.h>

extern void write_to_fb(uint8_t *src, uint8_t *dst, uint8_t pixel_width);
/*
void inline write_to_fb(uint8_t *src, uint8_t *dst, uint8_t pixel_width) {
    for(uint16_t y = 0; y < 240; y++) {
        for(uint16_t x = 0; x < 400; x++) {
            uint16_t draw_x = 239 - y;
            uint16_t draw_y = x;
            size_t draw = (draw_x + draw_y * 240) * pixel_width;
            
            memcpy(dst + draw, src + ((y * 400 + x) * pixel_width), pixel_width);

        }
    }
}
*/
