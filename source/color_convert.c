#include "color_convert.h"

void rgb565_to_rgb888(uint8_t *src, uint8_t *dst, size_t width, size_t height) {
    /*
     * Fast conversion
     * RGB565: RRRRRGGGGGGBBBBB (16 bits)
     * RGB888: RRRRRRRRGGGGGGGBBBBBBBB (24 bits)
     * r1/2**5 = r2/2**8
     * r1 * 2**8 / 2**5 = r2
     * so we can use this trick
     * 2**8 / 2**5 = 2**3
     * r1 * 2**3 = r2
     * r1 << 3 = r2
     * 
     * r1/2**6 = r2/2**8
     * r1 * 2**8 / 2**6 = r2
     * same trick
     * 2**8 / 2**6 = 2**2
     * r1 * 2**2 = r2
     * r1 << 2 = r2
     */
    for(size_t y = 0; y < height; y++) {
        for(size_t x = 0; x < width; x++) {
            // endianess matters
            uint16_t rgb565 = 0;
            rgb565 |= (uint16_t) (*(src + (y * width + x) * 2));
            rgb565 |= (uint16_t) (*(src + (y * width + x) * 2 + 1) & 0xFF) << 8;
            
            uint8_t r = (rgb565 >> 11) & 0x1F;
            uint8_t g = (rgb565 >> 5) & 0x3F;
            uint8_t b = rgb565 & 0x1F;
            
            // write 24-bit (3 byte) pixel
            *(dst + ((y * width + x) * 3))     = r << 3;
            *(dst + ((y * width + x) * 3) + 1) = g << 2;
            *(dst + ((y * width + x) * 3) + 2) = b << 3;
            
        }
    }
}

void rgb565_to_bgr888(uint8_t *src, uint8_t *dst, size_t width, size_t height) {
    /*
     * Fast conversion
     * RGB565: RRRRRGGGGGGBBBBB (16 bits)
     * RGB888: RRRRRRRRGGGGGGGBBBBBBBB (24 bits)
     * r1/2**5 = r2/2**8
     * r1 * 2**8 / 2**5 = r2
     * so we can use this trick
     * 2**8 / 2**5 = 2**3
     * r1 * 2**3 = r2
     * r1 << 3 = r2
     * 
     * r1/2**6 = r2/2**8
     * r1 * 2**8 / 2**6 = r2
     * same trick
     * 2**8 / 2**6 = 2**2
     * r1 * 2**2 = r2
     * r1 << 2 = r2
     */
    for(size_t y = 0; y < height; y++) {
        for(size_t x = 0; x < width; x++) {
            // endianess matters
            uint16_t rgb565 = 0;
            rgb565 |= (uint16_t) (*(src + (y * width + x) * 2));
            rgb565 |= (uint16_t) (*(src + (y * width + x) * 2 + 1) & 0xFF) << 8;
            
            uint8_t r = (rgb565 >> 11) & 0x1F;
            uint8_t g = (rgb565 >> 5) & 0x3F;
            uint8_t b = rgb565 & 0x1F;
            
            // write 24-bit (3 byte) pixel
            *(dst + ((y * width + x) * 3))     = b << 3;
            *(dst + ((y * width + x) * 3) + 1) = g << 2;
            *(dst + ((y * width + x) * 3) + 2) = r << 3;
            
        }
    }
}

void rgb888_to_bgr888(uint8_t *src, uint8_t *dst, size_t width, size_t height) {
    for(size_t y = 0; y < height; y++) {
        for(size_t x = 0; x < width; x++) {
            *(dst + ((y * width + x) * 3))     = *(src + ((y * width + x) * 3) + 2);
            *(dst + ((y * width + x) * 3) + 1) = *(src + ((y * width + x) * 3) + 1);
            *(dst + ((y * width + x) * 3) + 2) = *(src + ((y * width + x) * 3));
        }
    }
}


